///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CDataTransformer.h"
#include "CPublicKeyEncryptor.h"

#include <cstring>

using namespace Engine::Data;
using namespace Engine::Data::Encryption;

void CPublicKeyEncryptor::GenerateKeyPair(Engine::Containers::CString& pubKey, Engine::Containers::CString& privKey, u32 keysize)
{
	Bint priv; //d
	Bunt pub, mod; //e n
	o_RSA::generateKeys(priv, pub, mod, keysize);

	pubKey  = bigUnsignedToEngineString(mod) + "\n" + bigUnsignedToEngineString(pub);
	privKey = bigUnsignedToEngineString(mod) + "\n" + bigIntegerToEngineString(priv);
}

CPublicKeyEncryptor::CPublicKeyEncryptor(Engine::Containers::CString publicKey)
{
	u32 index = publicKey.IndexOf('\n');

	Engine::Containers::CString x = publicKey.SubString(0, index - 1).Trim();
	Engine::Containers::CString y = publicKey.SubString(index + 1).Trim();
	_modulus = engineStringToBigUnsigned(x);
	_public = engineStringToBigUnsigned(y);
}

Bunt CPublicKeyEncryptor::CalcBytes(Bunt x)
{
	if (x == 0)
		return 1;

	Bunt bits = 1;
	Bunt comp = 1;
	while(x >= comp)
	{
		bits++;
		comp *= 2;
	}

	bits--;
	return (bits-1)/8 + 1;
}

//c = m^e(mod n)
void CPublicKeyEncryptor::CharToCrypt(const u8* m, u32 numChars, u8* block, u32 size)
{
	for(u32 j = 0; j < numChars; j++)
	{
		Bint temp((u32)*(m+j));
		Bunt x = modexp(temp, _public, _modulus);
		for(u32 i = 0; i < size; i ++)
		{
			*(block+size*j+i) = 0;
		}

		Bunt rem;
		for(u32 i = 1; x > 0; i++)
		{
			rem = x%256;
			*(block+(j+1)*size-i) = (u8)rem.toUnsignedInt();
			if(rem > x)
				x = 0;
			else
				x = (x-rem)/256;
		}
	}
}

void CPublicKeyEncryptor::Initialize()
{
	_stream = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::FileSystem::Streams::CMemoryStream>(2048);
	_stream->Open();
}

void CPublicKeyEncryptor::AddBuffer(const u8* buffer, u32 size)
{
	u32 bufferSize = 512;
	u32 bSize	   = CalcBytes(_modulus).toUnsignedLong();
	u8* block	   = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(bSize * bufferSize);

	u32 bytesRemaining = size;
	u32 offset = 0;
	while (bytesRemaining > 0)
	{
		u32 chunk = min(bytesRemaining, bufferSize);

		CharToCrypt(buffer + offset, chunk, block, bSize);
		_stream->WriteBytes(block, bSize * chunk);

		offset += chunk;
		bytesRemaining -= chunk;
	}

	Engine::Memory::GetDefaultAllocator()->Free(&block);
}  

CTransformedData CPublicKeyEncryptor::Calculate()
{
	CTransformedData data = CTransformedData(_stream->GetBuffer(), (u32)_stream->Length());
	_stream->Close();
	Engine::Memory::GetDefaultAllocator()->FreeObj(&_stream);	
	return data;
}