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

		// General curve functions.
		f32 Bezier3Curve2D(f32 p1x, f32 p2x, f32 p3x, f32 delta);
		f32 Bezier4Curve2D(f32 p1x, f32 p2x, f32 p3x, f32 p4x, f32 delta);

		// Spline implementation.
		// TODO

	}
}