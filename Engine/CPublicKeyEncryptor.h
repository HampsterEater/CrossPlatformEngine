///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CDataTransformer.h"
#include "CMemoryStream.h"

#include "External\RSA\RSA\o-RSA\o-RSA.h"

namespace Engine
{
	namespace Data
	{
		namespace Encryption
		{

			// Can be used to encrypt a block of data using a private key.
			class CPublicKeyEncryptor : public CDataTransformer 
			{
				private:
					Bunt _modulus;
					Bunt _public;

					Engine::FileSystem::Streams::CMemoryStream* _stream;

					Bunt CalcBytes	(Bunt x);
					void CharToCrypt(const u8* m, u32 numChars, u8* block, u32 size);

				public:
					static void GenerateKeyPair	(Engine::Containers::CString& pub, Engine::Containers::CString& privKey, u32 keysize=256);

					CPublicKeyEncryptor			(Engine::Containers::CString publicKey);

					void Initialize				();
					void AddBuffer				(const u8* buffer, u32 size);
					CTransformedData Calculate	();			
			};

		}
	}
}