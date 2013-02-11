///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CMatrix2.h"
#include "CVector2.h"
#include "CArray.h"

using namespace Engine::Math;

CMatrix2::CMatrix2()
{
	_mat[0][0] = 0; _mat[0][1] = 0;
	_mat[1][0] = 0; _mat[1][1] = 0;
}

CMatrix2::CMatrix2(const CVector2& x, const CVector2& y)
{
	_mat[0][0] = x[0]; _mat[0][1] = x[1];
	_mat[1][0] = y[0]; _mat[1][1] = y[1];
}

CMatrix2::CMatrix2(const f32 xx, const f32 xy, const f32 yx, const f32 yy)
{
	_mat[0][0] = xx; _mat[0][1] = xy;
	_mat[1][0] = yx; _mat[1][1] = yy;
}

CMatrix2::CMatrix2(const f32 src[2][2])
{
	_mat[0][0] = src[0][0]; _mat[0][1] = src[0][1];
	_mat[1][0] = src[1][0]; _mat[1][1] = src[1][1];
}

const CVector2&	CMatrix2::operator[](u32 index) const
{
	return _mat[index];
}

const CVector2&	CMatrix2::operator[](s32 index) const
{
	return _mat[index];
}

CVector2& CMatrix2::operator[](u32 index)
{
	return _mat[index];
}

CVector2& CMatrix2::operator[](s32 index)
{
	return _mat[index];
}

CMatrix2 CMatrix2::operator-() const
{
	return CMatrix2(-_mat[0][0], -_mat[0][1],
					-_mat[1][0], -_mat[1][1]);
}

CMatrix2 CMatrix2::operator*(const f32 a) const
{
	return CMatrix2(_mat[0].X * a, _mat[0].Y * a,
					_mat[1].X * a, _mat[1].Y * a);
}

CMatrix2 CMatrix2::operator/(const f32 a) const
{
	float inv = 1.0f / a;
	return CMatrix2(_mat[0].X * inv, _mat[0].Y * inv,
					_mat[1].X * inv, _mat[1].Y * inv);
}

CVector2 CMatrix2::operator*(const CVector2& a) const
{
	return CVector2(_mat[0][0] * a[0] + _mat[0][1] * a[1],
					_mat[1][0] * a[0] + _mat[1][1] * a[1]);
}

CMatrix2 CMatrix2::operator*(const CMatrix2& a) const
{
	return CMatrix2(_mat[0].X * a[0].X + _mat[0].Y * a[1].X,
					_mat[0].X * a[0].Y + _mat[0].Y * a[1].Y,
					_mat[1].X * a[0].X + _mat[1].Y * a[1].X,
					_mat[1].X * a[0].Y + _mat[1].Y * a[1].Y);
}

CMatrix2 CMatrix2::operator+(const CMatrix2& a) const
{
	return CMatrix2(_mat[0].X + a[0].X, _mat[0].Y + a[0].Y, 
					_mat[1].X + a[1].X, _mat[1].Y + a[1].Y);
}

CMatrix2 CMatrix2::operator-(const CMatrix2& a) const
{
	return CMatrix2(_mat[0].X - a[0].X, _mat[0].Y - a[0].Y, 
					_mat[1].X - a[1].X, _mat[1].Y - a[1].Y);
}

CMatrix2& CMatrix2::operator/=(const f32 a)
{
	float inv = 1.0f / a;
	_mat[0].X *= inv; _mat[0].Y *= inv;
	_mat[1].X *= inv; _mat[1].Y *= inv;
	return *this;
}

CMatrix2& CMatrix2::operator*=(const f32 a)
{
	_mat[0].X *= a; _mat[0].Y *= a;
	_mat[1].X *= a; _mat[1].Y *= a;

	return *this;
}

CMatrix2& CMatrix2::operator*=(const CMatrix2& a)
{
	f32 x, y;

	x = _mat[0].X; y = _mat[0].Y;
	_mat[0].X = x * a[0].X + y * a[1].X;
	_mat[0].Y = x * a[0].Y + y * a[1].Y;

	x = _mat[1].X; y = _mat[1].Y;
	_mat[1].X = x * a[0].X + y * a[1].X;
	_mat[1].Y = x * a[0].Y + y * a[1].Y;

	return *this;
}

CMatrix2& CMatrix2::operator+=(const CMatrix2& a) 
{
	_mat[0].X += a[0].X; _mat[0].Y += a[0].Y;
	_mat[1].X += a[1].X; _mat[1].Y += a[1].Y;

	return *this;
}

CMatrix2& CMatrix2::operator-=(const CMatrix2& a)
{
	_mat[0].X -= a[0].X; _mat[0].Y -= a[0].Y;
	_mat[1].X -= a[1].X; _mat[1].Y -= a[1].Y;

	return *this;
}

CMatrix2 Engine::Math::operator*(const f32 a, const CMatrix2& mat)
{
	return mat * a;
}

CVector2 Engine::Math::operator*(const CVector2& a, const CMatrix2& mat)
{
	return mat * a;
}

CVector2& Engine::Math::operator*=(CVector2& a, const CMatrix2& mat)
{
	a = mat * a;
	return a;
}

bool CMatrix2::Compare(const CMatrix2& a) const
{
	return _mat[0].Compare(a[0]) && _mat[1].Compare(a[1]);
}

bool CMatrix2::Compare(const CMatrix2& a, const f32 epsilon) const
{
	return _mat[0].Compare(a[0], epsilon) && _mat[1].Compare(a[1], epsilon);
}

bool CMatrix2::operator==(const CMatrix2& a) const
{
	return Compare(a);
}

bool CMatrix2::operator!=(const CMatrix2& a) const
{
	return !Compare(a);
}

void CMatrix2::Zero()
{
	_mat[0] = CVector2(0, 0);
	_mat[1] = CVector2(0, 0);
}

void CMatrix2::Identify()
{
	_mat[0] = CVector2(0, 0);
	_mat[1] = CVector2(0, 1);
}

bool CMatrix2::IsIdentify(const f32 epsilon) const
{
	return GetIdentity().Compare(*this, epsilon);
}

bool CMatrix2::IsSymmetric(const f32 epsilon) const
{
	return Abs(_mat[0][1] - _mat[1][0]) < epsilon;
}

bool CMatrix2::IsDiagonal(const f32 epsilon) const
{
	return (Abs(_mat[0][1]) <= epsilon && 
			Abs(_mat[1][0]) <= epsilon);
}

f32	CMatrix2::Trace() const
{
	return (_mat[0][0] + _mat[1][1]);
}

CMatrix2 CMatrix2::Normalize() const
{
	CMatrix2 output;
	f32 determinant = Determinant();

	for (u32 i = 0; i < 2; i++) 
		for (u32 j = 0; j < 2; j++ ) 
			output[i][j] = _mat[i][j] / determinant;

	return output;
}

f32	CMatrix2::Determinant() const
{
	return _mat[0][0] * _mat[1][1] - _mat[0][1] * _mat[1][0];
}

CMatrix2 CMatrix2::Inverse() const
{
	f32 det, invDet, a;

	det = _mat[0][0] * _mat[1][1] - _mat[0][1] * _mat[1][0];

	if (Abs((f32)det) < Engine::Math::EPSILON)
		return *this;

	invDet = 1.0f / det;

	CMatrix2 inverse;

	a = _mat[0][0];
	inverse[0][0] =   _mat[1][1] * invDet;
	inverse[0][1] = - _mat[0][1] * invDet;
	inverse[1][0] = - _mat[1][0] * invDet;
	inverse[1][1] =   a * invDet;

	return inverse;
}

CMatrix2 CMatrix2::Transpose() const
{
	return CMatrix2(_mat[0][0], _mat[1][0],
				    _mat[0][1], _mat[1][1]);
}

Engine::Containers::CString	CMatrix2::ToString()
{
	return S("[(") + _mat[0][0] + "," + _mat[0][1] + "),(" + _mat[1][0] + "," + _mat[1][1] + ")]";
}

bool CMatrix2::FromString(Engine::Containers::CString str, CMatrix2& a)
{
	if (str.Length() <= 0 ||								// Needs a string :3
		str[0] != '[' || str[str.Length() - 1] != ']' ||	// Needs the braces around it.
		str.Count(',') != 3)								// Needs exactly 1 seperating comma.
		return false;

	str = str.Trim("[] ");

	Engine::Containers::CArray<Engine::Containers::CString> split = str.Split(',');
	a._mat[0][0] = split[0].Trim("() ").ToFloat();
	a._mat[0][1] = split[1].Trim("() ").ToFloat();
	a._mat[1][0] = split[2].Trim("() ").ToFloat();
	a._mat[1][1] = split[3].Trim("() ").ToFloat();

	return true;
}
		
CMatrix2 CMatrix2::GetIdentity()
{
	return CMatrix2(CVector2(1, 0),
					CVector2(0, 1));
}

CMatrix2 CMatrix2::GetZero()
{
	return CMatrix2(CVector2(0, 0),
					CVector2(0, 0));
}
