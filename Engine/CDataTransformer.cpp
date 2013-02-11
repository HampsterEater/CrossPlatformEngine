///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CDataTransformer.h"
#include "CString.h"
#include "CStream.h"
#include "CFileStream.h"
#include "CLog.h"
#include "Memory.h"

#include <stdio.h>

using namespace Engine::Data;

CTransformedData::~CTransformedData()
{
	if (_buffer != NULL)
	{
		Engine::Memory::GetDefaultAllocator()->Free(&_buffer);
	}
}

CTransformedData::CTransformedData()
{
	_buffer = NULL;
	_length = 0;
}

CTransformedData::CTransformedData(const u8* buffer, u32 length)
{
	_buffer = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(length);
	_length = length;

	memcpy(_buffer, buffer, length);
}

CTransformedData::CTransformedData(const CTransformedData& v)
{
	_length = v._length;
	_buffer = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(_length);

	memcpy(_buffer, v._buffer, _length);
}

void CTransformedData::operator=(const CTransformedData &v)
{
	if (_length != v._length)
	{
		_length = v._length;
		if (_buffer != NULL)
		{			
			Engine::Memory::GetDefaultAllocator()->Free(&_buffer);
		}
		_buffer = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(_length);
	}
	memcpy(_buffer, v._buffer, _length);
}

// Operators, delicious operators!
const u8 CTransformedData::operator[] (u32 index) const
{
	LOG_ASSERT(index >= 0 && index < _length);
	return _buffer[index];
}

const u8 CTransformedData::operator[] (s32 index) const
{
	LOG_ASSERT(index >= 0 && index < (s32)_length);
	return _buffer[index];
}

							
bool Engine::Data::operator==(const CTransformedData& a, const CTransformedData& b)
{
	if (a._length != b._length)
		return false;

	for (u32 i = 0; i < a._length; i++)
	{
		if (a._buffer[i] != b._buffer[i])
			return false;
	}

	return true;
}

bool Engine::Data::operator!=(const CTransformedData& a, const CTransformedData& b)
{
	return !(a == b);
}

u32	CTransformedData::Size()
{
	return _length;
}

const u8* CTransformedData::GetBuffer()
{
	return _buffer;
}

Engine::Containers::CString CTransformedData::GetString()
{
	Engine::Containers::CString str(_buffer, _length);
	return str;
}

Engine::Containers::CString CTransformedData::GetHexString()
{
	Engine::Containers::CString str;

	for (u32 i = 0; i < _length; i++)
	{
		u8 buff[3];
		sprintf_s(buff, 3, "%02X", (unsigned char)_buffer[i]); 

		str += Engine::Containers::CString((u8*)buff, 2);
	}

	return str;
}

void CDataTransformer::AddStream(Engine::FileSystem::Streams::CStream* stream)
{
	u8 buffer[1024];

	while (!stream->AtEnd())
	{
		u64 remaining = stream->Length() - stream->Position();
		u64 readAmount = min(remaining, 1024);

		stream->ReadBytes(buffer, readAmount);
		AddBuffer(buffer, (u32)readAmount);
	}
}

CTransformedData CDataTransformer::CalculateBuffer(CDataTransformer* trans, const u8* buffer, u32 size)
{
	trans->Initialize();
	trans->AddBuffer(buffer, size);
	return trans->Calculate();
}

CTransformedData CDataTransformer::CalculateStream(CDataTransformer* trans, Engine::FileSystem::Streams::CStream* stream)
{
	trans->Initialize();
	trans->AddStream(stream);
	return trans->Calculate();
}