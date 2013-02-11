///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CVector2.h"

namespace Engine
{
	namespace Containers
	{
		class CString;
	}
    namespace Math
    {
		class CRect;
		class CCircle;

		class CLine
		{
			private:
				u32 Orientation(f32 x1, f32 y1, f32 x2, f32 y2, f32 Px, f32 Py) const; 

			public:
				CVector2 Start;
				CVector2 End;

				CVector2		operator[]		(u32 index) const;
				CVector2 &		operator[]		(u32 index);
				
				bool			Compare			(const CLine & a) const;
				bool			Compare			(const CLine & a, const f32 epsilon) const;
				bool			operator==		(const CLine & a) const;
				bool			operator!=		(const CLine & a) const;

				void			Set				(const f32 x, const f32 y, const f32 x2, const f32 y2);
				void			Set				(const CVector2& start, const CVector2& end);
				void			Zero			();
				
				CLine							(f32 x, f32 y, f32 x2, f32 y2);
				CLine							(const CVector2& start, const CVector2& end);
				CLine							(const CLine& r);

				f32				GetLength		() const;
				
				CVector2		GetCenter		() const;
				CVector2		GetPoint		(f32 delta) const;

				CLine			Rotate			(f32 angle) const;

				bool			Overlaps		(const CLine& other) const;
				bool			Overlaps		(const CRect& other) const;
				bool			Overlaps		(const CCircle& other) const;
				bool			Contains		(const CVector2& other) const;
				bool			IntersectPoint	(const CLine& other, CVector2& a) const;
				bool			IntersectPoint	(const CRect& other, CVector2& a) const;
				CVector2		NearestPoint	(const CVector2& to) const;

				CLine			Inflate			(f32 amount) const;
				CLine			Deflate			(f32 amount) const;
				CLine			Scale			(f32 amount) const;
				CLine			Offset			(f32 amount) const;
				CLine			Offset			(CVector2 size) const;

				Engine::Containers::CString	ToString	();
				static bool					FromString	(Engine::Containers::CString str, CLine& a);
	
		};

	}
}