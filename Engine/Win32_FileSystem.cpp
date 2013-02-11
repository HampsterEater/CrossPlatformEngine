///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

namespace Engine
{
    namespace Platform
    {

        // ----------------------------------------------------------------------------
        // The following is one massive clusterfuck of code that implements all the 
		// platform framework - This should probably be split into multiple sections
		// and placed in different files?
        // ----------------------------------------------------------------------------

		bool FileOpen(FileHandle* file, const Engine::Containers::CString& realpath, FileAccessMode accessMode, FileOpenMode openMode)
        {
            LOG_ASSERT_MSG(file != NULL, "File handle passed was NULL.");

            // Normalize the path!
            Engine::Containers::CString path = PathNormalize(realpath);

            // Create and return teh file!
            file->_filePath = path;
            file->_fileHandle = CreateFile(path.c_str(),
                                          accessMode == FILE_ACCESS_MODE_READ ? GENERIC_READ :
                                            accessMode == FILE_ACCESS_MODE_WRITE ? GENERIC_WRITE :
                                                GENERIC_READ | GENERIC_WRITE,
                                          0,
                                          NULL,
                                          openMode == FILE_OPEN_MODE_CREATE_ALWAYS ? CREATE_ALWAYS :
                                            openMode == FILE_OPEN_MODE_CREATE_NEW ? CREATE_NEW :
                                                openMode == FILE_OPEN_MODE_OPEN_ALWAYS ? OPEN_ALWAYS :
                                                    openMode == FILE_OPEN_MODE_OPEN_EXISTING ? OPEN_EXISTING :
                                                        openMode == FILE_OPEN_MODE_TRUNCATE ? TRUNCATE_EXISTING :
                                                            CREATE_ALWAYS,
                                          FILE_ATTRIBUTE_NORMAL,
                                          NULL);

            if (file->_fileHandle ==  INVALID_HANDLE_VALUE)
            {
                u32 ec = GetLastError();
                LOG_ERROR(S("Failed to open file '%s', GetLastError()=%i (%s)").Format(path.c_str(), ec, FormatSystemError(ec).c_str()));
                return false;
            }

            return true;
        }

        void FileClose(FileHandle* file)
        {
            LOG_ASSERT_MSG(file != NULL && file->_fileHandle != INVALID_HANDLE_VALUE, "File handle passed was NULL.");

            FileFlush(file);
            CloseHandle(file->_fileHandle);

            file->_filePath = "";
            file->_fileHandle = NULL;
        }

        bool FileRead(FileHandle* file, u8* buffer, u64 size)
        {
            LOG_ASSERT_MSG(file != NULL && file->_fileHandle != INVALID_HANDLE_VALUE, "File handle passed was NULL.");

            DWORD bytesToRead = (DWORD)size;
            DWORD bytesRead = 0;
            DWORD totalBytesRead = 0;
            while (totalBytesRead < bytesToRead)
            {
                u8* offsetBuffer = (buffer + totalBytesRead);
                DWORD offsetRead = bytesToRead - totalBytesRead;

                BOOL ret = ReadFile(file->_fileHandle, (LPVOID)offsetBuffer, offsetRead, &bytesRead, NULL);
                if (ret == FALSE)
                {
                    u32 ec = GetLastError();
                    LOG_ERROR(S("Failed to read data from file '%s', GetLastError()=%i (%s)").Format(file->_filePath.c_str(), ec, FormatSystemError(ec).c_str()));

                    return false;
                }

                totalBytesRead += bytesRead;
            }

            return true;
        }

        bool FileWrite(FileHandle* file, const u8* buffer, u64 size)
        {
            LOG_ASSERT_MSG(file != NULL && file->_fileHandle != INVALID_HANDLE_VALUE, "File handle passed was NULL.");

            DWORD bytesToWrite = (DWORD)size;
            DWORD bytesWrite = 0;
            DWORD totalBytesWrite = 0;
            while (totalBytesWrite < bytesToWrite)
            {
                const u8* offsetBuffer = (buffer + totalBytesWrite);
                DWORD offsetWrite = bytesToWrite - totalBytesWrite;

                BOOL ret = WriteFile(file->_fileHandle, (LPVOID)offsetBuffer, offsetWrite, &bytesWrite, NULL);
                if (ret == FALSE)
                {
                    u32 ec = GetLastError();
                    LOG_ERROR(S("Failed to write data to file '%s', GetLastError()=%i (%s)").Format(file->_filePath.c_str(), ec, FormatSystemError(ec).c_str()));

                    return false;
                }

                totalBytesWrite += bytesWrite;
            }

            return true;
        }

        u64 FilePos(FileHandle* file)
        {
            LOG_ASSERT_MSG(file != NULL && file->_fileHandle != INVALID_HANDLE_VALUE, "File handle passed was NULL.");

            LARGE_INTEGER offset;
            offset.QuadPart = 0;

            LARGE_INTEGER newPtr;
            BOOL ret = SetFilePointerEx(file->_fileHandle,
                                        offset,
                                        &newPtr,
                                        FILE_CURRENT);

            if (ret == FALSE)
            {
                u32 ec = GetLastError();
                LOG_ERROR(S("Failed to set file position of '%s', GetLastError()=%i (%s)").Format(file->_filePath.c_str(), ec, FormatSystemError(ec).c_str()));

                return 0;
            }

            return (u64)newPtr.QuadPart;
        }

        u64 FileLen(FileHandle* file)
        {
            LOG_ASSERT_MSG(file != NULL && file->_fileHandle != INVALID_HANDLE_VALUE, "File handle passed was NULL.");

            // Store original position in file.
            u64 originalPosition = FilePos(file);

            // Get position 0 bytes away from the end (the length lol).
            LARGE_INTEGER realOffset;
            realOffset.QuadPart = 0;
            LARGE_INTEGER newPtr;
            BOOL ret = SetFilePointerEx(file->_fileHandle,
                                        realOffset,
                                        &newPtr,
                                        FILE_END);

            if (ret == FALSE)
            {
                u32 ec = GetLastError();
                LOG_ERROR(S("Failed to get file length of '%s', GetLastError()=%i (%s)").Format(file->_filePath.c_str(), ec, FormatSystemError(ec).c_str()));

                return 0;
            }

            // Reset to original position.
            FileSeek(file, originalPosition);

            return newPtr.QuadPart;
        }

        bool FileSeek(FileHandle* file, u64 offset)
        {
            LOG_ASSERT_MSG(file != NULL && file->_fileHandle != INVALID_HANDLE_VALUE, "File handle passed was NULL.");

            LARGE_INTEGER realOffset;
            realOffset.QuadPart = offset;

            LARGE_INTEGER newPtr;
            BOOL ret = SetFilePointerEx(file->_fileHandle,
                                        realOffset,
                                        &newPtr,
                                        FILE_BEGIN);

            if (ret == FALSE)
            {
                u32 ec = GetLastError();
                LOG_ERROR(S("Failed to seek in file '%s', GetLastError()=%i (%s)").Format(file->_filePath.c_str(), ec, FormatSystemError(ec).c_str()));

                return false;
            }

            return true;
        }

        bool FileEOF(FileHandle* file)
        {
            LOG_ASSERT_MSG(file != NULL && file->_fileHandle != INVALID_HANDLE_VALUE, "File handle passed was NULL.");
            return FilePos(file) >= FileLen(file);
        }

        void FileFlush(FileHandle* file)
        {
            LOG_ASSERT_MSG(file != NULL && file->_fileHandle != INVALID_HANDLE_VALUE, "File handle passed was NULL.");
            FlushFileBuffers(file->_fileHandle);
        }

        bool DirOpen(DirHandle* dir, const Engine::Containers::CString& realpath)
        {
            LOG_ASSERT_MSG(dir != NULL, "File handle passed was NULL.");

            // Normalize the path!
            Engine::Containers::CString path = PathNormalize(realpath) + "\\*";

            // Store info.
            dir->_dirPath = path;
            dir->_dirAtEnd = false;

            // Open up the directory.
            dir->_dirHandle = FindFirstFile(path.c_str(), &dir->_dirDataHandle);
            if (dir->_dirHandle == INVALID_HANDLE_VALUE)
                return false;

            return true;
        }

        Engine::Containers::CString DirNext(DirHandle* dir)
        {
            LOG_ASSERT_MSG(dir != NULL && dir->_dirHandle != INVALID_HANDLE_VALUE, "File handle passed was NULL.");

            if (dir->_dirHandle == INVALID_HANDLE_VALUE || dir->_dirAtEnd == true)
            {
                return "";
            }

            Engine::Containers::CString path = dir->_dirDataHandle.cFileName;

            if (dir->_dirHandle != INVALID_HANDLE_VALUE)
            {
                if (FindNextFile(dir->_dirHandle, &dir->_dirDataHandle) == 0)
                {
                    dir->_dirAtEnd = true;
                }
            }

            // .. or . directory? Try next directory.
            while (path == "." || path == "..")
				path = DirNext(dir);

            return path;
        }

        void DirClose(DirHandle* dir)
        {
            LOG_ASSERT_MSG(dir != NULL && dir->_dirHandle != INVALID_HANDLE_VALUE, "File handle passed was NULL.");

            FindClose(dir->_dirHandle);

            dir->_dirPath = "";
            dir->_dirHandle = NULL;
        }

        Engine::Containers::CString PathNormalize(const Engine::Containers::CString& realpath)
        {
			Engine::Containers::CString path = realpath;

            // Replace forward slashes with back slashes!
            path = path.Replace("/", "\\").Replace("\\\\", "\\");

            // Add root path if relative.
            if (PathIsRelative(path))
                path = PathJoin(PathGetWorkingDir(), path);

            // Replace any .. or . special paths :3.
            if (path.IndexOf('.') > 0)
            {
                u8 buffer[MAX_PATH];
                GetFullPathName(path.c_str(), MAX_PATH, buffer, NULL);
                path = Engine::Containers::CString(buffer);
            }

            // Trim off ending slashes, they aren't important.
            path = path.TrimEnd("\\", true);

            return path;
        }

        Engine::Containers::CString PathJoin(const Engine::Containers::CString& a, const Engine::Containers::CString& b)
        {
            if (a.Length() == 0)
                return b;

            if (b.Length() == 0)
                return a;

            return a + (a[a.Length() - 1] == '\\' || b[0] == '\\' ? "" : "\\") + b;
        }

        Engine::Containers::CString PathFileName(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            s32 slash = path.LastIndexOf("\\");
            if (slash >= 0)
            {
                return path.SubString(slash + 1);
            }

            return path;
        }

        Engine::Containers::CString PathDirectoryName(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            s32 slash = path.LastIndexOf("\\");
            if (slash >= 0)
            {
                return path.SubString(0, slash);
            }

            return path;
        }

        Engine::Containers::CString PathExtension(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            s32 idx = path.LastIndexOf('.');
            if (idx >= 0 && (u32)(idx + 2) < path.Length())
            {
                return path.SubString(idx + 1);
            }

            return "";
        }

        bool PathExists(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            DWORD flags = GetFileAttributes(path.c_str());
            if (flags == INVALID_FILE_ATTRIBUTES)
                return false;

            return (flags != INVALID_FILE_ATTRIBUTES);
        }

        bool PathIsDirectory(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            DWORD flags = GetFileAttributes(path.c_str());
            if (flags == INVALID_FILE_ATTRIBUTES)
                return false;

            return ((flags & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
        }

        bool PathIsRoot(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            DWORD flags = GetFileAttributes(path.c_str());
            if (flags == INVALID_FILE_ATTRIBUTES)
                return false;

            return ((flags & FILE_ATTRIBUTE_DEVICE) != FILE_ATTRIBUTE_DEVICE);
        }

        bool PathIsFile(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            DWORD flags = GetFileAttributes(path.c_str());
            if (flags == INVALID_FILE_ATTRIBUTES)
                return false;

            return ((flags & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY);
        }

        bool PathIsRelative(const Engine::Containers::CString& path)
        {
            if (path.Length() <= 1)
                return true;

            return (path[1] != ':');
        }

        bool PathIsAbsolute(const Engine::Containers::CString& path)
        {
            if (path.Length() <= 1)
                return false;

            return (path[1] == ':');
        }

        Engine::Containers::CString PathGetWorkingDir()
        {
            u8 path[MAX_PATH];
            if (GetCurrentDirectory(MAX_PATH, path) == 0)
            {
                u32 ec = GetLastError();
                LOG_ERROR(S("Failed to retrieve current working directory, GetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));
                return "";
            }

            return Engine::Containers::CString(path);
        }

        bool PathSetWorkingDir(const Engine::Containers::CString& b)
        {
            return (SetCurrentDirectory(b.c_str()) != 0);
        }

        bool PathSetTime(const Engine::Containers::CString& realpath, u32 access, u32 modified, u32 creation)
        {
            // Normalise and open file.
            Engine::Containers::CString path = PathNormalize(realpath);
            HANDLE fileHandle = CreateFile(path,
                                          FILE_WRITE_ATTRIBUTES,
                                          0,
                                          NULL,
                                          OPEN_EXISTING,
                                          FILE_ATTRIBUTE_NORMAL,
                                          NULL);

            if (fileHandle ==  INVALID_HANDLE_VALUE)
            {
                u32 ec = GetLastError();
                LOG_ERROR(S("Failed to open file '%s', GetLastError()=%i (%s)").Format(path.c_str(), ec, FormatSystemError(ec).c_str()));
                return false;
            }

            // Convert to win32 filetimes and set!
            FILETIME creationTime = UnixTimeToFileTime(creation);
            FILETIME modifyTime   = UnixTimeToFileTime(modified);
            FILETIME accessTime   = UnixTimeToFileTime(access);
            BOOL ret = SetFileTime(fileHandle, creation == 0 ? NULL : &creationTime, access == 0 ? NULL : &accessTime, modified == 0 ? NULL : &modifyTime);

            if (ret == FALSE)
            {
                DWORD ec = GetLastError();
                LOG_ERROR(S("Failed to set file time, GetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));

                CloseHandle(fileHandle);
                return false;
            }

            CloseHandle(fileHandle);
            return true;
        }

        bool PathGetTime(const Engine::Containers::CString& path, u32& access, u32& modified, u32& creation)
        {
            access   = 0;
            modified = 0;
            creation = 0;

            FileHandle file;
            if (!FileOpen(&file, path, FILE_ACCESS_MODE_READ, FILE_OPEN_MODE_OPEN_EXISTING))
            {
                u32 ec = GetLastError();
                LOG_ERROR(S("Failed to open file '%s', GetLastError()=%i (%s)").Format(path.c_str(), ec, FormatSystemError(ec).c_str()));

                return false;
            }

            FILETIME creationTime, modifyTime, accessTime;
            BOOL ret = GetFileTime(file._fileHandle, &creationTime, &accessTime, &modifyTime);
            if (ret == FALSE)
            {
                DWORD ec = GetLastError();
                LOG_ERROR(S("Failed to get file time, GetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));

                FileClose(&file);
                return false;
            }

            access   = FileTimeToUnixTime(accessTime);
            modified = FileTimeToUnixTime(modifyTime);
            creation = FileTimeToUnixTime(creationTime);

            FileClose(&file);

            return true;
        }

        bool PathSetFlags(const Engine::Containers::CString& realpath, IOPathFlags mode)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            DWORD attr = 0;
            if ((mode & PATH_FLAG_READ_ONLY) == PATH_FLAG_READ_ONLY)
            {
                attr |= FILE_ATTRIBUTE_READONLY;
            }

            return (SetFileAttributes(path, attr) == TRUE);
        }

        IOPathFlags PathGetFlags(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            DWORD attr = GetFileAttributes(path);
            if (attr == INVALID_FILE_ATTRIBUTES)
            {
                LOG_ERROR(S("GetFileAttributes on file (%s) failed!").Format(path.c_str()));
                return (IOPathFlags)0;
            }

            unsigned int flags = 0;
            if ((attr & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY)
            {
                flags = flags | PATH_FLAG_READ_ONLY;
            }

            return (IOPathFlags)flags;
        }

        bool PathSetMode(const Engine::Containers::CString& realpath, u32 mode)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            u32 ret = _chmod(path, mode);
            if (ret != 0)
            {
                LOG_ERROR(S("chmod_ on file (%s) failed!").Format(path.c_str()));
                return false;
            }

            return true;
        }

        u32 PathGetMode(const Engine::Containers::CString& realpath)
        {
            struct _stat st;

            Engine::Containers::CString path = PathNormalize(realpath);

            u32 ret = _stat(path, &st);
            if (ret != 0)
            {
                LOG_ERROR(S("stat_ on file (%s) failed!").Format(path.c_str()));
                return false;
            }

            return st.st_mode & 511;
        }

        Engine::Containers::CArray<Engine::Containers::CString> PathGetRoots()
        {
            DWORD drives = GetLogicalDrives();
            DWORD mask = 1;

            Engine::Containers::CArray<Engine::Containers::CString> list;

            for (u32 bit = 0; bit < 24; bit++)
            {
                if (drives & mask)
                {
                    Engine::Containers::CString drive = S((u8)('A' + bit)) + ":";
                    list.AddToEnd(drive);
                }
                mask <<= 1;
            }

            return list;
        }

        u64 PathGetRootFreeSpace(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            ULARGE_INTEGER free;
            ULARGE_INTEGER total;
            ULARGE_INTEGER totalfree;

            free.QuadPart = 0;
            total.QuadPart = 0;
            totalfree.QuadPart = 0;

            if (GetDiskFreeSpaceEx(path.c_str(), &free, &total, &totalfree) == 0)
            {
                DWORD ec = GetLastError();
                if (ec != 21) // 21=device not ready (returned when we try and get space for cd-drives
                {
                    LOG_ERROR(S("GetDiskFreeSpaceEx (%s) failed, GetLastError()=%i (%s)").Format(path.c_str(), ec, FormatSystemError(ec).c_str()));
                    return 0;
                }
            }

            return totalfree.QuadPart;
        }

        u64 PathGetRootUsedSpace(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            ULARGE_INTEGER free;
            ULARGE_INTEGER total;
            ULARGE_INTEGER totalfree;

            free.QuadPart = 0;
            total.QuadPart = 0;
            totalfree.QuadPart = 0;

            if (GetDiskFreeSpaceEx(path.c_str(), &free, &total, &totalfree) == 0)
            {
                DWORD ec = GetLastError();
                if (ec != 21) // 21=device not ready (returned when we try and get space for cd-drives
                {
                    LOG_ERROR(S("GetDiskFreeSpaceEx (%s) failed, GetLastError()=%i (%s)").Format(path.c_str(), ec, FormatSystemError(ec).c_str()));
                    return 0;
                }
            }

            return total.QuadPart - totalfree.QuadPart;
        }

        u64 PathGetRootTotalSpace(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            ULARGE_INTEGER free;
            ULARGE_INTEGER total;
            ULARGE_INTEGER totalfree;

            free.QuadPart = 0;
            total.QuadPart = 0;
            totalfree.QuadPart = 0;

            if (GetDiskFreeSpaceEx(path.c_str(), &free, &total, &totalfree) == 0)
            {
                DWORD ec = GetLastError();
                if (ec != 21) // 21=device not ready (returned when we try and get space for cd-drives
                {
                    LOG_ERROR(S("GetDiskFreeSpaceEx (%s) failed, GetLastError()=%i (%s)").Format(path.c_str(), ec, FormatSystemError(ec).c_str()));
                    return 0;
                }
            }

            return total.QuadPart;
        }

		Engine::Containers::CString PathGetSpecialDir(SpecialDirectory id)
		{
			s32 csidl;
			switch (id)
			{
			case SPECIAL_DIR_DESKTOP:	csidl = CSIDL_DESKTOP;	break;
			case SPECIAL_DIR_APPDATA:	csidl = CSIDL_APPDATA;	break;
			default:					return "";
			}

			u8 path[MAX_PATH];
			if (SHGetFolderPath(NULL, csidl, NULL, SHGFP_TYPE_CURRENT, path) != S_OK)
			{
                DWORD ec = GetLastError();
                LOG_ERROR(S("SHGetFolderLocation failed, GetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));
                return false;
			}

			return path;
		}

        bool FileDelete(const Engine::Containers::CString& realpath)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            if (DeleteFile(path.c_str()) == 0)
            {
                DWORD ec = GetLastError();
                LOG_ERROR(S("DeleteFile (%s) failed, GetLastError()=%i (%s)").Format(path.c_str(), ec, FormatSystemError(ec).c_str()));
                return false;
            }

            return true;
        }

        bool FileRename(const Engine::Containers::CString& realsrc, const Engine::Containers::CString& realdst)
        {
            Engine::Containers::CString src = PathNormalize(realsrc);
            Engine::Containers::CString dst = PathNormalize(realdst);

            if (MoveFile(src.c_str(), dst.c_str()) == 0)
            {
                DWORD ec = GetLastError();
                LOG_ERROR(S("FileRename (%s -> %s) failed, GetLastError()=%i (%s)").Format(src.c_str(), dst.c_str(), ec, FormatSystemError(ec).c_str()));
                return false;
            }

            return true;
        }

        bool FileCopy(const Engine::Containers::CString& realsrc, const Engine::Containers::CString& realdst)
        {
            Engine::Containers::CString src = PathNormalize(realsrc);
            Engine::Containers::CString dst = PathNormalize(realdst);

            if (CopyFile(src.c_str(), dst.c_str(), FALSE) == 0)
            {
                DWORD ec = GetLastError();
                LOG_ERROR(S("FileCopy (%s -> %s) failed, GetLastError()=%i (%s)").Format(src.c_str(), dst.c_str(), ec, FormatSystemError(ec).c_str()));
                return false;
            }

            return true;
        }

        bool FileCreate(const Engine::Containers::CString& path)
        {
            FileHandle file;
            if (!FileOpen(&file, path, FILE_ACCESS_MODE_WRITE, FILE_OPEN_MODE_OPEN_ALWAYS))
            {
                LOG_ERROR(S("FileCreate (%s) failed, unable to open file.").Format(path.c_str()));
                return false;
            }

            FileClose(&file);
            return true;
        }

        u64 FileSize(const Engine::Containers::CString& realpath)
        {
            struct _stati64 st;

            Engine::Containers::CString path = PathNormalize(realpath);

            u32 ret = _stati64(path, &st);
            if (ret != 0)
            {
                LOG_ERROR(S("stat_ on file (%s) failed!").Format(path.c_str()));
                return false;
            }

            return st.st_size;
        }

        bool DirDelete(const Engine::Containers::CString& realpath, bool recursive)
        {
            Engine::Containers::CString path = PathNormalize(realpath);

            if (recursive == true)
            {
                DirHandle dir;

                // Loop through everything in this directory and delete it too!
                if (!DirOpen(&dir, path))
                {
                    LOG_ERROR(S("DirDelete (%s) failed, unable to open directory!").Format(path.c_str()));
                    return false;
                }
                while (true)
                {
                    Engine::Containers::CString sub = DirNext(&dir);
                    if (sub == "")
                        break;

                    sub = path + "\\" + sub;

                    if (PathIsFile(sub) == true)
                    {
                        if (!FileDelete(sub))
                        {
                            LOG_ERROR(S("DirDelete (%s) failed, unable to delete file %s!").Format(path.c_str(), sub.c_str()));
                            return false;
                        }
                    }
                    else if (PathIsDirectory(sub) == true)
                    {
                        if (!DirDelete(sub, true))
                        {
                            LOG_ERROR(S("DirDelete (%s) failed, unable to delete directory %s!").Format(path.c_str(), sub.c_str()));
                            return false;
                        }
                    }
                }
                DirClose(&dir);

                // Finally remove the directory!
                if (!DirDelete(path, false))
                {
                    LOG_ERROR(S("DirDelete (%s) failed, unable to delete root path!").Format(path.c_str()));
                    return false;
                }
            }
            else
            {
                if (RemoveDirectory(path.c_str()) == 0)
                {
                    DWORD ec = GetLastError();
                    LOG_ERROR(S("DirDelete (%s) failed, GetLastError()=%i (%s)").Format(path.c_str(), ec, FormatSystemError(ec).c_str()));
                    return false;
                }

                return true;
            }

            return true;
        }

        bool DirCreate(const Engine::Containers::CString& realpath, bool recursive)
        {
            Engine::Containers::CString path = PathNormalize(realpath) + "\\";

            if (recursive == true)
            {
                s32 slashIdx = -1;
                do
                {
                    slashIdx = path.IndexOf('\\', slashIdx + 1);

                    Engine::Containers::CString root = path.SubString(0, slashIdx);
                    if (!PathExists(root) && !PathIsRoot(root) && !DirCreate(root, false))
                    {
                        return false;
                    }
                }
                while (slashIdx >= 0 && slashIdx < (s32)path.Length() - 1);

                return true;
            }
            else
            {
                if (CreateDirectory(path.c_str(), NULL) == 0)
                {
                    DWORD ec = GetLastError();
                    LOG_ERROR(S("DirCreate (%s) failed, GetLastError()=%i (%s)").Format(path.c_str(), ec, FormatSystemError(ec).c_str()));
                    return false;
                }

                return true;
            }
        }

        bool DirCopy(const Engine::Containers::CString& realsrc, const Engine::Containers::CString& realdst, bool recursive)
        {
            Engine::Containers::CString src = PathNormalize(realsrc);
            Engine::Containers::CString dst = PathNormalize(realdst);

            // Make root destination.
            if (!PathIsDirectory(dst))
            {
                if (!DirCreate(dst, true))
                {
                    LOG_ERROR(S("DirCopy (%s -> %s) failed, unable to create destination directory!").Format(src.c_str(), dst.c_str()));
                    return false;
                }
            }

            DirHandle dir;

            // Loop through everything in this directory and delete it too!
            if (!DirOpen(&dir, src))
            {
                LOG_ERROR(S("DirCopy (%s -> %s) failed, unable to open source directory!").Format(src.c_str(), dst.c_str()));
                return false;
            }
            while (true)
            {
                Engine::Containers::CString sub = DirNext(&dir);
                if (sub == "")
                    break;

                Engine::Containers::CString sub_src = src + "\\" + sub;
                Engine::Containers::CString sub_dst = dst + "\\" + sub;

                if (PathIsFile(sub_src) == true)
                {
                    if (!FileCopy(sub_src, sub_dst))
                    {
                        LOG_ERROR(S("DirCopy (%s -> %s) failed, failed to copy file %s!").Format(src.c_str(), dst.c_str(), sub_src.c_str()));
                        return false;
                    }
                }
                else if (PathIsDirectory(sub_src) == true)
                {
                    if (recursive == true)
                    {
                        DirCopy(sub_src, sub_dst, recursive);
                    }
                    else
                    {
                        // Make root destination.
                        if (!PathIsDirectory(sub_dst))
                        {
                            if (!DirCreate(sub_dst, true))
                            {
                                LOG_ERROR(S("DirCopy (%s -> %s) failed, unable to create folder %s!").Format(src.c_str(), dst.c_str(), sub_dst.c_str()));
                                return false;
                            }
                        }
                    }
                }
            }
            DirClose(&dir);

            return true;
        }


	}
}