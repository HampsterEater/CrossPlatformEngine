///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CMemoryStream.h"
#include "CLog.h"
#include "Memory.h"

using namespace Engine::FileSystem::Streams;

CMemoryStream::CMemoryStream(u32 bufferLength)
{
	_buffer  = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(bufferLength);
	_bufferLength = bufferLength;
	_opened = false;
	_position = 0;
	_length = 0;
	memset(_buffer, 0, bufferLength);
}

CMemoryStream::CMemoryStream(u8* buffer, u32 bufferLength)
{
	_buffer = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(bufferLength);
	_bufferLength = bufferLength;
	_opened = false;
	_position = 0;
	_length = bufferLength;
	memcpy(_buffer, buffer, bufferLength);
}

CMemoryStream::~CMemoryStream()
{
	if (_buffer != NULL)
	{
		Engine::Memory::GetDefaultAllocator()->Free(&_buffer);
		_buffer = NULL;
	}
}

const u8* CMemoryStream::GetBuffer()
{
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	return _buffer;
}

u32 CMemoryStream::GetBufferLength()
{
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	return _bufferLength;
}

void CMemoryStream::ResizeBuffer(u32 size)
{
	if (_bufferLength >= size)
	{
		// We don't actually need to resize for this.
	}
	else if (_bufferLength < size) 
	{
		u8* newBuffer = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(size);
		memset(newBuffer, 0, size);
		memcpy(newBuffer, _buffer, _length);		
		Engine::Memory::GetDefaultAllocator()->Free(&_buffer);

		_buffer = newBuffer;
		_bufferLength = size;
	}
}

bool CMemoryStream::Open()
{
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	LOG_ASSERT_MSG(_opened == false, "Attempt to open already opened memory stream.");
	
	_opened = true;

	return true;
}

void CMemoryStream::Close()
{	
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	LOG_ASSERT_MSG(_opened == true, "Attempt to close already closed memory stream.");
	
	_opened = false;
}

void CMemoryStream::Seek(u64 position)
{	
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	LOG_ASSERT_MSG(_opened == true, "Attempt to access closed memory stream.");
	LOG_ASSERT_MSG(position >= 0 && position < _length, "Attempt to seek beyond the bounds of the stream.");
	
	_position = (u32)position;
}

void CMemoryStream::Flush()
{	
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	LOG_ASSERT_MSG(_opened == true, "Attempt to access closed memory stream.");
	
	// Memory stream dosen't have to flush!
}

u64 CMemoryStream::Position()
{	
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	LOG_ASSERT_MSG(_opened == true, "Attempt to access closed memory stream.");

	return _position;
}

u64 CMemoryStream::Length()
{	
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	LOG_ASSERT_MSG(_opened == true, "Attempt to access closed memory stream.");

	return _length;
}

bool CMemoryStream::AtEnd()
{	
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	LOG_ASSERT_MSG(_opened == true, "Attempt to access closed memory stream.");

	return Position() >= Length();
}

void CMemoryStream::ReadBytes(u8* buffer, u64 length)
{	
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	LOG_ASSERT_MSG(_opened == true, "Attempt to access closed memory stream.");
	LOG_ASSERT_MSG(_position + length <= _length, "Attempt to read past end of memory stream.");

	memcpy(buffer, _buffer + _position, (u32)length);

	_position += (u32)length;
}

void CMemoryStream::WriteBytes(const u8* buffer, u64 length)
{	
	LOG_ASSERT_MSG(_buffer != NULL, "Attempt to access disposed memory stream.");
	LOG_ASSERT_MSG(_opened == true, "Attempt to access closed memory stream.");

	if (_position + length >= _length)
	{
		ResizeBuffer((u32)(_position + length + MEMORY_STREAM_CHUNK_SIZE));
		_length = _position + (u32)length;
	}

	memcpy(_buffer + _position, buffer, (u32)length); 

	_position += (u32)length;
}