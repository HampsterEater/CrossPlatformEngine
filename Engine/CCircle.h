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
		class CVector2;
		class CRect;
		class CLine;

		class CCircle
		{
			public:
				f32 X, Y;
				f32 Radius;

				f32				operator[]		(u32 index) const;
				f32 &			operator[]		(u32 index);
				
				bool			Compare			(const CCircle & a) const;
				bool			Compare			(const CCircle & a, const f32 epsilon) const;
				bool			operator==		(const CCircle & a) const;
				bool			operator!=		(const CCircle & a) const;

				void			Set				(const f32 x, const f32 y, const f32 radius);
				void			Zero			();
				
				CCircle							(f32 x, f32 y, f32 radius);
				CCircle							(const CCircle& r);

				f32				GetRadius		() const;
				CVector2		GetPosition		() const;
				CVector2		GetPoint		(f32 angle) const;
				CRect			GetBounds		() const;

				bool			Contains		(const CCircle& other) const;
				bool			Overlaps		(const CCircle& other) const;
				bool			Contains		(const CRect& other) const;
				bool			Overlaps		(const CRect& other) const;
				bool			Contains		(const CLine& other) const;
				bool			Overlaps		(const CLine& other) const;
				bool			Contains		(const CVector2& other) const;

				CCircle			Inflate			(f32 amount) const;
				CCircle			Deflate			(f32 amount) const;
				CCircle			Scale			(f32 amount) const;
				CCircle			Offset			(f32 amount) const;
				CCircle			Offset			(CVector2 size) const;
				CCircle			Union			(const CCircle& other) const;

				CCircle			Normalize		() const;

				Engine::Containers::CString	ToString	();
				static bool					FromString	(Engine::Containers::CString str, CCircle& a);
	
		};

	}
}