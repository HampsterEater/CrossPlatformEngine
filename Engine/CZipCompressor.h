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
	namespace Containers
	{
		template <typename T> class CArray;
	}
	namespace Data
	{
		namespace Compression
		{
			// Compresses a buffer with a zlib compression.
			class CZipCompressor : public CDataTransformer 
			{
				private:
					Engine::Containers::CString _input;

				public:
					CZipCompressor				();

					void Initialize				();
					void AddBuffer				(const u8* buffer, u32 size);
					CTransformedData Calculate	();			
			};

		}
	}
}