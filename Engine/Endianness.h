///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

namespace Engine
{
	namespace Misc
	{

		bool IsBigEndian		();
		bool IsLittleEndian		();
		void SwapEndian			(u8* buffer, u32 size);

		void ToLittleEndian		(u8* buffer, u32 size);
		void ToBigEndian		(u8* buffer, u32 size);

	}
}