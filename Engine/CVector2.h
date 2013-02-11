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

		class CVector2
		{
			public:
				f32 X, Y;

				CVector2						();
				CVector2						(const f32 x, const f32 y);

				f32				operator[]		(u32 index) const;
				f32 &			operator[]		(u32 index);

				CVector2		operator-		() const;
				
				CVector2		operator/		(const f32 a) const;
				CVector2		operator+		(const f32 a) const;
				CVector2		operator-		(const f32 a) const;
				CVector2		operator/		(const CVector2 & a) const;
				CVector2		operator+		(const CVector2 & a) const;
				CVector2		operator-		(const CVector2 & a) const;

				CVector2		operator*		(const f32 a) const;				
				float			operator*		(const CVector2 & a) const;

				CVector2 &		operator*=		(const f32 a);
				CVector2 &		operator/=		(const f32 a);
				CVector2 &		operator+=		(const f32 a);
				CVector2 &		operator-=		(const f32 a);
				CVector2 &		operator*=		(const CVector2 & a);
				CVector2 &		operator/=		(const CVector2 & a);
				CVector2 &		operator+=		(const CVector2 & a);
				CVector2 &		operator-=		(const CVector2 & a);

				friend CVector2 operator*		(const f32 a, const CVector2 b);

				bool			Compare			(const CVector2 & a) const;
				bool			Compare			(const CVector2 & a, const f32 epsilon) const;
				bool			operator==		(const CVector2 & a) const;
				bool			operator!=		(const CVector2 & a) const;

				bool			operator>		(const CVector2 & a) const;
				bool			operator<		(const CVector2 & a) const;
				bool			operator>=		(const CVector2 & a) const;
				bool			operator<=		(const CVector2 & a) const;

				bool			IsUnitVector	() const;
				bool			IsPerpendicular	(const CVector2 & v2) const;

				void			Set				(const f32 x, const f32 y);
				void			Zero			();
				f32				Length			() const;
				f32				LengthSquared	() const;
				f32				ToAngle			() const;
				f32				Sum				() const;

				CVector2	Rotate			(f32 rads) const;
				CVector2 	Normalize		() const;
				CVector2 	Truncate		(const f32 length) const;
				CVector2 	Clamp			(const CVector2& min, const CVector2& max) const;
				CVector2 	Ceil			() const; 
				CVector2 	Floor			() const;
				f32			Cross			(const CVector2& b) const;
				f32			Dot				(const CVector2& b) const;
				f32			Distance		(const CVector2& b) const;
				CVector2	Lerp			(const CVector2& b, const f32 delta) const;
				CVector2	SLerp			(const CVector2& b, const f32 delta) const;
				CVector2	SmoothStep		(const CVector2& b, const f32 delta) const;
				CVector2	Reflect			(const CVector2& normal) const;
				CVector2 	Min				(const CVector2& b) const;
				CVector2 	Max				(const CVector2& b) const;

				Engine::Containers::CString	ToString	();
				static bool					FromString	(Engine::Containers::CString str, CVector2& a);
		};


	}
}