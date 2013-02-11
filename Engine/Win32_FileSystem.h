///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CString.h"

namespace Engine
{
    namespace Platform
    {

        struct FileHandle
        {
            Engine::Containers::CString  _filePath;
            HANDLE                  _fileHandle;

            friend bool operator==(FileHandle& a, FileHandle& b)
			{
				return a._fileHandle == b._fileHandle;
			}
		};
		 
		struct DirHandle
        {
            Engine::Containers::CString  _dirPath;
            HANDLE                  _dirHandle;
            WIN32_FIND_DATA         _dirDataHandle;
            bool                    _dirAtEnd;

            friend bool operator==(DirHandle& a, DirHandle& b)
			{
				return a._dirHandle == b._dirHandle;
			}
        };

    }
}
