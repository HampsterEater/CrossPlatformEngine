///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CStream.h"

// As the file package streams have to read from the same 'package' stream, streaming
// can not be done normally as you could have multiple threads fighting over reading/writing
// from it. So what we do to allow it, is to use a mutex to control access to the 
// package stream and then read in "chunks", preventing constant fighting. 
// 
// You ideally want to set the cache size high enough that small files (images, etc) will be
// loaded in full from the stream, and then just read out of memory rather than from the disk.
//
// Streaming should only be neccessary on very large files (movies, music, etc), if others are
// being streamed like this, you will end up with problems :).
//
// Writing obviously dosen't work on these streams, it will just cause an assert. You can't
// write to a static file package.
#define FILE_PACKAGE_STREAM_CACHE_SIZE (1024 * 512)

namespace Engine
{
    namespace FileSystem
    {
		namespace Streams
		{
			class CStream;
		}
		namespace Containers
		{
			class CFilePackage;
			
			class CFilePackageStream : public Engine::FileSystem::Streams::CStream
			{
				private:
				
					Engine::Containers::CString _path;
					CFilePackage*		   _package;
					s32					   _chunkIndex;

					u8*  _chunk;
					u64  _chunkStart;
					u64  _chunkLength;

					u64	 _length;
					u64	 _position;
					bool _opened;

					void UpdateChunk();

				public:
					
					CFilePackageStream	(CFilePackage* package, const Engine::Containers::CString& path);
					~CFilePackageStream	();

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