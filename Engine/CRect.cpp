///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CRect.h"
#include "CLine.h"
#include "CCircle.h"
#include "CVector2.h"
#include "CString.h"
#include "Math.h"

using namespace Engine::Math;

f32	CRect::operator[](u32 index) const
{
	if (index == 0)
		return X;
	else if (index == 1)
		return Y;
	else if (index == 2)
		return Width;
	else
		return Height;
}

f32 & CRect::operator[](u32 index)
{
	if (index == 0)
		return X;
	else if (index == 1)
		return Y;
	else if (index == 2)
		return Width;
	else
		return Height;
}
				
bool CRect::Compare(const CRect & a) const
{
	return (X == a.X && Y == a.Y && Width == a.Width && Height == a.Height);
}

bool CRect::Compare(const CRect & a, const f32 epsilon) const
{
	return (Abs(X - a.X) > epsilon && Abs(Y - a.Y) > epsilon && Abs(Width - a.Width) > epsilon && Abs(Height - a.Height) > epsilon);
}

bool CRect::operator==(const CRect & a) const
{
	return Compare(a);
}

bool CRect::operator!=(const CRect & a) const
{
	return !Compare(a);
}

void CRect::Set(const f32 x, const f32 y, const f32 w, const f32 h)
{
	X = x;
	Y = y;
	Width = w;
	Height = h;
}

void CRect::Zero()
{
	X = 0;
	Y = 0;
	Width = 0;
	Height = 0;
}
				
CRect::CRect(f32 x, f32 y, f32 w, f32 h)
{
	X = x;
	Y = y;
	Width = w;
	Height = h;
}

CRect::CRect(const CRect& r)
{
	X = r.X;
	Y = r.Y;
	Width = r.Width;
	Height = r.Height;
}

CVector2 CRect::GetSize() const
{
	return CVector2(Width, Height);
}

CVector2 CRect::GetPosition() const
{
	return CVector2(X, Y);
}

CVector2 CRect::GetTop() const
{
	return CVector2(X + (Width / 2), Y);
}

CVector2 CRect::GetBottom() const
{
	return CVector2(X + (Width / 2), Y + Height);
}

CVector2 CRect::GetLeft() const
{
	return CVector2(X, Y + (Height / 2));
}

CVector2 CRect::GetRight() const
{
	return CVector2(X + Width, Y + (Height / 2));
}

CVector2 CRect::GetTopLeft() const
{
	return CVector2(X, Y);
}

CVector2 CRect::GetTopRight() const
{
	return CVector2(X + Width, Y);
}

CVector2 CRect::GetBottomLeft() const
{
	return CVector2(X, Y + Height);
}

CVector2 CRect::GetBottomRight() const
{
	return CVector2(X + Width, Y + Height);
}

CVector2 CRect::GetCenter() const
{
	return CVector2(X + (Width / 2), Y + (Height / 2));
}
				
bool CRect::Contains(const CRect& other) const
{
	return (other.X >= X && other.Y >= Y &&
			other.X + other.Width <= X + Width && other.Y + other.Height <= Y + Height);
}

bool CRect::Overlaps(const CRect& other) const
{
	if (X > (other.X + other.Width) || (X + Width) < other.X) 
		return false;
	if (Y > (other.Y + other.Height) || (Y + Height) < other.Y)
		return false;
	return true;
}

bool CRect::Contains(const CLine& other) const
{
	return this->Contains(other.Start) && this->Contains(other.End);
}

bool CRect::Overlaps(const CLine& other) const
{
	CVector2 point;
	return IntersectPoint(other, point);
}

bool CRect::IntersectPoint(const CLine& other, CVector2& a) const
{
	f32 last_x = other.Start.X;
	f32 last_y = other.Start.Y;
	f32 x = other.End.X;
	f32 y = other.End.Y;
	f32 bx1 = X;
	f32 by1 = Y;
	f32 bx2 = Width;
	f32 by2 = Height;
	f32 ix = 0;
	f32 iy = 0;

	if (last_x < bx1 && x >= bx1) 			//does it cross left edge?
	{
		iy = last_y + (y - last_y) * (bx1 - last_x) / (x - last_x);
		if (iy >= by1 && iy <= by2)			//is intersection point on left edge?
		{
			ix = bx1;
			a = CVector2(ix, iy);
			return true;
		}
	}
	else if (last_x > bx2 && x <= bx2)		//does it cross right edge?
	{
		iy = last_y + (y - last_y) * (bx2 - last_x)/(x - last_x);
		if (iy >= by1 && iy <= by2)			//is intersection point on right edge?
		{
			ix = bx2;
			a = CVector2(ix, iy);
			return true;
		}
	}
	
	if (last_y < by1 && y >= by1) 			//does it cross top edge?
	{
		ix = last_x + (x - last_x) * (by1 - last_y)/(y - last_y);
		if (ix >= bx1 && ix <= bx2)			//is intersection point on top edge?
		{	
			iy = by1;
			a = CVector2(ix, iy);
			return true;
		}
	}
	else if (last_y > by2 && y <= by2)		//does it cross bottom edge?
	{
		ix = last_x + (x - last_x) * (by2 - last_y) / (y - last_y);
		if (ix >= bx1 && ix <= bx2)			//is intersection point on bottom edge?
		{
			iy = by2;
			a = CVector2(ix, iy);
			return true;
		}
	}

	return false;
}

bool CRect::Contains(const CCircle& other) const
{
	return this->Contains(other.GetBounds());
}

bool CRect::Overlaps(const CCircle& other) const
{
	CVector2 center = other.GetPosition();
	f32 radius = other.GetRadius();
	
	if (GetTopLeft().Distance(center) < radius)
		return true;
	if (GetTopRight().Distance(center) < radius)
		return true;
	if (GetBottomLeft().Distance(center) < radius)
		return true;
	if (GetBottomRight().Distance(center) < radius)
		return true;

	return false;
}

bool CRect::Contains(const CVector2& other) const
{
	return (other.X >= X && other.Y >= Y && other.X < X + Width && other.Y < Y + Height);
}

CRect CRect::Inflate(f32 amount) const
{
	CRect inflated = *this;
	inflated.X -= amount;
	inflated.Y -= amount;
	inflated.Width += amount * 2;
	inflated.Height += amount * 2;

	return inflated.Normalize();
}

CRect CRect::Inflate(CVector2 amount) const
{
	CRect inflated = *this;
	inflated.X -= amount.X;
	inflated.Y -= amount.Y;
	inflated.Width += amount.X * 2;
	inflated.Height += amount.Y * 2;

	return inflated.Normalize();
}

CRect CRect::Deflate(f32 amount) const
{
	CRect inflated = *this;
	inflated.X += amount;
	inflated.Y += amount;
	inflated.Width -= amount * 2;
	inflated.Height -= amount * 2;

	return inflated.Normalize();
}

CRect CRect::Deflate(CVector2 amount) const
{
	CRect inflated = *this;
	inflated.X += amount.X;
	inflated.Y += amount.Y;
	inflated.Width -= amount.X * 2;
	inflated.Height -= amount.Y * 2;

	return inflated.Normalize();
}

CRect CRect::Scale(f32 amount) const
{
	CRect inflated = *this;
	inflated.Width *= amount;
	inflated.Height *= amount;

	return inflated.Normalize();
}

CRect CRect::Scale(CVector2 amount) const
{
	CRect inflated = *this;
	inflated.Width *= amount.X;
	inflated.Height *= amount.Y;

	return inflated.Normalize();
}

CRect CRect::Offset(f32 amount) const
{
	CRect inflated = *this;
	inflated.X += amount;
	inflated.Y += amount;

	return inflated.Normalize();
}

CRect CRect::Offset(CVector2 amount) const
{
	CRect inflated = *this;
	inflated.X += amount.X;
	inflated.Y += amount.Y;

	return inflated.Normalize();
}

CRect CRect::Clip(const CRect& other) const
{
	CRect clipped = *this;

	// Left
	if (clipped.X < other.X)
	{
		f32 amount = other.X - clipped.X;
		clipped.X += amount;
		clipped.Width -= amount;
	}

	// Top
	if (clipped.Y < other.Y)
	{
		f32 amount = other.Y - clipped.Y;
		clipped.Y += amount;
		clipped.Height -= amount;
	}
	
	// Width
	if (clipped.X + clipped.Width > other.X + other.Width)
	{
		f32 amount = (clipped.X + clipped.Width) - (other.X + other.Width);
		clipped.Width -= amount;
	}

	// Height
	if (clipped.Y + clipped.Height > other.Y + other.Height)
	{
		f32 amount = (clipped.Y + clipped.Height) - (other.Y + other.Height);
		clipped.Height -= amount;
	}

	return clipped.Normalize();
}

CRect CRect::Union(const CRect& other) const
{
	CRect clipped = *this;

	if (other.X < clipped.X)
		clipped.X = other.X;
	
	if (other.Y < clipped.Y)
		clipped.Y = other.Y;

	if (other.X + other.Width > clipped.X + clipped.Width)
	{
		f32 amount = (other.X + other.Width) - (clipped.X + clipped.Width);
		clipped.Width += amount;
	}

	if (other.Y + other.Height > clipped.Y + clipped.Height)
	{
		f32 amount = (other.Y + other.Height) - (clipped.Y + clipped.Height);
		clipped.Height += amount;
	}

	return clipped.Normalize();
}

CRect CRect::Normalize() const
{
	CRect a = *this;
	
	if (a.Width < 0) 
		a.Width = 0;

	if (a.Height < 0) 
		a.Height = 0;

	return a;
}

Engine::Containers::CString	CRect::ToString()
{
	return S("(") + X + "," + Y + "," + Width + "," + Height + ")";
}

bool CRect::FromString(Engine::Containers::CString str, CRect& a)
{
	if (str.Length() <= 0 ||								// Needs a string :3
		str[0] != '(' || str[str.Length() - 1] != ')' ||	// Needs the braces around it.
		str.Count(',') != 3)								// Needs exactly 1 seperating comma.
		return false;
	
	str = str.Trim("() ");

	s32 idx = str.IndexOf(',');
	s32 idx2 = str.IndexOf(',', idx + 1);
	s32 idx3 = str.IndexOf(',', idx2 + 1);

	Engine::Containers::CString x = str.SubString(0, idx).Trim();
	Engine::Containers::CString y = str.SubString(idx + 1, (idx2 - idx) - 1).Trim();
	Engine::Containers::CString w = str.SubString(idx2 + 1, (idx3 - idx2) - 1).Trim();
	Engine::Containers::CString h = str.SubString(idx3 + 1).Trim();

	a.Set(x.ToFloat(), y.ToFloat(), w.ToFloat(), h.ToFloat());

	return true;
}
	
