///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CDataTransformer.h"
#include "CZipCompressor.h"
#include "CLog.h"
#include "Endianness.h"
#include "External\ZLib\zlib.h"
#include "Memory.h"

using namespace Engine::Data;
using namespace Engine::Data::Compression;

CZipCompressor::CZipCompressor()
{
	_input = "";
}

void CZipCompressor::Initialize()
{
	_input = "";
}

void CZipCompressor::AddBuffer(const u8* buffer, u32 size)
{
	_input += Engine::Containers::CString(buffer, size);
}

CTransformedData CZipCompressor::Calculate()
{
	Engine::Containers::CString output = "";	
	u32 initialSize = _input.Length();

	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;

	// Initialize a zlib stream.
	if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK)
	{
		LOG_CRITICAL("deflateInit failed, unable to perform compression!");
		return CTransformedData("", 0);
	}

	// Compress our data.
	stream.avail_in = _input.Length();
	stream.next_in = reinterpret_cast<Bytef*>(const_cast<u8*>(_input.c_str()));

	u32 output_size = initialSize + initialSize / 10 + 32;
	u8* outBuffer = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(output_size);
	do 
	{
		stream.avail_out = output_size;
		stream.next_out	 = reinterpret_cast<Bytef*>(outBuffer);
		LOG_ASSERT(deflate(&stream, Z_FINISH) != Z_STREAM_ERROR);
	} while (stream.avail_out == 0);

	// Prefix initial size to data (little endian).
	Engine::Misc::ToLittleEndian((u8*)&initialSize, 4);
	output = Engine::Containers::CString(reinterpret_cast<u8*>(&initialSize), 4) + outBuffer;

	// Deinitialize zlib.
	Engine::Memory::GetDefaultAllocator()->Free(&outBuffer);
	deflateEnd(&stream);

	return CTransformedData(output.c_str(), output.Length());
}