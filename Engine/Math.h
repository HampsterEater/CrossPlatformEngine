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

		extern f32				EPSILON;
		extern f32				INFINITY;
		extern f32				PI;
		extern f32				E;
		extern f32				DEG_TO_RAD;
		extern f32				RAD_TO_DEG;

		f32	Lerp				(f32 a, f32 b, f32 delta);
		f32 SmoothStep			(f32 a, f32 b, f32 delta);

		f32	Abs					(f32 v);
		s32	Abs					(s32 v);
		f32 Floor				(f32 v);
		f32 Ceil				(f32 v);
		f32 Clamp				(f32 value, f32 min, f32 max);

		f32	Sqrt				(f32 v);
		f32 InvSqrt				(f32 v);
		f32 RSqrt				(f32 v);

		f32 ACos				(f32 v);
		f32 ASin				(f32 v);
		f32 ATan				(f32 v);
		f32 ATan2				(f32 x, f32 y);
		f32 Sin					(f32 v);
		f32 Sinh				(f32 v);
		f32 Cos					(f32 v);
		f32 Cosh				(f32 v);
		f32 Exp					(f32 v);
		f32 Pow					(f32 x, f32 y);
		s32 Pow					(s32 x, s32 y);
		f32 Tan					(f32 v);
		f32 Tanh				(f32 v);

		f32 Log					(f32 v);

		s32 Sign				(f32 v);

		f32 Max					(f32 x, f32 y);
		f32 Min					(f32 x, f32 y);

		f32 ToRad				(f32 v);
		f32 ToDeg				(f32 v);

		bool IsNan				(f32 v);
		bool IsInf				(f32 v);
		bool IsInd				(f32 v);
		bool IsDen				(f32 v);

		f32	NormalizeAngle360	(f32 a);
		f32	NormalizeAngle180	(f32 a);
		f32	AngleDelta			(f32 a, f32 b);

		void SetBits			(u32& dw, s32 lowBit, s32 bits, s32 value);

	}
}

