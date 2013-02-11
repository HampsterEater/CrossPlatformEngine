///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CFileStream.h"
#include "CLog.h"

using namespace Engine::FileSystem::Streams;

CFileStream::CFileStream(const Engine::Containers::CString& path, Engine::Platform::FileAccessMode accessMode, Engine::Platform::FileOpenMode openMode)
{
	_path = path;
	_accessMode = accessMode;
	_openMode = openMode;
	_opened = false;
}

CFileStream::~CFileStream()
{
	if (_opened == true)
		Close();
}

bool CFileStream::Open()
{
	LOG_ASSERT_MSG_ARGS(_opened == false, "Attempt to open already opened file stream (%s).", _path.c_str());
	_opened = Engine::Platform::FileOpen(&_fileHandle, _path, _accessMode, _openMode);
	return _opened;
}

void CFileStream::Close()
{
	LOG_ASSERT_MSG_ARGS(_opened == true, "Attempt to access closed file stream (%s).", _path.c_str());
	Engine::Platform::FileClose(&_fileHandle);
	_opened = false;
}

void CFileStream::Seek(u64 position)
{
	LOG_ASSERT_MSG_ARGS(_opened == true, "Attempt to access closed file stream (%s).", _path.c_str());
	LOG_ASSERT_MSG_ARGS(position >= 0 && position < Length(), "Attempt to seek beyond the bounds of the stream (%s).", _path.c_str());
	Engine::Platform::FileSeek(&_fileHandle, position);
}

void CFileStream::Flush()
{
	LOG_ASSERT_MSG_ARGS(_opened == true, "Attempt to access closed file stream (%s).", _path.c_str());
	Engine::Platform::FileFlush(&_fileHandle);
}

u64 CFileStream::Position()
{
	LOG_ASSERT_MSG_ARGS(_opened == true, "Attempt to access closed file stream (%s).", _path.c_str());
	return Engine::Platform::FilePos(&_fileHandle);
}

u64 CFileStream::Length()
{
	LOG_ASSERT_MSG_ARGS(_opened == true, "Attempt to access closed file stream (%s).", _path.c_str());
	return Engine::Platform::FileLen(&_fileHandle);
}

bool CFileStream::AtEnd()
{
	return Position() >= Length();
}

void CFileStream::ReadBytes(u8* buffer, u64 length)
{
	LOG_ASSERT_MSG_ARGS(_opened == true, "Attempt to access closed file stream (%s).", _path.c_str());
	Engine::Platform::FileRead(&_fileHandle, buffer, length);
}

void CFileStream::WriteBytes(const u8* buffer, u64 length)
{
	LOG_ASSERT_MSG_ARGS(_opened == true, "Attempt to access closed file stream (%s).", _path.c_str());
	Engine::Platform::FileWrite(&_fileHandle, buffer, length);
}