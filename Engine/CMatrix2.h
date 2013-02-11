///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Math.h"

#include "CVector2.h"

namespace Engine
{
	namespace Containers
	{
		class CString;
	}
    namespace Math
    {
		class CMatrix2
		{
			private:
				CVector2 _mat[2];

			public:
				CMatrix2			();
				CMatrix2			(const CVector2& x, const CVector2& y);
				CMatrix2			(const f32 xx, const f32 xy, const f32 yx, const f32 yy);
				CMatrix2			(const f32 src[2][2]);

				const CVector2&		operator[]	(u32 index) const;
				const CVector2&		operator[]	(s32 index) const;
				CVector2&			operator[]	(u32 index);
				CVector2&			operator[]	(s32 index);

				CMatrix2			operator-	() const;
				CMatrix2			operator*	(const f32 a) const;
				CMatrix2			operator/	(const f32 a) const;
				CVector2			operator*	(const CVector2& a) const;
				CMatrix2			operator*	(const CMatrix2& a) const;
				CMatrix2			operator+	(const CMatrix2& a) const;
				CMatrix2			operator-	(const CMatrix2& a) const;

				CMatrix2&			operator/=	(const f32 a);
				CMatrix2&			operator*=	(const f32 a);
				CMatrix2&			operator*=	(const CMatrix2& a);
				CMatrix2&			operator+=	(const CMatrix2& a);
				CMatrix2&			operator-=	(const CMatrix2& a);

				friend CMatrix2		operator*	(const f32 a, const CMatrix2& mat);
				friend CVector2		operator*	(const CVector2& a, const CMatrix2& mat);
				friend CVector2&	operator*=	(CVector2& a, const CMatrix2& mat);

				bool				Compare		(const CMatrix2& a) const;
				bool				Compare		(const CMatrix2& a, const f32 epsilon) const;
				bool				operator==	(const CMatrix2& a) const;
				bool				operator!=	(const CMatrix2& a) const;

				void				Zero		();
				void				Identify	();
				bool				IsIdentify	(const f32 epsilon = Engine::Math::EPSILON) const;
				bool				IsSymmetric	(const f32 epsilon = Engine::Math::EPSILON) const;
				bool				IsDiagonal	(const f32 epsilon = Engine::Math::EPSILON) const;

				f32					Trace		() const;
				f32					Determinant	() const;
				CMatrix2			Inverse		() const;
				CMatrix2			Transpose	() const;
				
				CMatrix2			Normalize	() const;
				
				Engine::Containers::CString	ToString	();
				static bool					FromString	(Engine::Containers::CString str, CMatrix2& a);
		
				static CMatrix2		GetIdentity	();
				static CMatrix2		GetZero		();

//				CAngle				ToAngle		() const;
//				CQuaternion			ToQuaternion() const;

		};

	}
}