///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CFilePackageStream.h"
#include "CFilePackage.h"

using namespace Engine::FileSystem::Containers;

CFilePackageStream::CFilePackageStream(CFilePackage* package, const Engine::Containers::CString& path)
{
	_chunk			= NULL;
	_chunkStart		= -1;
	_chunkLength	= 0;
	_length			= 0;
	_position		= 0;
	_opened			= false;
	_path			= path;
	_package		= package;

	_chunk = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(FILE_PACKAGE_STREAM_CACHE_SIZE);
}

CFilePackageStream::~CFilePackageStream()
{
	if (_chunk != NULL)
	{
		Engine::Memory::GetDefaultAllocator()->Free(&_chunk);
	}
}

bool CFilePackageStream::Open()
{
	LOG_ASSERT_MSG(_opened == false, "Attempt to close already closed package stream.");
	LOG_ASSERT_MSG(_package != NULL, "Attempt to open on a null package.");

	_opened = true;
	_chunkIndex = _package->GetChunkIndex(_path);
	_position = 0;

	if (_chunkIndex >= 0)
		_length = _package->_dictionary[_chunkIndex].Length;

	return (_chunkIndex >= 0);
}

void CFilePackageStream::Close()
{
	LOG_ASSERT_MSG(_opened == true, "Attempt to close already closed package stream.");
	_opened = false;
}

void CFilePackageStream::Seek(u64 position)
{
	LOG_ASSERT_MSG(_opened == true, "Attempt to close already closed package stream.");

	_position = position;
	UpdateChunk();
}

void CFilePackageStream::Flush()
{
	LOG_ASSERT_MSG(_opened == true, "Attempt to close already closed package stream.");
	LOG_ASSERT_MSG(false, "Attempt to write to file package. File package's are read only!");
}

u64 CFilePackageStream::Position()	
{
	LOG_ASSERT_MSG(_opened == true, "Attempt to close already closed package stream.");
	return _position;
}

u64 CFilePackageStream::Length()	
{
	LOG_ASSERT_MSG(_opened == true, "Attempt to close already closed package stream.");
	return _length;
}

bool CFilePackageStream::AtEnd()	
{
	LOG_ASSERT_MSG(_opened == true, "Attempt to close already closed package stream.");
	return Position() >= Length();
}

void CFilePackageStream::UpdateChunk()
{
	u64 chunkStartOffset = (_position / FILE_PACKAGE_STREAM_CACHE_SIZE) * FILE_PACKAGE_STREAM_CACHE_SIZE;
	if (chunkStartOffset != _chunkStart)
	{
		u64 remaining = _length - chunkStartOffset;
		u64 readCount = min(remaining, FILE_PACKAGE_STREAM_CACHE_SIZE);

		// Read in the bytes from the volume :3.
		if (readCount > 0)
		{
			_package->ReadBytes(_chunk, _package->_dictionary[_chunkIndex].Volume, _package->_dictionary[_chunkIndex].Offset + chunkStartOffset, readCount);  
		}

		_chunkLength = readCount;
		_chunkStart = chunkStartOffset;
	}
}

void CFilePackageStream::ReadBytes(u8* buffer, u64 length)
{
	LOG_ASSERT_MSG(_opened == true, "Attempt to close already closed package stream.");	
	LOG_ASSERT_MSG(Position() + length <= _length, "Attempt to read beyond length of package stream.");

	u64 remaining = length;
	u64 offset = 0;
	while (remaining > 0)
	{
		// Make sure we have something to read.
		UpdateChunk();

		// See how much of the chunk we have left to read.
		u64 chunkRemaining = (_chunkStart + _chunkLength) - _position;
		u64 maxRead = min(chunkRemaining, remaining);

		// Read bytes from the package file internally.
		memcpy(buffer + offset, _chunk + (_position - _chunkStart), (size_t)maxRead);

		_position += maxRead;
		remaining -= maxRead;
		offset += maxRead;
	}
}

void CFilePackageStream::WriteBytes(const u8* buffer, u64 length)
{
	LOG_ASSERT_MSG(false, "Attempt to write to file package. File package's are read only!");
}