///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CStream.h"
#include "CArray.h"

namespace Engine
{
    namespace FileSystem
    {
		namespace Streams
		{

			// Size of chunks to allocate when we need to increase memory stream's
			// buffer size.
			#define MEMORY_STREAM_CHUNK_SIZE 1024

			class CMemoryStream : public CStream
			{
				private:
					u8*  _buffer;
					u32	 _bufferLength;
					u32	 _length;
					u32	 _position;
					bool _opened;
					
					void ResizeBuffer	(u32 size);

				public:
					
					CMemoryStream		(u32 bufferLength=1024);
					CMemoryStream		(u8* buffer, u32 bufferLength);
					~CMemoryStream		();

					const u8* GetBuffer	();
					u32	 GetBufferLength();		 

					bool Open			();
					void Close			();
					void Seek			(u64 position);
					void Flush			();
					u64  Position		();
					u64  Length			();
					bool AtEnd			();
					void ReadBytes		(u8* buffer, u64 length);
					void WriteBytes		(const u8* buffer, u64 length);

			};

		}
	}
}