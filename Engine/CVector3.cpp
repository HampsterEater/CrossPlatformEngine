///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CVector3.h"
#include "CString.h"
#include "Math.h"

using namespace Engine::Math;

CVector3::CVector3()
{
	X = 0;
	Y = 0;
	Z = 0;
}

CVector3::CVector3(const f32 x, const f32 y, const f32 z)
{
	X = x;
	Y = y;
	Z = z;
}

f32	CVector3::operator[](u32 index) const
{
	if (index == 0)
		return X;
	else if (index == 1)
		return Y;
	else 
		return Z;
}

f32 & CVector3::operator[](u32 index)
{
	if (index == 0)
		return X;
	else if (index == 1)
		return Y;
	else 
		return Z;
}

CVector3 CVector3::operator-() const
{
	return CVector3(-X, -Y, -Z);
}

CVector3 CVector3::operator*(const f32 a) const
{
	return CVector3(X * a, Y * a, Z * a);
}

float CVector3::operator*(const CVector3 & a) const
{
	return X * a.X + Y * a.Y + Z * a.Z;
}

CVector3 CVector3::operator/(const f32 a) const
{
	float inva = 1.0f / a;
	return CVector3(X * inva, Y * inva, Z * inva);
}

CVector3 CVector3::operator+(const f32 a) const
{
	return CVector3(X + a, Y + a, Z + a);
}

CVector3 CVector3::operator-(const f32 a) const
{
	return CVector3(X - a, Y - a, Z + a);
}

CVector3 CVector3::operator/(const CVector3 & a) const
{
	return CVector3(X / a.X, Y / a.Y, Z / a.Z);
}

CVector3 CVector3::operator+(const CVector3 & a) const
{
	return CVector3(X + a.X, Y + a.Y, Z + a.Z);
}

CVector3 CVector3::operator-(const CVector3 & a) const
{
	return CVector3(X - a.X, Y - a.Y, Z - a.Z);
}

CVector3 & CVector3::operator*=(const f32 a)
{
	X *= a;
	Y *= a;
	Z *= a;
	return *this;
}

CVector3 & CVector3::operator/=(const f32 a)
{
	X /= a;
	Y /= a;
	Z /= a;
	return *this;
}

CVector3 & CVector3::operator+=(const f32 a)
{
	X += a;
	Y += a;
	Z += a;
	return *this;
}

CVector3 & CVector3::operator-=(const f32 a)
{
	X -= a;
	Y -= a;
	Z -= a;
	return *this;
}

CVector3 & CVector3::operator*=(const CVector3 & a)
{
	X *= a.X;
	Y *= a.Y;
	Z *= a.Z;
	return *this;
}

CVector3 & CVector3::operator/=(const CVector3 & a)
{
	X /= a.X;
	Y /= a.Y;
	Z /= a.Z;
	return *this;
}

CVector3 & CVector3::operator+=(const CVector3 & a)
{
	X += a.X;
	Y += a.Y;
	Z += a.Z;
	return *this;
}

CVector3 & CVector3::operator-=(const CVector3 & a)
{
	X -= a.X;
	Y -= a.Y;
	Z -= a.Z;
	return *this;
}

CVector3 Engine::Math::operator*(const f32 a, const CVector3 b)
{
	return CVector3(b.X * a, b.Y * a, b.Z * a);
}

bool CVector3::Compare(const CVector3 & a) const
{
	return (X == a.X && Y == a.Y && Z == a.Z);
}

bool CVector3::Compare(const CVector3 & a, const f32 epsilon) const
{
	return (Abs(X - a.X) <= EPSILON && Abs(X - a.X) <= EPSILON && Abs(Z - a.Z) <= EPSILON);
}

bool CVector3::operator==(const CVector3 & a) const
{
	return Compare(a);
}

bool CVector3::operator!=(const CVector3 & a) const
{
	return !Compare(a);
}

bool CVector3::operator>(const CVector3 & a) const
{
	return Length() > a.Length();
}

bool CVector3::operator<(const CVector3 & a) const
{
	return Length() < a.Length();
}

bool CVector3::operator>=(const CVector3 & a) const
{
	return Length() >= a.Length();
}

bool CVector3::operator<=(const CVector3 & a) const
{
	return Length() <= a.Length();
}

bool CVector3::IsUnitVector() const
{
	return Length() <= 1.0f;
}

bool CVector3::IsPerpendicular(const CVector3 & v2) const
{
	return Dot(v2) == 0;
}

void CVector3::Set(const f32 x, const f32 y, const f32 z)
{
	X = x;
	Y = y;
	Z = z;
}

void CVector3::Zero()
{
	X = 0;
	Y = 0;
	Z = 0;
}

f32 CVector3::Length() const
{
	return Sqrt(X * X + Y * Y + Z * Z);
}

f32	CVector3::LengthSquared() const
{
	return (X * X + Y * Y + Z * Z);
}

f32 CVector3::ToPitch() const
{
	f32 yaw = 0.0f;

	if (Y == 0.0f && X == 0.0f)
	{
		yaw = 0.0f;
	}
	else
	{
		yaw = ToDeg(ATan2(Y, X));
		if (yaw <= 360)
		{
			yaw += 360;
		}
	}

	return ToRad(yaw);
}

f32 CVector3::ToYaw() const
{
	f32 forward = 0.0f;
	f32 pitch = 0.0f;

	if (X == 0.0f && Y == 0.0f)
	{
		if (Z > 0.0f)
		{
			pitch = 90.0f;
		}
		else
		{
			pitch = 270.0f;
		}
	}
	else
	{
		forward = (f32)Sqrt(X * X + Y * Y);
		pitch = ToDeg(ATan2( Z, forward ));
		if (pitch < 0.0f)
		{
			pitch += 360.0f;
		}
	}

	return ToRad(pitch);
}

f32	CVector3::Sum() const
{
	return X + Y + Z;
}

CVector3 CVector3::RotatePitch(f32 rads) const
{
	f32 x = X;
	f32 y = ( Y * Cos(rads) ) - ( Z * Sin(rads) );
	f32 z = ( Y * Sin(rads) ) + ( Z * Cos(rads) );
	return CVector3(x, y, z);
}

CVector3 CVector3::RotateYaw(f32 rads) const
{   
	f32 x = ( Z * Sin(rads) ) + ( X * Cos(rads) );
	f32 y = Y;
	f32 z = ( Z * Cos(rads) ) - ( X * Sin(rads) );
	return CVector3(x, y, z);
}

CVector3 CVector3::RotateRoll(f32 rads) const
{
	f32 x = ( X * Cos(rads) ) - ( Y * Sin(rads) );
	f32 y = ( X * Sin(rads) ) + ( Y * Cos(rads) );
	f32 z = Z;
	return CVector3(x, y, z);
}

CVector3 CVector3::Normalize() const
{
	float x = X, y = Y, z = Z;

	f32 sqrLen = x * x + y * y + z * z;
	f32 invLen = InvSqrt(sqrLen);

	x *= invLen;
	y *= invLen;
	z *= invLen;

	return CVector3(x, y, z);
}

CVector3 CVector3::Truncate(const f32 length) const
{
	float x = X, y = Y, z = Z;

	if (length == 0)
	{
		x = 0;
		y = 0;
		z = 0;
	}
	else
	{
		f32 len = (x * x + y * y + z * z);
		if (len > length * length)
		{
			f32 trunlen = length * InvSqrt(len); 
			x *= trunlen;
			y *= trunlen;
			z *= trunlen;
		}
	}

	return CVector3(x, y, z);
}

CVector3 CVector3::Clamp(const CVector3& min, const CVector3& max) const
{
	float x = X, y = Y, z = Z;

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

	return CVector3(x, y, z);
}

CVector3 CVector3::Ceil() const
{
	return CVector3(Engine::Math::Floor(X + 0.5f), Engine::Math::Floor(Y + 0.5f), Engine::Math::Floor(Z + 0.5f));
}

CVector3 CVector3::Floor() const
{
	return CVector3(f32(u32(X)), f32(u32(Y)), f32(u32(Z)));
}

CVector3 CVector3::Cross(const CVector3& b) const
{
	f32 x = X * b.Z - Z * b.Y;
	f32 y = Z * b.X - X * b.Z;
	f32 z = X * b.Y - Y * b.X;
	return CVector3(x, y, z);
}

f32 CVector3::Dot(const CVector3& b) const
{
	return X * b.X + Y * b.Y + Z * b.Z;
}

f32 CVector3::Distance(const CVector3& b) const
{
	return (b - *this).Length();
}

CVector3 CVector3::Lerp(const CVector3& b, const f32 delta) const
{
	if (delta <= 0.0f)
		return *this;
	else if (delta >= 1.0f)
		return b;
	else
	{
		return *this + delta * (b - (*this));
	}
}

CVector3 CVector3::SLerp(const CVector3& b, const f32 d) const
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

CVector3 CVector3::SmoothStep(const CVector3& b, const f32 delta) const
{
	f32 x = Engine::Math::SmoothStep(X, b.X, delta);
	f32 y = Engine::Math::SmoothStep(Y, b.Y, delta);
	f32 z = Engine::Math::SmoothStep(Z, b.Z, delta);

	return CVector3(x, y, z);
}

CVector3 CVector3::Reflect(const CVector3& normal) const
{
	return *this - (normal * 2.0f * Dot(normal));
}

CVector3 CVector3::Min(const CVector3& b) const
{	
	return CVector3(Engine::Math::Min(X, b.X), Engine::Math::Min(Y, b.Y), Engine::Math::Min(Z, b.Z));
}

CVector3 CVector3::Max(const CVector3& b) const
{
	return CVector3(Engine::Math::Max(X, b.X), Engine::Math::Max(Y, b.Y), Engine::Math::Max(Z, b.Z));
}

bool CVector3::IsBackFace(const CVector3& normal, const CVector3& lineOfSight)
{
	return normal.Dot(lineOfSight) < 0;
}

void CVector3::NormalVectors(CVector3& left, CVector3& down) const
{
	float d;

	d = X * X + Y * Y;
	if (!d)
	{
		left = CVector3(1, 0, 0);
	}
	else
	{		
		d = InvSqrt(d);
		left = CVector3(-Y * d, X * d, 0);
	}

	down = left.Cross(*this);
}

void CVector3::OrthogonalBasis(CVector3& left, CVector3& up) const
{
	f32 l, s;

	if (Abs(Z) > 0.7f)
	{
		l = Y * Y + Z * Z;
		s = InvSqrt(l);
		up = CVector3(0, Z * s, -Y * s);
		left = CVector3(l * s, -X * up.Z, X * up.Y);
	}
	else
	{
		l = X * X + Y * Y;
		s = InvSqrt(l);
		left = CVector3(-Y * s, X * s, 0.0f);
		up = CVector3(-Z * left.Y, Z * left.X, l * s);
	}
}

CVector3 CVector3::ProjectOntoPlane(const CVector3& normal, const f32 overBounce) const
{
	f32 backoff;

	backoff = ((*this) * normal);

	if (overBounce != 1.0f)
	{
		if (backoff < 0)
		{
			backoff *= overBounce;
		}
		else
		{
			backoff /= overBounce;
		}
	}

	return CVector3(*this) - backoff * normal;
}

CVector3 CVector3::ProjectAlongPlane(const CVector3& normal, const f32 epsilon, const f32 overBounce) const
{
	CVector3 cross;
	f32 len;

	cross = Cross(normal).Cross(*this);
	cross = cross.Normalize();
	len = (normal * cross);

	if (Abs(len) < epsilon)
	{
		return CVector3(0, 0, 0);
	}

	cross *= overBounce * (normal * (*this)) / len;

	return CVector3(*this) - cross;
}

CVector3 CVector3::ProjectOntoSphere(const f32 radius) const
{
	f32 rsqr = radius * radius;
	f32 len = Length();
	f32 z = 0;

	if (len < rsqr * 0.5f)
	{
		z = Sqrt(rsqr - len);
	}
	else
	{
		z = rsqr / (2.0f * Sqrt(len));
	}

	return CVector3(X, Y, z);
}

Engine::Containers::CString CVector3::ToString()
{
	return S("(%f,%f,%f)").Format(X, Y, Z);
}

bool CVector3::FromString(Engine::Containers::CString str, CVector3& a)
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
	Engine::Containers::CString z = str.SubString(idx2 + 1).Trim();

	a.Set(x.ToFloat(), y.ToFloat(), z.ToFloat());

	return true;
}

