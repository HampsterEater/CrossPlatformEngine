///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CDataTransformer.h"
#include "CString.h"

namespace Engine
{
	namespace Data
	{
		namespace Compression
		{
			static u8 BASE64_DECODE_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			// Converts a buffer of data into base64 encoding.
			class CBase64Decoder : public CDataTransformer 
			{
				private:
					Engine::Containers::CString _input;
					
				public:
					CBase64Decoder				();

					void Initialize				();
					void AddBuffer				(const u8* buffer, u32 size);
					CTransformedData Calculate	();			
			};

		}
	}
}