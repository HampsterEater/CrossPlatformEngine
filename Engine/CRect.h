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
		class CLine;
		class CCircle;

		class CRect
		{
			public:
				f32 X, Y;
				f32 Width, Height;
				
				f32				operator[]		(u32 index) const;
				f32 &			operator[]		(u32 index);
				
				bool			Compare			(const CRect & a) const;
				bool			Compare			(const CRect & a, const f32 epsilon) const;
				bool			operator==		(const CRect & a) const;
				bool			operator!=		(const CRect & a) const;

				void			Set				(const f32 x, const f32 y, const f32 w, const f32 h);
				void			Zero			();
				
				CRect							(f32 x, f32 y, f32 w, f32 h);
				CRect							(const CRect& r);

				CVector2		GetSize			() const;
				CVector2		GetPosition		() const;

				CVector2 		GetTop			() const;
				CVector2		GetBottom		() const;
				CVector2		GetLeft			() const;
				CVector2		GetRight		() const;
				CVector2		GetTopLeft		() const;
				CVector2		GetTopRight		() const;
				CVector2		GetBottomLeft	() const;
				CVector2		GetBottomRight	() const;
				CVector2		GetCenter		() const;
				
				bool			Contains		(const CRect& other) const;
				bool			Overlaps		(const CRect& other) const;
				bool			Contains		(const CLine& other) const;
				bool			Overlaps		(const CLine& other) const;
				bool			Contains		(const CCircle& other) const;
				bool			Overlaps		(const CCircle& other) const;
				bool			Contains		(const CVector2& other) const;
				bool			IntersectPoint	(const CLine& other, CVector2& a) const;

				CRect			Inflate			(f32 amount) const;
				CRect			Inflate			(CVector2 size) const;
				CRect			Deflate			(f32 amount) const;
				CRect			Deflate			(CVector2 size) const;
				CRect			Scale			(f32 amount) const;
				CRect			Scale			(CVector2 size) const;
				CRect			Offset			(f32 amount) const;
				CRect			Offset			(CVector2 size) const;
				CRect			Union			(const CRect& other) const;
				CRect			Clip			(const CRect& other) const;

				CRect			Normalize		() const;

				Engine::Containers::CString	ToString	();
				static bool					FromString	(Engine::Containers::CString str, CRect& a);
	
		};

	}
}