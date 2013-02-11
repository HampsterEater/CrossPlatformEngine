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
		class CVector3;
		class CVector4;
		class CMatrix3;
		class CMatrix4;

		class CQuaternion
		{
			public:
				f32 X;
				f32 Y;
				f32 Z;
				f32 W;

									CQuaternion		();
									CQuaternion		(f32 x, f32 y, f32 z, f32 w);

				void				Set				(const f32 x, const f32 y, const f32 z, const f32 w);
				void				Zero			();
					
				f32					operator[]		(u32 index) const;
				f32 &				operator[]		(u32 index);
				f32					operator[]		(s32 index) const;
				f32 &				operator[]		(s32 index);
				CQuaternion			operator-		() const;
				CQuaternion &		operator=		(const CQuaternion& a);

				CQuaternion 		operator+		(const CQuaternion& a) const;
				CQuaternion 		operator-		(const CQuaternion& a) const;
				CQuaternion 		operator*		(const CQuaternion& a) const;
				CVector3	 		operator*		(const CVector3& a) const;
				CQuaternion	 		operator*		(f32 a) const;
				CQuaternion	 		operator/		(f32 a) const;

				CQuaternion& 		operator*=		(f32 a);
				CQuaternion& 		operator*=		(const CQuaternion& a);
				CQuaternion& 		operator-=		(const CQuaternion& a);
				CQuaternion& 		operator+=		(const CQuaternion& a);
				
				friend CQuaternion	operator*		(const f32 a, const CQuaternion& b);
				friend CVector3		operator*		(const CVector3 a, const CQuaternion& b);

				bool				Compare			(const CQuaternion & a) const;
				bool				Compare			(const CQuaternion & a, const f32 epsilon) const;
				bool				operator==		(const CQuaternion & a) const;
				bool				operator!=		(const CQuaternion & a) const;

				CQuaternion			Inverse			() const;
				f32					Length			() const;
				CQuaternion			Normalize		() const;

				CQuaternion			Lerp			(const CQuaternion& b, f32 delta);
				CQuaternion			Slerp			(const CQuaternion& b, f32 delta);

				Engine::Containers::CString	ToString	();
				static bool					FromString	(Engine::Containers::CString str, CQuaternion& a);

				//Angles				ToAngle			() const;
				CMatrix3				ToMatrix3		() const;
				CMatrix4				ToMatrix4		() const;
		};

	}
}