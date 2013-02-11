///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CStream.h"

namespace Engine
{
    namespace FileSystem
    {
		namespace Streams
		{
			class CEncryptStream : public CStream
			{
				private:

				public:
					
					bool Open		();
					void Close		();
					void Seek		(u64 position);
					void Flush		();
					u64  Position	();
					u64  Length		();
					bool AtEnd		();
					void Read		(u8* buffer, u64 length);
					void Write		(const u8* buffer, u64 length);

			};
		}
	}
}