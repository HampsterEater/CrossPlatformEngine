///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CRandom.h"
#include "Math.h"

using namespace Engine::Math;

CRandom::CRandom(u32 seed)
{
	SetSeed(seed);
}

void CRandom::SetSeed(u32 seed)
{
	_seed = seed;
}

u32 CRandom::GetSeed()
{
	return _seed;
}

u32	CRandom::GetInt()
{
	_seed = 1664525L * _seed + 1013904223L;
	return ((u32)_seed & MAX_RAND);
}

u32	CRandom::GetInt(u32 min, u32 max)
{
	return min + (GetInt() % (max - min));
}

f32	CRandom::GetFloat()
{
	_seed = 1664525L * _seed + 1013904223L;
	u32 i = IEEE_ONE | (_seed & IEEE_MASK);
	return ((*(float*)&i) - 1.0f);
}

f32	CRandom::GetFloat(f32 min, f32 max)
{
	return min + (GetFloat() * (max - min));
}