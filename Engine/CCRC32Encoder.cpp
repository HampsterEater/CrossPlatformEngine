///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CDataTransformer.h"
#include "CCRC32Encoder.h"
#include "Memory.h"

#include <stdio.h>

using namespace Engine::Data;
using namespace Engine::Data::Hashing;

CCRC32Encoder::CCRC32Encoder()
{
}

void CCRC32Encoder::Initialize()
{
	_crc = 0xFFFFFFFF;
}

void CCRC32Encoder::AddBuffer(const u8* buffer, u32 size)
{
	for (u32 i = 0; i < size; i++)
	{
		u8 v = buffer[i];
		_crc = (_crc >> 8) ^ CRC32_LOOKUP_TABLE[(_crc & 0xFF) ^ v];
	}	
}

CTransformedData CCRC32Encoder::Calculate()
{
	// Finalize CRC and return.
	_crc ^= 0xFFFFFFFF;

#ifdef BIG_ENDIAN
	return CTransformedData(reinterpret_cast<u8*>(&_crc), 4);
#else
	u8  buff[4];
	u8* crcbuff = reinterpret_cast<u8*>(&_crc);
	
	buff[0] = crcbuff[3];
	buff[1] = crcbuff[2];
	buff[2] = crcbuff[1];
	buff[3] = crcbuff[0];

	return CTransformedData(buff, 4);
#endif
}