///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CDataTransformer.h"
#include "CPublicKeyDecryptor.h"

#include <cstring>

using namespace Engine::Data;
using namespace Engine::Data::Encryption;

CPublicKeyDecryptor::CPublicKeyDecryptor(Engine::Containers::CString privateKey)
{
	u32 index = privateKey.IndexOf('\n');

	Engine::Containers::CString x = privateKey.SubString(0, index - 1).Trim();
	Engine::Containers::CString y = privateKey.SubString(index + 1).Trim();
	_modulus = engineStringToBigUnsigned(x);
	_private = engineStringToBigUnsigned(y);
}

void CPublicKeyDecryptor::Initialize()
{
	_stream = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::FileSystem::Streams::CMemoryStream>(2048);
	_stream->Open();
}

Bunt CPublicKeyDecryptor::CalcBytes(Bunt x)
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

//m = c^d(mod n)
u8 CPublicKeyDecryptor::CryptToChar(const u8* block, u32 size)
{
	Bunt c = 0;
	for(u32 i = 0; i < size; i++)
	{
		c *= 256;
		c += (u32)(unsigned char)*(block+i);
	}
	u8 x = (u8)(modexp(c, _private, _modulus).toInt());
	return x;
}

void CPublicKeyDecryptor::AddBuffer(const u8* buffer, u32 size)
{
	u32 bSize	   = CalcBytes(_modulus).toUnsignedLong();
	u8* block	   = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(bSize);

	u32 bytesRemaining = size;
	u32 offset = 0;
	while (bytesRemaining > 0)
	{
		u8 temp = CryptToChar(buffer + offset, bSize);
		_stream->WriteU8(temp);

		offset += bSize;
		bytesRemaining -= bSize;
	}

	Engine::Memory::GetDefaultAllocator()->Free(&block);
}

CTransformedData CPublicKeyDecryptor::Calculate()
{
	CTransformedData data = CTransformedData(_stream->GetBuffer(), (u32)_stream->Length());
	_stream->Close();
	Engine::Memory::GetDefaultAllocator()->FreeObj(&_stream);	
	return data;
}