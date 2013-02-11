///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "Math.h"
#include <math.h>

using namespace Engine::Math;

f32 Engine::Math::EPSILON		= 1.192092896e-07f;
f32 Engine::Math::INFINITY		= 1e30f;
f32 Engine::Math::PI			= 3.14159265358979f;
f32 Engine::Math::E				= 2.71828182845904f;
f32 Engine::Math::DEG_TO_RAD	= PI / 180.0f;
f32 Engine::Math::RAD_TO_DEG	= 180.0f / PI;

f32	Engine::Math::Lerp(f32 a, f32 b, f32 delta)
{
	if (delta <= 0.0f)
		return a;
	else if (delta >= 1.0f)
		return b;
	else
		return a + ((b - a) * delta);
}

f32 Engine::Math::SmoothStep(f32 a, f32 b, f32 delta)
{
	delta = Clamp((delta - a) / (b - a), 0, 1);
	return delta * delta * delta * (delta * (delta * 6 - 15) + 10);
}

f32	Engine::Math::Abs(f32 v)
{
	return (v >= 0 ? v : -v);
}

s32	Engine::Math::Abs(s32 v)
{
	return (v >= 0 ? v : -v);
}

f32 Engine::Math::Floor(f32 v)
{
	return floor(v);
}

f32 Engine::Math::Ceil(f32 v)
{
	return ceil(v);
}

f32 Engine::Math::Clamp(f32 value, f32 min, f32 max)
{
	if (value < min)
		value = min;
	if (value > max)
		value = max;
	return value;
}

f32	Engine::Math::Sqrt(f32 v)
{
	return sqrt(v);
}

f32 Engine::Math::InvSqrt(f32 v)
{
	return 1.0f / sqrt(v); // Slowest possible way ;_;

	// Not very portable, has a lot of limitations because
	// of the pointer arithmatic and casting. Assumes endianness
	// of float and int are the same.
#if 0
	// The crazy Quake inverse squareroot code :3
	// Fuck if I know how it works, but it does!

   f32 xhalf = 0.5f * x;
   s32 i = *(s32*)&x;				// store floating-point bits in integer

   i = 0x5f3759d5 - (i >> 1);		// initial guess for Newton's method
   x = *(f32*)&i;					// convert new bits into float
   x = x*(1.5f - xhalf*x*x);		// One round of Newton's method

   return x;
#endif
}

f32 Engine::Math::RSqrt(f32 x)
{
	s32 i;
	f32 y, r;

	y = x * 0.5f;
	
	i = *reinterpret_cast<s32 *>( &x );
	i = 0x5f3759df - ( i >> 1 );
	
	r = *reinterpret_cast<f32 *>( &i );
	r = r * ( 1.5f - r * r * y );
	
	return r;
}

f32 Engine::Math::ACos(f32 v)
{
	return acosf(v);
}

f32 Engine::Math::ASin(f32 v)
{
	return asinf(v);
}

f32 Engine::Math::ATan(f32 v)
{
	return atanf(v);
}

f32 Engine::Math::ATan2(f32 x, f32 y)
{
	return atan2f(x, y);
}

f32 Engine::Math::Sin(f32 v)
{
	return sinf(v);
}

f32 Engine::Math::Sinh(f32 v)
{
	return sinhf(v);
}

f32 Engine::Math::Cos(f32 v)
{
	return cosf(v);
}

f32 Engine::Math::Cosh(f32 v)
{
	return coshf(v);
}

f32 Engine::Math::Exp(f32 v)
{
	return expf(v);
}

f32 Engine::Math::Pow(f32 x, f32 y)
{
	return powf(x, y);
}

s32 Engine::Math::Pow(s32 x, s32 y)
{
	return (s32)pow((f32)x, y);
}

f32 Engine::Math::Tan(f32 v)
{
	return tanf(v);
}

f32 Engine::Math::Tanh(f32 v)
{
	return tanhf(v);
}

f32 Engine::Math::Log(f32 v)
{
	return logf(v);
}

s32 Engine::Math::Sign(f32 v)
{
	return (v > 0 ? 1 : (v < 0 ? -1 : 0));
}

f32 Engine::Math::Max(f32 x, f32 y)
{
	return x > y ? x : y;
}

f32 Engine::Math::Min(f32 x, f32 y)
{
	return x < y ? x : y;
}

f32 Engine::Math::ToRad(f32 v)
{
	return v * Engine::Math::DEG_TO_RAD;
}

f32 Engine::Math::ToDeg(f32 v)
{
	return v * Engine::Math::RAD_TO_DEG;
}

bool Engine::Math::IsNan(f32 v)
{
	return (((*(const unsigned long *)&v) & 0x7f800000) == 0x7f800000);
}

bool Engine::Math::IsInf(f32 v)	
{
	return (((*(const unsigned long *)&v) & 0x7fffffff) == 0x7f800000);
}

bool Engine::Math::IsInd(f32 v)
{
	return ((*(const unsigned long *)&v) == 0xffc00000);
}

bool Engine::Math::IsDen(f32 v)
{
	return (((*(const unsigned long *)&v) & 0x7f800000) == 0x00000000 && ((*(const unsigned long *)&v) & 0x007fffff) != 0x00000000);
}

f32	Engine::Math::NormalizeAngle360(f32 a)
{
	if (a >= 360.0f || a < 0.0f)
	{
		a -= floor(a / 360.0f) * 360.0f;
	}
	return a;
}

f32	Engine::Math::NormalizeAngle180(f32 a)
{
	a = NormalizeAngle360(a);
	if (a > 180.0f)
	{
		a -= 360.0f;
	}
	return a;
}

f32	Engine::Math::AngleDelta(f32 a, f32 b)
{
	return NormalizeAngle180(a - b);
}

void Engine::Math::SetBits(u32& dw, s32 lowBit, s32 bits, s32 value)
{
	s32 mask = (1 << bits) - 1;
	dw = (dw & ~(mask << lowBit)) | (value << lowBit);
}
