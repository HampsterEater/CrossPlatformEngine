///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Math.h"

#include "CVector3.h"

namespace Engine
{
	namespace Containers
	{
		class CString;
	}
    namespace Math
    {
		class CMatrix4;
		class CQuaternion;

		class CMatrix3
		{
			private:
				CVector3 _mat[3];

			public:
				CMatrix3			();
				CMatrix3			(const CVector3& x, const CVector3& y, const CVector3& z);
				CMatrix3			(const f32 xx, const f32 xy, const f32 xz, const f32 yx, const f32 yy, const f32 yz, const f32 zx, const f32 zy, const f32 zz);
				CMatrix3			(const f32 src[3][3]);

				const CVector3&		operator[]	(u32 index) const;
				const CVector3&		operator[]	(s32 index) const;
				CVector3&			operator[]	(u32 index);
				CVector3&			operator[]	(s32 index);

				CMatrix3			operator-	() const;
				CMatrix3			operator*	(const f32 a) const;
				CMatrix3			operator/	(const f32 a) const;
				CVector3			operator*	(const CVector3& a) const;
				CMatrix3			operator*	(const CMatrix3& a) const;
				CMatrix3			operator+	(const CMatrix3& a) const;
				CMatrix3			operator-	(const CMatrix3& a) const;

				CMatrix3&			operator/=	(const f32 a);
				CMatrix3&			operator*=	(const f32 a);
				CMatrix3&			operator*=	(const CMatrix3& a);
				CMatrix3&			operator+=	(const CMatrix3& a);
				CMatrix3&			operator-=	(const CMatrix3& a);

				friend CMatrix3		operator*	(const f32 a, const CMatrix3& mat);
				friend CVector3		operator*	(const CVector3& a, const CMatrix3& mat);
				friend CVector3&	operator*=	(CVector3& a, const CMatrix3& mat);

				bool				Compare		(const CMatrix3& a) const;
				bool				Compare		(const CMatrix3& a, const f32 epsilon) const;
				bool				operator==	(const CMatrix3& a) const;
				bool				operator!=	(const CMatrix3& a) const;

				void				Zero		();
				void				Identify	();
				bool				IsIdentify	(const f32 epsilon = Engine::Math::EPSILON) const;
				bool				IsSymmetric	(const f32 epsilon = Engine::Math::EPSILON) const;
				bool				IsDiagonal	(const f32 epsilon = Engine::Math::EPSILON) const;

				CMatrix3			OrthoNormalize	() const;
				
				CVector3			ProjectVector	(const CVector3& src) const;
				CVector3			UnprojectVector	(const CVector3& src) const;
				
				CMatrix3			Normalize	() const;
				
				f32					Trace		() const;
				f32					Determinant	() const;
				CMatrix3			Inverse		() const;
				CMatrix3			Transpose	() const;

				Engine::Containers::CString	ToString	();
				static bool					FromString	(Engine::Containers::CString str, CMatrix3& a);
		
				static CMatrix3		GetIdentity	();
				static CMatrix3		GetZero		();

//				GetXRotationMatrix();
//				GetYRotationMatrix();
//				GetZRotationMatrix();

//				CAngle				ToAngle		() const;
				CQuaternion			ToQuaternion() const;
				CMatrix4			ToMatrix4	() const;
		
		};

	}
}