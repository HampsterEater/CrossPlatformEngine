///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Math.h"

#include "CVector4.h"

namespace Engine
{
	namespace Containers
	{
		class CString;
	}
    namespace Math
    {
		class CVector3;

		class CMatrix4
		{
			private:
				CVector4 _mat[4];

			public:
				CMatrix4			();
				CMatrix4			(const CVector4& x, const CVector4& y, const CVector4& z, const CVector4& w);
				CMatrix4			(const f32 xx, const f32 xy, const f32 xz, const f32 xw,
									 const f32 yx, const f32 yy, const f32 yz, const f32 yw, 
									 const f32 zx, const f32 zy, const f32 zz, const f32 zw,
									 const f32 wx, const f32 wy, const f32 wz, const f32 ww);
				CMatrix4			(const f32 src[4][4]);

				const CVector4&		operator[]	(u32 index) const;
				const CVector4&		operator[]	(s32 index) const;
				CVector4&			operator[]	(u32 index);
				CVector4&			operator[]	(s32 index);

				CMatrix4			operator-	() const;
				CMatrix4			operator*	(const f32 a) const;
				CMatrix4			operator/	(const f32 a) const;
				CVector4			operator*	(const CVector4& a) const;
				CVector3			operator*	(const CVector3& a) const;
				CMatrix4			operator*	(const CMatrix4& a) const;
				CMatrix4			operator+	(const CMatrix4& a) const;
				CMatrix4			operator-	(const CMatrix4& a) const;

				CMatrix4&			operator/=	(const f32 a);
				CMatrix4&			operator*=	(const f32 a);
				CMatrix4&			operator*=	(const CMatrix4& a);
				CMatrix4&			operator+=	(const CMatrix4& a);
				CMatrix4&			operator-=	(const CMatrix4& a);

				friend CMatrix4		operator*	(const f32 a, const CMatrix4& mat);
				friend CVector4		operator*	(const CVector4& a, const CMatrix4& mat);
				friend CVector4&	operator*=	(CVector4& a, const CMatrix4& mat);
				friend CVector3&	operator*=	(CVector3& a, const CMatrix4& mat);

				bool				Compare		(const CMatrix4& a) const;
				bool				Compare		(const CMatrix4& a, const f32 epsilon) const;
				bool				operator==	(const CMatrix4& a) const;
				bool				operator!=	(const CMatrix4& a) const;

				void				Zero		();
				void				Identify	();
				bool				IsIdentify	(const f32 epsilon = Engine::Math::EPSILON) const;
				bool				IsSymmetric	(const f32 epsilon = Engine::Math::EPSILON) const;
				bool				IsDiagonal	(const f32 epsilon = Engine::Math::EPSILON) const;

				CMatrix4			Normalize	() const;
				
				CVector4			ProjectVector	(const CVector4& src) const;
				CVector4			UnprojectVector	(const CVector4& src) const;

				f32					Trace		() const;
				f32					Determinant	() const;
				CMatrix4			Inverse		() const;
				CMatrix4			Transpose	() const;

				Engine::Containers::CString	ToString	();
				static bool					FromString	(Engine::Containers::CString str, CMatrix4& a);
		
				static CMatrix4		GetIdentity	();
				static CMatrix4		GetZero		();

//				CAngle				ToAngle		() const;
//				CQuaternion			ToQuaternion() const;

		};

	}
}