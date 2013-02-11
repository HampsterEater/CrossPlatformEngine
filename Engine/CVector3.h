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

		class CVector3
		{
			public:				
				f32 X, Y, Z;

				CVector3						();
				CVector3						(const f32 x, const f32 y, const f32 z);

				f32				operator[]		(u32 index) const;
				f32 &			operator[]		(u32 index);

				CVector3		operator-		() const;
				
				CVector3		operator/		(const f32 a) const;
				CVector3		operator+		(const f32 a) const;
				CVector3		operator-		(const f32 a) const;
				CVector3		operator/		(const CVector3 & a) const;
				CVector3		operator+		(const CVector3 & a) const;
				CVector3		operator-		(const CVector3 & a) const;

				CVector3		operator*		(const f32 a) const;				
				float			operator*		(const CVector3 & a) const;

				CVector3 &		operator*=		(const f32 a);
				CVector3 &		operator/=		(const f32 a);
				CVector3 &		operator+=		(const f32 a);
				CVector3 &		operator-=		(const f32 a);
				CVector3 &		operator*=		(const CVector3 & a);
				CVector3 &		operator/=		(const CVector3 & a);
				CVector3 &		operator+=		(const CVector3 & a);
				CVector3 &		operator-=		(const CVector3 & a);

				friend CVector3 operator*		(const f32 a, const CVector3 b);

				bool			Compare			(const CVector3 & a) const;
				bool			Compare			(const CVector3 & a, const f32 epsilon) const;
				bool			operator==		(const CVector3 & a) const;
				bool			operator!=		(const CVector3 & a) const;

				bool			operator>		(const CVector3 & a) const;
				bool			operator<		(const CVector3 & a) const;
				bool			operator>=		(const CVector3 & a) const;
				bool			operator<=		(const CVector3 & a) const;

				bool			IsUnitVector	() const;
				bool			IsPerpendicular	(const CVector3 & v2)  const;
				static bool 	IsBackFace		(const CVector3& normal, const CVector3& lineOfSight);

				void			Set				(const f32 x, const f32 y, const f32 z);
				void			Zero			();
				f32				Length			() const;
				f32				LengthSquared	() const;
				f32				ToPitch			() const;
				f32				ToYaw			() const;
				f32				Sum				() const;
//				CAngle			ToAngle			() const;

				CVector3	RotatePitch		(f32 rads) const;
				CVector3	RotateYaw		(f32 rads) const;
				CVector3	RotateRoll		(f32 rads) const;
				CVector3 	Normalize		() const;
				CVector3 	Truncate		(const f32 length) const;
				CVector3 	Clamp			(const CVector3& min, const CVector3& max) const;
				CVector3 	Ceil			() const; 
				CVector3 	Floor			() const;
				CVector3	Cross			(const CVector3& b) const;
				f32			Dot				(const CVector3& b) const;
				f32			Distance		(const CVector3& b) const;
				CVector3	Lerp			(const CVector3& b, const f32 delta) const;
				CVector3	SLerp			(const CVector3& b, const f32 delta) const;
				CVector3	SmoothStep		(const CVector3& b, const f32 delta) const;
				CVector3	Reflect			(const CVector3& normal) const;
				CVector3 	Min				(const CVector3& b) const;
				CVector3 	Max				(const CVector3& b) const;

				void	 	NormalVectors	(CVector3& left, CVector3& down) const;
				void		OrthogonalBasis	(CVector3& left, CVector3& up) const;

				CVector3 	ProjectOntoPlane		(const CVector3& normal, const f32 overBounce = 1.0f) const;
				CVector3 	ProjectAlongPlane		(const CVector3& normal, const f32 epsilon, const f32 overBounce = 1.0f) const;
				CVector3 	ProjectOntoSphere		(const f32 radius) const;

				Engine::Containers::CString	ToString	();
				static bool				FromString	(Engine::Containers::CString str, CVector3& a);
		};

	}
}