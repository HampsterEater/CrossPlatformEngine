///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CCircle.h"
#include "CLine.h"
#include "CRect.h"
#include "CVector2.h"
#include "CString.h"
#include "Math.h"

using namespace Engine::Math;

f32	CCircle::operator[](u32 index) const
{
	if (index == 0)
		return X;
	else if (index == 1)
		return Y;
	else 
		return Radius;
}

f32 & CCircle::operator[](u32 index)
{
	if (index == 0)
		return X;
	else if (index == 1)
		return Y;
	else 
		return Radius;
}
				
bool CCircle::Compare(const CCircle & a) const
{
	return (a.X == X && a.Y == Y && a.Radius == Radius);
}

bool CCircle::Compare(const CCircle & a, const f32 epsilon) const
{
	return Abs(a.X - X) > epsilon && Abs(a.Y - Y) > epsilon && Abs(a.Radius - Radius) > epsilon;
}

bool CCircle::operator==(const CCircle & a) const
{
	return Compare(a);
}

bool CCircle::operator!=(const CCircle & a) const
{
	return !Compare(a);
}

void CCircle::Set(const f32 x, const f32 y, const f32 radius)
{
	X = x;
	Y = y;
	Radius = radius;
}

void CCircle::Zero()
{
	X = 0;
	Y = 0;
	Radius = 0;
}
				
CCircle::CCircle(f32 x, f32 y, f32 radius)
{
	X = x;
	Y = y;
	Radius = radius;
}

CCircle::CCircle(const CCircle& r)
{
	X = r.X;
	Y = r.Y;
	Radius = r.Radius;
}

f32	CCircle::GetRadius() const
{
	return Radius;
}

CVector2 CCircle::GetPosition() const
{
	return CVector2(X, Y);
}

CVector2 CCircle::GetPoint(f32 angle) const
{
	CVector2 pos;
	pos.X = X + (Cos(angle) * Radius);
	pos.Y = Y + (Sin(angle) * Radius);
	return pos;
}

CRect CCircle::GetBounds() const
{
	return CRect(X - Radius, Y - Radius, Radius * 2, Radius * 2);
}

bool CCircle::Contains(const CCircle& other) const
{
	f32 x1 = X,		  y1 = Y,		r1 = Radius;
	f32 x2 = other.X, y2 = other.Y, r2 = other.Radius;

	// R1 must be the smallest.
	if (r1 > r2)
	{
		f32 tx1 = x1, ty1 = y1, tr1 = r1;
		x1 = x2;
		y1 = y2;
		r1 = r2;
		x2 = tx1;
		y2 = ty1;
		r2 = tr1;
	}

	// Compute distance between circle centers.
	f32 dx = (x1 - x2);
	f32 dy = (y1 - y2);
	f32 d = Sqrt(dx*dx + dy*dy);

	// Are we fully within the other circle?
	return (d + r1 <= r2);	
}

bool CCircle::Overlaps(const CCircle& other) const
{
	f32 x1 = X,		  y1 = Y,		r1 = Radius;
	f32 x2 = other.X, y2 = other.Y, r2 = other.Radius;

	// R1 must be the smallest.
	if (r1 > r2)
	{
		f32 tx1 = x1, ty1 = y1, tr1 = r1;
		x1 = x2;
		y1 = y2;
		r1 = r2;
		x2 = tx1;
		y2 = ty1;
		r2 = tr1;
	}

	// Compute distance between circle centers.
	f32 dx = (x1 - x2);
	f32 dy = (y1 - y2);
	f32 d = Sqrt(dx*dx + dy*dy);// * 0.5f;

	// Touching the other circle?
	return (d < (r1 + r2));	
}

bool CCircle::Contains(const CRect& other) const
{
	f32 tl = other.GetTopLeft().Distance(CVector2(X, Y));
	f32 tr = other.GetTopRight().Distance(CVector2(X, Y));
	f32 bl = other.GetBottomLeft().Distance(CVector2(X, Y));
	f32 br = other.GetBottomRight().Distance(CVector2(X, Y));
	return tl <= Radius && tr <= Radius && bl <= Radius && br <= Radius;
}

bool CCircle::Overlaps(const CRect& other) const
{
	return other.Overlaps(*this);
}

bool CCircle::Contains(const CLine& other) const
{
	f32 start_d = other.Start.Distance(CVector2(X, Y));
	f32 end_d   = other.End.Distance(CVector2(X, Y));
	return (start_d <= Radius && end_d <= Radius);
}

bool CCircle::Overlaps(const CLine& other) const
{
	return other.Overlaps(*this);
}

bool CCircle::Contains(const CVector2& other) const
{
	f32 distance = other.Distance(CVector2(X, Y));
	return distance <= Radius;
}

CCircle CCircle::Inflate(f32 amount) const
{
	CCircle a = *this;
	a.Radius += amount;
	return a.Normalize();
}

CCircle CCircle::Deflate(f32 amount) const
{
	CCircle a = *this;
	a.Radius -= amount;
	return a.Normalize();
}

CCircle CCircle::Scale(f32 amount) const
{
	CCircle a = *this;
	a.Radius *= amount;
	return a.Normalize();
}

CCircle CCircle::Offset(f32 amount) const
{
	CCircle a = *this;
	a.X += amount;
	a.Y += amount;
	return a.Normalize();
}

CCircle CCircle::Offset(CVector2 size) const
{
	CCircle a = *this;
	a.X += size.X;
	a.Y += size.Y;
	return a.Normalize();
}

CCircle CCircle::Union(const CCircle& other) const
{
	f32 x1 = X,		  y1 = Y,		r1 = Radius;
	f32 x2 = other.X, y2 = other.Y, r2 = other.Radius;

	// R1 must be the smallest.
	if (r1 > r2)
	{
		f32 tx1 = x1, ty1 = y1, tr1 = r1;
		x1 = x2;
		y1 = y2;
		r1 = r2;
		x2 = tx1;
		y2 = ty1;
		r2 = tr1;
	}

	// Compute distance between circle centers.
	f32 dx = (x1 - x2);
	f32 dy = (y1 - y2);
	f32 d = Sqrt(dx*dx + dy*dy);

	// If we are fully within the circle, no need to do anything.
	if (d + r1 <= r2)
	{
		return CCircle(x2, y2, r2);
	}

	// Work out the closing radius.
	f32 enclosingRadius = (d + r1 + r2) / 2.0f;

	// Work out the center.
	f32 theta = 1.0f / 2.0f + (r2 - r1) / (2.0f * d);
	CVector2 center = (1.0f - theta) * CVector2(x1, y1) + theta * CVector2(x2, y2);

	return CCircle(center.X, center.Y, enclosingRadius);
}

CCircle	CCircle::Normalize() const
{
	CCircle a = *this;
	if (a.Radius < 0)
		a.Radius = 0;
	return a;
}

Engine::Containers::CString	CCircle::ToString()
{
	return S("(%f,%f,%f)").Format(X, Y, Radius);
}

bool CCircle::FromString(Engine::Containers::CString str, CCircle& a)
{
	if (str.Length() <= 0 ||								// Needs a string :3
		str[0] != '(' || str[str.Length() - 1] != ')' ||	// Needs the braces around it.
		str.Count(',') != 2)								// Needs exactly 1 seperating comma.
		return false;
	
	str = str.Trim("()");

	s32 idx = str.IndexOf(',');
	s32 idx2 = str.IndexOf(',', idx + 1);

	Engine::Containers::CString x = str.SubString(0, idx).Trim();
	Engine::Containers::CString y = str.SubString(idx + 1, (idx2 - idx) - 1).Trim();
	Engine::Containers::CString r = str.SubString(idx2 + 1).Trim();

	a.Set(x.ToFloat(), y.ToFloat(), r.ToFloat());

	return true;
}
	