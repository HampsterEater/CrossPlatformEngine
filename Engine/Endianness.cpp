///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "Endianness.h"

bool Engine::Misc::IsBigEndian()
{
#ifdef ENDIAN_BIG
	return true;
#else
	return false;
#endif
}

bool Engine::Misc::IsLittleEndian()
{
#ifdef ENDIAN_LITTLE
	return true;
#else
	return false;
#endif
}

void Engine::Misc::SwapEndian(u8* buffer, u32 size)
{
	for (u32 i = 0; i < (size / 2); i++)
		buffer[i] = buffer[size - (i + 1)];
}

void Engine::Misc::ToLittleEndian(u8* buffer, u32 size)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(buffer, size);
}

void Engine::Misc::ToBigEndian(u8* buffer, u32 size)
{
	if (!Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(buffer, size);
}
