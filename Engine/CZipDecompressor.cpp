///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CDataTransformer.h"
#include "CZipDecompressor.h"
#include "CLog.h"
#include "Endianness.h"
#include "External\ZLib\zlib.h"
#include "Memory.h"

using namespace Engine::Data;
using namespace Engine::Data::Compression;

CZipDecompressor::CZipDecompressor()
{
	_input = "";
}

void CZipDecompressor::Initialize()
{
	_input = "";
}

void CZipDecompressor::AddBuffer(const u8* buffer, u32 size)
{
	_input += Engine::Containers::CString(buffer, size);
}

CTransformedData CZipDecompressor::Calculate()
{
	Engine::Containers::CString output = "";	
	u32 initialSize = _input.Length();

	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = 0;
	stream.next_in = Z_NULL;

	// Initialize a zlib stream.
	if (inflateInit(&stream) != Z_OK)
	{
		LOG_CRITICAL("inflateInit failed, unable to perform decompression!");
		return CTransformedData("", 0);
	}

	// Calculate the size.
	u32 output_size = reinterpret_cast<u32*>(const_cast<u8*>(_input.c_str()))[0];
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian((u8*)&output_size, 4);

	// Compress our data.
	stream.avail_in = _input.Length();
	stream.next_in = reinterpret_cast<Bytef*>(const_cast<u8*>(_input.c_str() + 4));

	u8* outBuffer = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(output_size);
	do 
	{
		stream.avail_out = output_size;
		stream.next_out	 = reinterpret_cast<Bytef*>(outBuffer);
		LOG_ASSERT(inflate(&stream, Z_NO_FLUSH) != Z_STREAM_ERROR);
	} while (stream.avail_out == 0);

	// Deinitialize zlib.
	CTransformedData transformedData(outBuffer, output_size);
	Engine::Memory::GetDefaultAllocator()->Free(&outBuffer);
	inflateEnd(&stream);

	return transformedData;
}