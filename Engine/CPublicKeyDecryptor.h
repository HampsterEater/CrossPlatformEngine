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

			// Can be used to decrypt a block of data using a public key.
			class CPublicKeyDecryptor : public CDataTransformer 
			{
				private:
					Bunt _modulus;
					Bunt _private;

					Engine::FileSystem::Streams::CMemoryStream* _stream;
					
					Bunt CalcBytes				(Bunt x);
					u8   CryptToChar			(const u8* block, u32 size);
					
				public:

					CPublicKeyDecryptor			(Engine::Containers::CString privateKey);

					void Initialize				();
					void AddBuffer				(const u8* buffer, u32 size);
					CTransformedData Calculate	();			
			};

		}
	}
}