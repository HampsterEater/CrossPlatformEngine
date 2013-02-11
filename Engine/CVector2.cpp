///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CVector2.h"
#include "CString.h"
#include "Math.h"

using namespace Engine::Math;

CVector2::CVector2()
{
	X = 0;
	Y = 0;
}

CVector2::CVector2(const f32 x, const f32 y)
{
	X = x;
	Y = y;
}

f32 CVector2::operator[](u32 index) const
{
	if (index == 0)
		return X;
	else 
		return Y;
}

f32 & CVector2::operator[](u32 index)
{
	if (index == 0)
		return X;
	else 
		return Y;
}

CVector2 CVector2::operator-() const
{
	return CVector2(-X, -Y);
}

CVector2 CVector2::operator*(const f32 a) const
{
	return CVector2(X * a, Y * a);
}

float CVector2::operator*(const CVector2 & a) const
{
	return X * a.X + Y * a.Y;
}

CVector2 CVector2::operator/(const f32 a) const
{
	float inva = 1.0f / a;
	return CVector2(X * inva, Y * inva);
}

CVector2 CVector2::operator+(const f32 a) const
{
	return CVector2(X + a, Y + a);
}

CVector2 CVector2::operator-(const f32 a) const
{
	return CVector2(X - a, Y - a);
}

CVector2 CVector2::operator/(const CVector2 & a) const
{
	return CVector2(X / a.X, Y / a.Y);
}

CVector2 CVector2::operator+(const CVector2 & a) const
{
	return CVector2(X + a.X, Y + a.Y);
}

CVector2 CVector2::operator-(const CVector2 & a) const
{
	return CVector2(X - a.X, Y - a.Y);
}

CVector2 & CVector2::operator*=(const f32 a) 
{
	X *= a;
	Y *= a;
	return *this;
}

CVector2 & CVector2::operator/=(const f32 a) 
{
	X /= a;
	Y /= a;
	return *this;
}

CVector2 & CVector2::operator+=(const f32 a) 
{
	X += a;
	Y += a;
	return *this;
}

CVector2 & CVector2::operator-=(const f32 a) 
{
	X -= a;
	Y -= a;
	return *this;
}

CVector2 & CVector2::operator*=(const CVector2 & a) 
{
	X *= a.X;
	Y *= a.Y;
	return *this;
}

CVector2 & CVector2::operator/=(const CVector2 & a) 
{
	X /= a.X;
	Y /= a.Y;
	return *this;
}

CVector2 & CVector2::operator+=(const CVector2 & a)
{
	X += a.X;
	Y += a.Y;
	return *this;
}

CVector2 & CVector2::operator-=(const CVector2 & a)
{
	X -= a.X;
	Y -= a.Y;
	return *this;
}

CVector2 Engine::Math::operator*(const f32 a, const CVector2 b)
{
	return CVector2(b.X * a, b.Y * a);
}

bool CVector2::Compare(const CVector2 & a) const
{
	return (X == a.X && Y == a.Y);
}

bool CVector2::Compare(const CVector2 & a, const float epsilon) const
{
	return (Abs(X - a.X) <= EPSILON && Abs(X - a.X) <= EPSILON);
}

bool CVector2::operator==(const CVector2 & a) const
{
	return Compare(a);
}

bool CVector2::operator!=(const CVector2 & a) const
{
	return !Compare(a);
}

bool CVector2::operator>(const CVector2 & a) const
{
	return Length() > a.Length();
}

bool CVector2::operator<(const CVector2 & a) const
{
	return Length() < a.Length();
}

bool CVector2::operator>=(const CVector2 & a) const
{
	return Length() >= a.Length();
}

bool CVector2::operator<=(const CVector2 & a) const
{	
	return Length() <= a.Length();
}

bool CVector2::IsUnitVector() const
{
	return Length() <= 1.0f;
}

bool CVector2::IsPerpendicular(const CVector2 & v2) const 
{
	return Dot(v2) == 0;
}

void CVector2::Set(const f32 x, const f32 y)
{
	X = x;
	Y = y;
}

void CVector2::Zero()
{
	X = 0;
	Y = 0;
}

f32 CVector2::Length() const
{
	return Sqrt(X * X + Y * Y);
}

f32 CVector2::LengthSquared() const
{
	return (X * X + Y * Y);
}

CVector2 CVector2::Normalize() const
{
	float x = X, y = Y;

	f32 sqrLen = x * x + y * y;
	f32 invLen = InvSqrt(sqrLen);

	x *= invLen;
	y *= invLen;

	return CVector2(x, y);
}

CVector2 CVector2::Truncate(const f32 length) const
{
	float x = X, y = Y;

	if (length == 0)
	{
		x = 0;
		y = 0;
	}
	else
	{
		f32 len = (x * x + y * y);
		if (len > length * length)
		{
			f32 trunlen = length * InvSqrt(len); 
			x *= trunlen;
			y *= trunlen;
		}
	}

	return CVector2(x, y);
}

CVector2 CVector2::Clamp(const CVector2& min, const CVector2& max) const
{
	float x = X, y = Y;

	if (x < min.X)
		x = min.X;
	else if (x > min.X)
		x = max.X;

	if (y < min.Y)
		y = min.Y;
	else if (y > min.Y)
		y = max.Y;

	return CVector2(x, y);
}

CVector2 CVector2::Ceil() const
{
	return CVector2(Engine::Math::Floor(X + 0.5f), Engine::Math::Floor(Y + 0.5f));
}

CVector2 CVector2::Floor() const
{
	return CVector2(f32(u32(X)), f32(u32(Y)));
}

f32 CVector2::Cross(const CVector2& b) const
{
	return X * b.Y - Y * b.X;
}

f32 CVector2::Dot(const CVector2& b) const
{
	return X * b.X + Y * b.Y;
}

f32 CVector2::Distance(const CVector2& b) const
{
	return (b - *this).Length();
}

CVector2 CVector2::Lerp(const CVector2& b, const f32 delta) const
{
	if (delta <= 0.0f)
		return *this;
	else if (delta >= 1.0f)
		return b;
	else
	{
		return *this + delta * (b - *this);
	}
}

CVector2 CVector2::SLerp(const CVector2& b, const f32 d) const
{
	if (d == 0) 
		return *this;
    
	if (*this == b || d == 1) 
		return b;

    f32 theta = ACos(Dot(b));
    if (theta == 0) return *this;

    f32 sinTheta = Sin(theta);
    return (f32)(Sin((1 - d) * theta) / sinTheta) * (*this) + (f32)(Sin(d * theta) / sinTheta) * b;
}

CVector2 CVector2::SmoothStep(const CVector2& b, const f32 delta) const
{
	f32 x = Engine::Math::SmoothStep(X, b.X, delta);
	f32 y = Engine::Math::SmoothStep(Y, b.Y, delta);

	return CVector2(x, y);
}

f32 CVector2::ToAngle() const
{
	return ATan2(Y, X);
}

f32	CVector2::Sum() const
{
	return X + Y;
}

CVector2 CVector2::Rotate(f32 rads) const
{
	f32 s = Sin(rads);
	f32 c = Cos(rads);
	f32 x = X * c - Y * s;
	f32 y = X * s + Y * c;
	return CVector2(x, y);
}

CVector2 CVector2::Reflect(const CVector2& normal) const
{
	return *this - (normal * 2.0f * Dot(normal));
}
			
CVector2 CVector2::Min(const CVector2& b) const
{
	return CVector2(Engine::Math::Min(X, b.X), Engine::Math::Min(Y, b.Y));
}

CVector2 CVector2::Max(const CVector2& b) const
{
	return CVector2(Engine::Math::Max(X, b.X), Engine::Math::Max(Y, b.Y));
}

Engine::Containers::CString CVector2::ToString()
{
	return S("(%f,%f)").Format(X, Y);
}

bool CVector2::FromString(Engine::Containers::CString str, CVector2& a)
{
	if (str.Length() <= 0 ||								// Needs a string :3
		str[0] != '(' || str[str.Length() - 1] != ')' ||	// Needs the braces around it.
		str.Count(',') != 1)								// Needs exactly 1 seperating comma.
		return false;

	str = str.Trim("() ");
	
	s32 idx = str.IndexOf(',');

	Engine::Containers::CString xs = str.SubString(0, idx).Trim();
	Engine::Containers::CString ys = str.SubString(idx + 1).Trim();

	a.Set(xs.ToFloat(), ys.ToFloat());

	return true;
}