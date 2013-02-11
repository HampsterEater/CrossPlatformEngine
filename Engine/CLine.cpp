///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CLine.h"
#include "CRect.h"
#include "CCircle.h"
#include "CVector2.h"
#include "CString.h"
#include "Math.h"

using namespace Engine::Math;

CVector2 CLine::operator[](u32 index) const
{
	if (index == 0)
		return Start;
	else
		return End;
}
CVector2 & CLine::operator[](u32 index)
{
	if (index == 0)
		return Start;
	else
		return End;
}
				
bool CLine::Compare(const CLine & a) const
{
	return Start.Compare(a.Start) && End.Compare(a.End);
}

bool CLine::Compare(const CLine & a, const f32 epsilon) const
{
	return Start.Compare(a.Start, epsilon) && End.Compare(a.End, epsilon);
}

bool CLine::operator==(const CLine & a) const
{
	return Compare(a);
}

bool CLine::operator!=(const CLine & a) const
{
	return !Compare(a);
}

void CLine::Set(const f32 x, const f32 y, const f32 x2, const f32 y2)
{
	Start.X = x;
	Start.Y = y;
	End.X = x2;
	End.Y = y2;
}

void CLine::Set(const CVector2& start, const CVector2& end)
{
	Start = start;
	End = end;
}

void CLine::Zero()
{
	Start.X = 0;
	Start.Y = 0;
	End.X = 0;
	End.Y = 0;
}
				
CLine::CLine(f32 x, f32 y, f32 x2, f32 y2)
{
	Start.X = x;
	Start.Y = y;
	End.X = x2;
	End.Y = y2;
}

CLine::CLine(const CVector2& start, const CVector2& end)
{
	Start = start;
	End = end;
}

CLine::CLine(const CLine& r)
{
	Start = r.Start;
	End = r.End;
}

f32 CLine::GetLength() const
{
	f32 dx = (End.X - Start.X);
	f32 dy = (End.Y - Start.Y);
	f32 d = Sqrt(dx*dx + dy*dy);
	return d;
}
				
CVector2 CLine::GetCenter() const
{
	return (Start + End) / 2.0f;
}

CVector2 CLine::GetPoint(f32 delta) const
{
	f32 x = Start.X + ((End.X - Start.X) * delta);
	f32 y = Start.Y + ((End.Y - Start.Y) * delta);
	return CVector2(x, y);
}

CLine CLine::Rotate(f32 angle) const
{
	CVector2 center = GetCenter();
	
	f32 distanceToStart = Start.Distance(center);
	f32 distanceToEnd   = End.Distance(center);
	f32 angleToStart = (Start - center).ToAngle();
	f32 angleToEnd   = (End - center).ToAngle();

	CLine extended = *this;
	extended.Start.X = center.X + (Cos(angleToStart + angle) * (distanceToStart));
	extended.Start.Y = center.Y + (Sin(angleToStart + angle) * (distanceToStart));
	extended.End.X   = center.X + (Cos(angleToEnd + angle)   * (distanceToEnd));
	extended.End.Y   = center.Y + (Sin(angleToEnd + angle)   * (distanceToEnd));
	return extended;
}

bool CLine::Overlaps(const CLine& other) const
{
	CVector2 point;
	return IntersectPoint(other, point);
}

bool CLine::Overlaps(const CRect& other) const
{
	return other.Overlaps(*this);
}

bool CLine::Overlaps(const CCircle& other) const
{
	f32 x1 = Start.X;
	f32 y1 = Start.Y;
	f32 x2 = End.X;
	f32 y2 = End.Y;
	f32 cx = other.X;
	f32 cy = other.Y;
	f32 radius = other.Radius;

	f32 dx31 = cx - x1;
	f32 dx21 = x2 - x1;
	f32 dy31 = cy - y1;
	f32 dy21 = y2 - y1;
	f32 d = (dx21 * dx21) + (dy21 * dy21);

	if (d != 0)		d = ((dx31 * dx21) + (dy31 * dy21)) / d;
	if (d < 0.0)	d = 0;
	if (d > 1.0)	d = 1;

	f32 dx = cx - (x1 + (dx21 * d));
	f32 dy = cy - (y1 + (dy21 * d));

	return (radius * radius >= (dx * dx) + (dy * dy));
}

bool CLine::Contains(const CVector2& other) const
{
	return NearestPoint(other).Distance(other) <= 1.5f;
}

u32 CLine::Orientation(f32 x1, f32 y1, f32 x2, f32 y2, f32 Px, f32 Py) const
{
	f32 val = (x2 - x1) * (Py - y1) - (Px - x1) * (y2 - y1);
	return (val < 0 ? -1 : (val > 0 ? 1 : 0));
}

CVector2 CLine::NearestPoint(const CVector2& to) const
{
	f32 lx1 = Start.X;
	f32 ly1 = Start.Y;
	f32 lx2 = End.X;
	f32 ly2 = End.Y;
	f32 x = to.X;
	f32 y = to.Y;

	f32 dx = lx2 - lx1;
	f32 dy = ly2 - ly1;
	u32 ori1 = Orientation(lx1, ly1, (lx1+dy), (ly1-dx), x, y);
	u32 ori2 = Orientation(lx2, ly2, (lx2+dy), (ly2-dx), x, y);
	
	CVector2 inter(0, 0);

	if ((ori1 == 1 && ori2 == 1) || ori2 == 0)
	{
		inter.X = lx2;
		inter.Y = ly2;
	}
	else if ((ori1 == -1 && ori2 == -1) || ori1 == 0)
	{
		inter.X = lx1;
		inter.Y = ly1;
	}
	else
	{
		CLine a = CLine(lx1, ly1, lx2, ly2);
		CLine b = CLine(x, y, x + dy, y - dx);
		a.IntersectPoint(b, inter);
	}

	return inter;
}

bool CLine::IntersectPoint(const CRect& other, CVector2& a) const
{
	return other.IntersectPoint(*this, a);
}

bool CLine::IntersectPoint(const CLine& other, CVector2& a) const
{
	f32 x1 = Start.X, y1 = Start.Y;
	f32 x2 = End.X, y2 = End.Y;
	f32 x3 = other.Start.X, y3 = other.Start.Y;
	f32 x4 = other.End.X, y4 = other.End.Y;

	f32 l1d = 0.0f;
	f32 l2d = 0.0f;
	f32 l1x = 0.0f, l1y = 0.0f, l2x = 0.0f, l2y = 0.0f;

	l1x = x2 - x1;
	l1y = y2 - y1;
	l2x = x4 - x3;
	l2y = y4 - y3;

	l1d = (y3 - y1 + (l2y / l2x) * (x1 - x3)) / (l1y - l2y * l1x / l2x);
	if (l1d >= 0 && l1d <= 1)
	{
		l2d = (x1 + l1x * l1d - x3) / l2x;
		if (l2d >= 0 && l2d <= 1)
		{
			a.X = x1 + l1d * l1x;
			a.Y = y1 + l1d * l1y;
			return true;
		}
	}

	return false;
}

CLine CLine::Inflate(f32 amount) const
{
	CVector2 center = GetCenter();
	
	f32 distanceToStart = Start.Distance(center);
	f32 distanceToEnd   = End.Distance(center);
	f32 angleToStart = (Start - center).ToAngle();
	f32 angleToEnd   = (End - center).ToAngle();

	CLine extended = *this;
	extended.Start.X = center.X + (Cos(angleToStart) * (distanceToStart + amount));
	extended.Start.Y = center.Y + (Sin(angleToStart) * (distanceToStart + amount));
	extended.End.X   = center.X + (Cos(angleToEnd)   * (distanceToEnd + amount));
	extended.End.Y   = center.Y + (Sin(angleToEnd)   * (distanceToEnd + amount));
	return extended;
}

CLine CLine::Deflate(f32 amount) const
{
	CVector2 center = GetCenter();
	
	f32 distanceToStart = Start.Distance(center);
	f32 distanceToEnd   = End.Distance(center);
	f32 angleToStart = (Start - center).ToAngle();
	f32 angleToEnd   = (End - center).ToAngle();

	CLine extended = *this;
	extended.Start.X = center.X + (Cos(angleToStart) * (distanceToStart - amount));
	extended.Start.Y = center.Y + (Sin(angleToStart) * (distanceToStart - amount));
	extended.End.X   = center.X + (Cos(angleToEnd)   * (distanceToEnd - amount));
	extended.End.Y   = center.Y + (Sin(angleToEnd)   * (distanceToEnd - amount));
	return extended;
}

CLine CLine::Scale(f32 amount) const
{
	CVector2 center = GetCenter();
	
	f32 distanceToStart = Start.Distance(center);
	f32 distanceToEnd   = End.Distance(center);
	f32 angleToStart = (Start - center).ToAngle();
	f32 angleToEnd   = (End - center).ToAngle();

	CLine extended = *this;
	extended.Start.X = center.X + (Cos(angleToStart) * (distanceToStart * amount));
	extended.Start.Y = center.Y + (Sin(angleToStart) * (distanceToStart * amount));
	extended.End.X   = center.X + (Cos(angleToEnd)   * (distanceToEnd * amount));
	extended.End.Y   = center.Y + (Sin(angleToEnd)   * (distanceToEnd * amount));
	return extended;
}

CLine CLine::Offset(f32 amount) const
{
	CLine extended = *this;
	extended.Start.X += amount;
	extended.Start.Y += amount;
	extended.End.X += amount;
	extended.End.Y += amount;
	return extended;
}

CLine CLine::Offset(CVector2 size) const
{
	CLine extended = *this;
	extended.Start += size;
	extended.End += size;
	return extended;
}

Engine::Containers::CString	CLine::ToString()
{
	return S("(%f,%f,%f,%f)").Format(Start.X, Start.Y, End.X, End.Y);
}

bool CLine::FromString(Engine::Containers::CString str, CLine& a)
{
	if (str.Length() <= 0 ||								// Needs a string :3
		str[0] != '(' || str[str.Length() - 1] != ')' ||	// Needs the braces around it.
		str.Count(',') != 3)								// Needs exactly 1 seperating comma.
		return false;
	
	str = str.Trim("() ");

	s32 idx = str.IndexOf(',');
	s32 idx2 = str.IndexOf(',', idx + 1);
	s32 idx3 = str.IndexOf(',', idx2 + 1);

	Engine::Containers::CString sx = str.SubString(0, idx).Trim();
	Engine::Containers::CString sy = str.SubString(idx + 1, (idx2 - idx) - 1).Trim();
	Engine::Containers::CString ex = str.SubString(idx2 + 1, (idx3 - idx2) - 1).Trim();
	Engine::Containers::CString ey = str.SubString(idx3 + 1).Trim();

	a.Set(sx.ToFloat(), sy.ToFloat(), ex.ToFloat(), ey.ToFloat());

	return true;
}
	