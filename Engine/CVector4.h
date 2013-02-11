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
		class CVector4
		{
			public:
				f32 X, Y, Z, W;

				CVector4						();
				CVector4						(const f32 x, const f32 y, const f32 z, const f32 w);

				f32				operator[]		(u32 index) const;
				f32 &			operator[]		(u32 index);

				CVector4		operator-		() const;
				
				CVector4		operator/		(const f32 a) const;
				CVector4		operator+		(const f32 a) const;
				CVector4		operator-		(const f32 a) const;
				
				CVector4		operator*		(const f32 a) const;				
				float			operator*		(const CVector4 & a) const;

				CVector4		operator/		(const CVector4 & a) const;
				CVector4		operator+		(const CVector4 & a) const;
				CVector4		operator-		(const CVector4 & a) const;

				CVector4 &		operator*=		(const f32 a);
				CVector4 &		operator/=		(const f32 a);
				CVector4 &		operator+=		(const f32 a);
				CVector4 &		operator-=		(const f32 a);
				CVector4 &		operator*=		(const CVector4 & a);
				CVector4 &		operator/=		(const CVector4 & a);
				CVector4 &		operator+=		(const CVector4 & a);
				CVector4 &		operator-=		(const CVector4 & a);

				friend CVector4 operator*		(const f32 a, const CVector4 b);

				bool			Compare			(const CVector4 & a) const;
				bool			Compare			(const CVector4 & a, const f32 epsilon) const;
				bool			operator==		(const CVector4 & a) const;
				bool			operator!=		(const CVector4 & a) const;

				bool			operator>		(const CVector4 & a) const;
				bool			operator<		(const CVector4 & a) const;
				bool			operator>=		(const CVector4 & a) const;
				bool			operator<=		(const CVector4 & a) const;

				bool			IsUnitVector	() const;
				bool			IsPerpendicular	(const CVector4 & v2) const;

				void			Set				(const f32 x, const f32 y, const f32 z, const f32 w);
				void			Zero			();
				f32				Length			() const;
				f32				LengthSquared	() const;
				f32				Sum				() const;

				CVector4 	Normalize		() const;
				CVector4 	Truncate		(const f32 length) const;
				CVector4 	Clamp			(const CVector4& min, const CVector4& max) const;
				CVector4 	Ceil			() const; 
				CVector4 	Floor			() const;
				f32			Dot				(const CVector4& b) const;
				f32			Distance		(const CVector4& b) const;
				CVector4	Lerp			(const CVector4& b, const f32 delta) const;
				CVector4	SLerp			(const CVector4& b, const f32 delta) const;
				CVector4	SmoothStep		(const CVector4& b, const f32 delta) const;
				CVector4	Reflect			(const CVector4& normal) const;
				CVector4 	Min				(const CVector4& b) const;
				CVector4 	Max				(const CVector4& b) const;

				Engine::Containers::CString	ToString	();
				static bool				FromString	(Engine::Containers::CString str, CVector4& a);
		};

	}
}