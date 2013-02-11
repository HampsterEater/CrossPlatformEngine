///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

namespace Engine
{
	namespace Containers
	{
		class CString;
	}
    namespace Math
    {
		class CPoint
		{
			public:
				f32 X, Y;

				f32				operator[]		(u32 index) const;
				f32 &			operator[]		(u32 index);
				
				bool			Compare			(const CRect & a) const;
				bool			Compare			(const CRect & a, const f32 epsilon) const;
				bool			operator==		(const CRect & a) const;
				bool			operator!=		(const CRect & a) const;

				void			Set				(const f32 x, const f32 y);
				void			Zero			();
				
				CPoint							(f32 x, f32 y);
				CPoint							(const CPoint& r);
				
				bool			Contains		(const CRect& other);
				bool			Overlaps		(const CRect& other);

				CRect			Inflate			(f32 amount);
				CRect			Inflate			(f32 x, f32 y);
				CRect			Deflate			(f32 amount);
				CRect			Deflate			(f32 x, f32 y);
				CRect			Scale			(f32 amount);
				CRect			Scale			(f32 x, f32 y);
				CRect			Offset			(f32 amount);
				CRect			Offset			(f32 x, f32 y);
				CRect			Union			(const CRect& other);

				Engine::Containers::CString	ToString	();
				static bool					FromString	(Engine::Containers::CString str, CRect& a);
	
		};

	}
}