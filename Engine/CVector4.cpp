///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CVector4.h"
#include "CString.h"
#include "Math.h"

using namespace Engine::Math;

CVector4::CVector4()
{
	X = 0;
	Y = 0;
	Z = 0;
	W = 0;
}

CVector4::CVector4(const f32 x, const f32 y, const f32 z, const f32 w)
{
	X = x;
	Y = y;
	Z = z;
	W = w;
}

f32	CVector4::operator[](u32 index) const
{
	if (index == 0)
		return X;
	else if (index == 1)
		return Y;
	else if (index == 2)
		return Z;
	else 
		return W;
}

f32 & CVector4::operator[](u32 index)
{
	if (index == 0)
		return X;
	else if (index == 1)
		return Y;
	else if (index == 2)
		return Z;
	else 
		return W;
}

CVector4 CVector4::operator-() const
{
	return CVector4(-X, -Y, -Z, -W);
}

CVector4 CVector4::operator*(const f32 a) const
{
	return CVector4(X * a, Y * a, Z * a, W * a);
}

float CVector4::operator*(const CVector4 & a) const
{
	return X * a.X + Y * a.Y + Z * a.Z + W * a.W;
}

CVector4 CVector4::operator/(const f32 a) const
{
	float inva = 1.0f / a;
	return CVector4(X * inva, Y * inva, Z * inva, W * inva);
}

CVector4 CVector4::operator+(const f32 a) const
{
	return CVector4(X + a, Y + a, Z + a, W + a);
}

CVector4 CVector4::operator-(const f32 a) const
{
	return CVector4(X - a, Y - a, Z + a, W + a);
}

CVector4 CVector4::operator/(const CVector4 & a) const
{
	return CVector4(X / a.X, Y / a.Y, Z / a.Z, W / a.W);
}

CVector4 CVector4::operator+(const CVector4 & a) const
{
	return CVector4(X + a.X, Y + a.Y, Z + a.Z, W + a.W);
}

CVector4 CVector4::operator-(const CVector4 & a) const
{
	return CVector4(X - a.X, Y - a.Y, Z - a.Z, W - a.W);
}

CVector4 & CVector4::operator*=(const f32 a)
{
	X *= a;
	Y *= a;
	Z *= a;
	W *= a;
	return *this;
}

CVector4 & CVector4::operator/=(const f32 a)
{
	X /= a;
	Y /= a;
	Z /= a;
	W /= a;
	return *this;
}

CVector4 & CVector4::operator+=(const f32 a)
{
	X += a;
	Y += a;
	Z += a;
	W += a;
	return *this;
}

CVector4 & CVector4::operator-=(const f32 a)
{
	X -= a;
	Y -= a;
	Z -= a;
	W -= a;
	return *this;
}

CVector4 & CVector4::operator*=(const CVector4 & a)
{
	X *= a.X;
	Y *= a.Y;
	Z *= a.Z;
	W *= a.W;
	return *this;
}

CVector4 & CVector4::operator/=(const CVector4 & a)
{
	X /= a.X;
	Y /= a.Y;
	Z /= a.Z;
	W /= a.W;
	return *this;
}

CVector4 & CVector4::operator+=(const CVector4 & a)
{
	X += a.X;
	Y += a.Y;
	Z += a.Z;
	W += a.W;
	return *this;
}

CVector4 & CVector4::operator-=(const CVector4 & a)
{
	X -= a.X;
	Y -= a.Y;
	Z -= a.Z;
	W -= a.W;
	return *this;
}

CVector4 Engine::Math::operator*(const f32 a, const CVector4 b)
{
	return CVector4(b.X * a, b.Y * a, b.Z * a, b.W * a);
}

bool CVector4::Compare(const CVector4 & a) const
{
	return (X == a.X && Y == a.Y && Z == a.Z && W == a.W);
}

bool CVector4::Compare(const CVector4 & a, const f32 epsilon) const
{
	return (Abs(X - a.X) <= EPSILON && Abs(X - a.X) <= EPSILON && Abs(Z - a.Z) <= EPSILON && Abs(W - a.W) <= EPSILON);
}

bool CVector4::operator==(const CVector4 & a) const
{
	return Compare(a);
}

bool CVector4::operator!=(const CVector4 & a) const
{
	return !Compare(a);
}

bool CVector4::operator>(const CVector4 & a) const
{
	return Length() > a.Length();
}

bool CVector4::operator<(const CVector4 & a) const
{
	return Length() < a.Length();
}

bool CVector4::operator>=(const CVector4 & a) const
{
	return Length() >= a.Length();
}

bool CVector4::operator<=(const CVector4 & a) const
{
	return Length() <= a.Length();
}

bool CVector4::IsUnitVector() const
{
	return Length() <= 1.0f;
}

bool CVector4::IsPerpendicular(const CVector4 & v2) const
{
	return Dot(v2) == 0;
}

void CVector4::Set(const f32 x, const f32 y, const f32 z, const f32 w)
{
	X = x;
	Y = y;
	Z = z;
	W = w;
}

void CVector4::Zero()
{
	X = 0;
	Y = 0;
	Z = 0;
	W = 0;
}

f32 CVector4::Length() const
{
	return Sqrt(X * X + Y * Y + Z * Z + W * W);
}

f32 CVector4::LengthSquared() const
{
	return (X * X + Y * Y + Z * Z + W * W);
}

f32	CVector4::Sum() const
{
	return X + Y + Z + W;
}

CVector4 CVector4::Normalize() const
{
	float x = X, y = Y, z = Z, w = W;

	f32 sqrLen = x * x + y * y + z * z + w * w;
	f32 invLen = InvSqrt(sqrLen);

	x *= invLen;
	y *= invLen;
	z *= invLen;
	w *= invLen;

	return CVector4(x, y, z, w);
}

CVector4 CVector4::Truncate(const f32 length) const
{
	float x = X, y = Y, z = Z, w = W;

	if (length == 0)
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	else
	{
		f32 len = (x * x + y * y + z * z + w * w);
		if (len > length * length)
		{
			f32 trunlen = length * InvSqrt(len); 
			x *= trunlen;
			y *= trunlen;
			z *= trunlen;
			w *= trunlen;
		}
	}

	return CVector4(x, y, z, w);
}

CVector4 CVector4::Clamp(const CVector4& min, const CVector4& max) const
{
	float x = X, y = Y, z = Z, w = W;

	if (x < min.X)
		x = min.X;
	else if (x > min.X)
		x = max.X;

	if (y < min.Y)
		y = min.Y;
	else if (y > min.Y)
		y = max.Y;

	if (z < min.Z)
		z = min.Z;
	else if (z > min.Z)
		z = max.Z;

	if (w < min.W)
		w = min.W;
	else if (w > min.W)
		w = max.W;

	return CVector4(x, y, z, w);
}

CVector4 CVector4::Ceil() const
{	
	return CVector4(Engine::Math::Floor(X + 0.5f), Engine::Math::Floor(Y + 0.5f), Engine::Math::Floor(Z + 0.5f), Engine::Math::Floor(W + 0.5f));
}

CVector4 CVector4::Floor() const
{
	return CVector4(f32(u32(X)), f32(u32(Y)), f32(u32(Z)), f32(u32(W)));
}

f32 CVector4::Dot(const CVector4& b) const
{
	return X * b.X + Y * b.Y + Z * b.Z + W * b.W;
}

f32 CVector4::Distance(const CVector4& b) const
{
	return (b - *this).Length();
}

CVector4 CVector4::Lerp(const CVector4& b, const f32 delta) const
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

CVector4 CVector4::SLerp(const CVector4& b, const f32 d) const
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

CVector4 CVector4::SmoothStep(const CVector4& b, const f32 delta) const
{
	f32 x = Engine::Math::SmoothStep(X, b.X, delta);
	f32 y = Engine::Math::SmoothStep(Y, b.Y, delta);
	f32 z = Engine::Math::SmoothStep(Z, b.Z, delta);
	f32 w = Engine::Math::SmoothStep(W, b.W, delta);

	return CVector4(x, y, z, w);
}

CVector4 CVector4::Reflect(const CVector4& normal) const
{
	return *this - (normal * 2.0f * Dot(normal));
}

CVector4 CVector4::Min(const CVector4& b) const
{	
	return CVector4(Engine::Math::Min(X, b.X), Engine::Math::Min(Y, b.Y), Engine::Math::Min(Z, b.Z), Engine::Math::Min(W, b.W));
}

CVector4 CVector4::Max(const CVector4& b) const
{
	return CVector4(Engine::Math::Max(X, b.X), Engine::Math::Max(Y, b.Y), Engine::Math::Max(Z, b.Z), Engine::Math::Max(W, b.W));
}

Engine::Containers::CString CVector4::ToString()
{
	return S("(%f,%f,%f,%f)").Format(X, Y, Z, W);
}

bool CVector4::FromString(Engine::Containers::CString str, CVector4& a)
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
	Engine::Containers::CString z = str.SubString(idx2 + 1, (idx3 - idx2) - 1).Trim();
	Engine::Containers::CString w = str.SubString(idx3 + 1).Trim();

	a.Set(x.ToFloat(), y.ToFloat(), z.ToFloat(), w.ToFloat());

	return true;
}
