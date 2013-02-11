///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

namespace Engine
{
    namespace Math
    {

		class CRandom
		{
			protected:
				u32 _seed;

				static const u32	MAX_RAND  = 0x7FFF;
				static const u32	IEEE_ONE  = 0x3f800000;
				static const u32	IEEE_MASK = 0x007fffff;

			public:
				CRandom(u32 seed = 0);

				void SetSeed	(u32 seed);
				u32  GetSeed	();

				u32	 GetInt		();
				u32	 GetInt		(u32 min, u32 max);

				f32	 GetFloat	();
				f32	 GetFloat	(f32 min, f32 max);
		};

	}
}