///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CSpline.h"
#include "Math.h"

using namespace Engine::Math;

f32 Engine::Math::Bezier3Curve2D(f32 p1x, f32 p2x, f32 p3x, f32 delta)
{
	p1x += ((p2x - p1x) * delta);	
	p2x += ((p3x - p2x) * delta);	
	return ((p2x - p1x) * delta) + p1x;	
}

f32 Engine::Math::Bezier4Curve2D(f32 p1x, f32 p2x, f32 p3x, f32 p4x, f32 delta)
{
	p1x += ((p2x - p1x) * delta);				
	p2x += ((p3x - p2x) * delta);				
	p3x += ((p4x - p3x) * delta);				

	p1x += ((p2x - p1x) * delta);				
	p2x += ((p3x - p2x) * delta);				
	
	return ((p2x - p1x) * delta) + p1x;
}
