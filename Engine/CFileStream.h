///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CString.h"
#include "CStream.h"

namespace Engine
{
    namespace FileSystem
    {
		namespace Streams
		{
			class CFileStream : public CStream
			{
				private:
					Engine::Platform::FileHandle		_fileHandle;
					Engine::Containers::CString				_path;
					Engine::Platform::FileAccessMode	_accessMode;
					Engine::Platform::FileOpenMode		_openMode;
					bool								_opened;

				public:

					CFileStream(const Engine::Containers::CString& path, Engine::Platform::FileAccessMode accessMode=Engine::Platform::FILE_ACCESS_MODE_READ_WRITE, Engine::Platform::FileOpenMode openMode=Engine::Platform::FILE_OPEN_MODE_OPEN_ALWAYS);
					~CFileStream();
					
					bool Open		();
					void Close		();
					void Seek		(u64 position);
					void Flush		();
					u64  Position	();
					u64  Length		();
					bool AtEnd		();
					void ReadBytes	(u8* buffer, u64 length);
					void WriteBytes	(const u8* buffer, u64 length);

			};
		}
	}
}