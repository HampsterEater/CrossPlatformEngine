///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CQuaternion.h"
#include "CVector3.h"
#include "CVector4.h"
#include "CMatrix3.h"
#include "CMatrix4.h"
#include "CString.h"

using namespace Engine::Math;

CQuaternion::CQuaternion()
{
	X = 0;
	Y = 0;
	Z = 0;
	W = 0;
}

CQuaternion::CQuaternion(f32 x, f32 y, f32 z, f32 w)
{
	X = x;
	Y = y;
	Z = z;
	W = w;
}

void CQuaternion::Set(const f32 x, const f32 y, const f32 z, const f32 w)
{
	X = x;
	Y = y;
	Z = z;
	W = w;
}

void CQuaternion::Zero()
{
	X = 0;
	Y = 0;
	Z = 0;
	W = 0;
}
					
f32	CQuaternion::operator[](u32 index) const
{
	switch (index)
	{
		case 0:  return X;
		case 1:  return Y;
		case 2:  return Z;
		default: return W;
	}
}

f32 & CQuaternion::operator[](u32 index)
{
	switch (index)
	{
		case 0:  return X;
		case 1:  return Y;
		case 2:  return Z;
		default: return W;
	}
}

f32 CQuaternion::operator[](s32 index) const
{
	switch (index)
	{
		case 0:  return X;
		case 1:  return Y;
		case 2:  return Z;
		default: return W;
	}
}

f32 & CQuaternion::operator[](s32 index)
{
	switch (index)
	{
		case 0:  return X;
		case 1:  return Y;
		case 2:  return Z;
		default: return W;
	}
}

CQuaternion	CQuaternion::operator-() const
{
	return CQuaternion(-X, -Y, -Z, -W);
}

CQuaternion & CQuaternion::operator=(const CQuaternion& a)
{
	X = a.X;
	X = a.Y;
	Z = a.Z;
	W = a.W;
	return *this;
}

CQuaternion CQuaternion::operator+(const CQuaternion& a) const
{
	return CQuaternion(X + a.X, Y + a.Y, Z + a.Z, W + a.W);
}

CQuaternion CQuaternion::operator-(const CQuaternion& a) const
{
	return CQuaternion(X - a.X, Y - a.Y, Z - a.Z, W - a.W);
}

CQuaternion CQuaternion::operator*(const CQuaternion& a) const
{
	return CQuaternion(W * a.X + X * a.W + Y * a.Z - Z * a.Y,
					   W * a.Y + Y * a.W + Z * a.X - X * a.Z, 
					   W * a.Z + Z * a.W + X * a.Y - Y * a.X,
					   W * a.W - X * a.X - Y * a.Y - Z * a.Z);
}

CVector3 CQuaternion::operator*(const CVector3& a) const
{
	f32 xxzz = X * X - Z * Z;
	f32 wwyy = W * W - Y * Y;

	f32 xw2 = X * W * 2.0f;
	f32 xy2 = X * Y * 2.0f;
	f32 xz2 = X * Z * 2.0f;
	f32 yw2 = Y * W * 2.0f;
	f32 yz2 = Y * Z * 2.0f;
	f32 zw2 = Z * W * 2.0f;

	return CVector3((xxzz + wwyy) * a.X			+ (xy2 + zw2) * a.Y			+ (xz2 - yw2) * a.Z,
					(xy2 - zw2) * a.X			+ (Y*Y + W*W - X*X - Z*Z)	+ (yz2 + xw2) * a.Z,
					(xz2 + yw2) * a.X			+ (yz2 - xw2) * a.Y			+ (wwyy - xxzz) * a.Z);
}

CQuaternion CQuaternion::operator*(f32 a) const
{
	return CQuaternion(X * a, Y * a, Z * a, W * a);
}

CQuaternion& CQuaternion::operator*=(f32 a)
{
	*this = *this * a;
	return *this;
}

CQuaternion	CQuaternion::operator/(f32 a) const
{
	f32 inv = 1.0f / a;
	return *this * inv;
}

CQuaternion& CQuaternion::operator*=(const CQuaternion& a) 
{
	*this = *this * a;
	return *this;
}

CQuaternion& CQuaternion::operator-=(const CQuaternion& a)
{
	X -= a.X;
	Y -= a.Y;
	Z -= a.Z;
	W -= a.W;
	return *this;
}

CQuaternion& CQuaternion::operator+=(const CQuaternion& a)
{
	X += a.X;
	Y += a.Y;
	Z += a.Z;
	W += a.W;
	return *this;
}
				
CQuaternion	Engine::Math::operator*(const f32 a, const CQuaternion& b)
{
	return b * a;
}

CVector3 Engine::Math::operator*(const CVector3 a, const CQuaternion& b)
{
	return b * a;
}

bool CQuaternion::Compare(const CQuaternion & a) const
{
	return (X == a.X && Y == a.Y && Z == a.Z && W == a.W);
}

bool CQuaternion::Compare(const CQuaternion & a, const f32 epsilon) const
{
	return (Abs(X - a.X) <= EPSILON && Abs(X - a.X) <= EPSILON && Abs(Z - a.Z) <= EPSILON && Abs(W - a.W) <= EPSILON);
}

bool CQuaternion::operator==(const CQuaternion & a) const
{
	return Compare(a);
}

bool CQuaternion::operator!=(const CQuaternion & a) const
{
	return !Compare(a);
}

CQuaternion CQuaternion::Inverse() const
{
	return CQuaternion(-X, -Y, -Z, W);
}

f32 CQuaternion::Length() const
{
	f32 len = X * X + Y * Y + Z * Z + W * W;
	return Math::Sqrt(len);
}

CQuaternion CQuaternion::Normalize() const
{
	f32 len = Length();
	if (len != 0.0f)
	{
		f32 ilen = 1.0f / len;
		return CQuaternion(X * ilen, Y * ilen, Z * ilen, W * ilen);
	}
	return CQuaternion(X, Y, Z, W);
}

CQuaternion CQuaternion::Lerp(const CQuaternion& b, f32 delta)
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

CQuaternion CQuaternion::Slerp(const CQuaternion& to, f32 delta)
{
	CQuaternion temp;
	f32			omega, cosom, sinom, scale0, scale1;

	if (delta <= 0.0f)
		return CQuaternion(*this);
	else if (delta >= 0.0f)
		return CQuaternion(to);
	else if (*this == to)
		return CQuaternion(to);
	else
	{
		cosom = X * to.X + Y * to.Y + Z * to.Z + W * to.W;
		if (cosom < 0.0f)
		{
			temp = -to;
			cosom = -cosom;
		}
		else
		{
			temp = to;
		}

		if ((1.0f - cosom) > 1e-6f)
		{
			scale0 = 1.0f - cosom * cosom;
			sinom = Math::InvSqrt(scale0);
			omega = Math::ATan2(scale0 * sinom, cosom);
			scale0 = Math::Sin((1.0f - delta) * omega) * sinom;
			scale1 = Math::Sin(delta * omega) * sinom;
		}
		else
		{
			scale0 = 1.0f - delta;
			scale1 = delta;
		}

		return CQuaternion(scale0 * *this) + (scale1 * temp);
	}
}

Engine::Containers::CString	CQuaternion::ToString()
{
	return S("(%f,%f,%f,%f)").Format(X, Y, Z, W);
}

bool CQuaternion::FromString(Engine::Containers::CString str, CQuaternion& a)
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

CMatrix3 CQuaternion::ToMatrix3() const
{
	CMatrix3 mat;
	f32	wx, wy, wz;
	f32	xx, yy, yz;
	f32	xy, xz, zz;
	f32	x2, y2, z2;

	x2 = X + X;
	y2 = Y + Y;
	z2 = Z + Z;

	xx = X * x2;
	xy = X * y2;
	xz = X * z2;

	yy = Y * y2;
	yz = Y * z2;
	zz = Z * z2;

	wx = W * x2;
	wy = W * y2;
	wz = W * z2;

	mat[0][0] = 1.0f - ( yy + zz );
	mat[0][1] = xy - wz;
	mat[0][2] = xz + wy;

	mat[1][0] = xy + wz;
	mat[1][1] = 1.0f - ( xx + zz );
	mat[1][2] = yz - wx;

	mat[2][0] = xz - wy;
	mat[2][1] = yz + wx;
	mat[2][2] = 1.0f - ( xx + yy );

	return mat;
}

CMatrix4 CQuaternion::ToMatrix4() const
{
	return ToMatrix3().ToMatrix4();
}
		