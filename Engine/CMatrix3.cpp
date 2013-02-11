///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CMatrix3.h"
#include "CVector3.h"
#include "CArray.h"
#include "CMatrix4.h"
#include "CQuaternion.h"

using namespace Engine::Math;

CMatrix3::CMatrix3()
{
	_mat[0][0] = 0; _mat[0][1] = 0; _mat[0][2] = 0;
	_mat[1][0] = 0; _mat[1][1] = 0; _mat[1][2] = 0;
	_mat[2][0] = 0; _mat[2][1] = 0; _mat[2][2] = 0;
}

CMatrix3::CMatrix3(const CVector3& x, const CVector3& y, const CVector3& z)
{
	_mat[0] = x;
	_mat[1] = y;
	_mat[2] = z;
}

CMatrix3::CMatrix3(const f32 xx, const f32 xy, const f32 xz, const f32 yx, const f32 yy, const f32 yz, const f32 zx, const f32 zy, const f32 zz)
{
	_mat[0][0] = xx; _mat[0][1] = xy; _mat[0][2] = xz;
	_mat[1][0] = yx; _mat[1][1] = yy; _mat[1][2] = yz;
	_mat[2][0] = zx; _mat[2][1] = zy; _mat[2][2] = zz;
}

CMatrix3::CMatrix3(const f32 src[3][3])
{
	_mat[0][0] = src[0][0]; _mat[0][1] = src[0][1]; _mat[0][2] = src[0][2];
	_mat[1][0] = src[1][0]; _mat[1][1] = src[1][1]; _mat[1][2] = src[1][2];
	_mat[2][0] = src[2][0]; _mat[2][1] = src[2][1]; _mat[2][2] = src[2][2];
}

const CVector3& CMatrix3::operator[](u32 index) const
{
	return _mat[index];
}

const CVector3& CMatrix3::operator[](s32 index) const
{
	return _mat[index];
}

CVector3& CMatrix3::operator[](u32 index)
{
	return _mat[index];
}

CVector3& CMatrix3::operator[](s32 index)
{
	return _mat[index];
}

CMatrix3 CMatrix3::operator-() const
{
	return CMatrix3(-_mat[0][0], -_mat[0][1], -_mat[0][2],
					-_mat[1][0], -_mat[1][1], -_mat[1][2],
					-_mat[2][0], -_mat[2][1], -_mat[2][2]);
}

CMatrix3 CMatrix3::operator*(const f32 a) const
{
	return CMatrix3(_mat[0].X * a, _mat[0].Y * a, _mat[0].Z * a,
					_mat[1].X * a, _mat[1].Y * a, _mat[1].Z * a,
					_mat[2].X * a, _mat[2].Y * a, _mat[2].Z * a);
}

CMatrix3 CMatrix3::operator/(const f32 a) const
{
	float inv = 1.0f / a;
	return CMatrix3(_mat[0].X * inv, _mat[0].Y * inv, _mat[0].Z * inv,
					_mat[1].X * inv, _mat[1].Y * inv, _mat[1].Z * inv,
					_mat[2].X * inv, _mat[2].Y * inv, _mat[2].Z * inv);
}

CVector3 CMatrix3::operator*(const CVector3& a) const
{
	return CVector3(_mat[0].X * a.X + _mat[1].X * a.Y + _mat[2].X * a.Z,
					_mat[0].Y * a.X + _mat[1].Y * a.Y + _mat[2].Y * a.Z,
					_mat[0].Z * a.X + _mat[1].Z * a.Y + _mat[2].Z * a.Z);
}

CMatrix3 CMatrix3::operator*(const CMatrix3& a) const
{
	CMatrix3 dst;

	for (u32 column = 0; column < 3; column++)
	{
		for (u32 row = 0; row < 3; row++)
		{
			dst[column][row] = (_mat[column][0] * a[0][row]) +
							   (_mat[column][1] * a[1][row]) +
							   (_mat[column][2] * a[2][row]);
		}
	}

	return dst;
}

CMatrix3 CMatrix3::operator+(const CMatrix3& a) const
{
	return CMatrix3(_mat[0].X + a._mat[0].X, _mat[0].Y + a._mat[0].Y, _mat[0].Z + a._mat[0].Z,
					_mat[1].X + a._mat[1].X, _mat[1].Y + a._mat[1].Y, _mat[1].Z + a._mat[1].Z,
					_mat[2].X + a._mat[2].X, _mat[2].Y + a._mat[2].Y, _mat[2].Z + a._mat[2].Z);
}

CMatrix3 CMatrix3::operator-(const CMatrix3& a) const
{
	return CMatrix3(_mat[0].X - a._mat[0].X, _mat[0].Y - a._mat[0].Y, _mat[0].Z - a._mat[0].Z,
					_mat[1].X - a._mat[1].X, _mat[1].Y - a._mat[1].Y, _mat[1].Z - a._mat[1].Z,
					_mat[2].X - a._mat[2].X, _mat[2].Y - a._mat[2].Y, _mat[2].Z - a._mat[2].Z);
}

CMatrix3& CMatrix3::operator/=(const f32 a)
{
	float inv = 1.0f / a;
	_mat[0].X *= inv; _mat[0].Y *= inv; _mat[0].Z *= inv;
	_mat[1].X *= inv; _mat[1].Y *= inv; _mat[1].Z *= inv; 
	_mat[2].X *= inv; _mat[2].Y *= inv; _mat[2].Z *= inv;
	return *this;
}

CMatrix3& CMatrix3::operator*=(const f32 a)
{
	_mat[0].X *= a; _mat[0].Y *= a; _mat[0].Z *= a;
	_mat[1].X *= a; _mat[1].Y *= a; _mat[1].Z *= a; 
	_mat[2].X *= a; _mat[2].Y *= a; _mat[2].Z *= a;
	return *this;
}

CMatrix3& CMatrix3::operator*=(const CMatrix3& a)
{
	CMatrix3 dst;

	for (u32 column = 0; column < 3; column++)
	{
		for (u32 row = 0; row < 3; row++)
		{
			dst[column][row] = (_mat[column][0] * a[0][row]) +
							   (_mat[column][1] * a[1][row]) +
							   (_mat[column][2] * a[2][row]);
		}
	}

	*this = dst;
	return *this;
}

CMatrix3& CMatrix3::operator+=(const CMatrix3& a)
{
	_mat[0].X += a[0].X; _mat[0].Y += a[0].Y; _mat[0].Z += a[0].Z;
	_mat[1].X += a[1].X; _mat[1].Y += a[1].Y; _mat[1].Z += a[1].Z; 
	_mat[2].X += a[2].X; _mat[2].Y += a[2].Y; _mat[2].Z += a[2].Z;
	return *this;
}

CMatrix3& CMatrix3::operator-=(const CMatrix3& a)
{
	_mat[0].X -= a[0].X; _mat[0].Y -= a[0].Y; _mat[0].Z -= a[0].Z;
	_mat[1].X -= a[1].X; _mat[1].Y -= a[1].Y; _mat[1].Z -= a[1].Z; 
	_mat[2].X -= a[2].X; _mat[2].Y -= a[2].Y; _mat[2].Z -= a[2].Z;
	return *this;
}

CMatrix3 Engine::Math::operator*(const f32 a, const CMatrix3& mat)
{
	return mat * a;
}

CVector3 Engine::Math::operator*(const CVector3& a, const CMatrix3& mat)
{
	return mat * a;
}

CVector3& Engine::Math::operator*=(CVector3& a, const CMatrix3& mat)
{
	float x = mat[0].X * a.X + mat[1].X * a.Y + mat[2].X * a.Z;
	float y = mat[0].Y * a.X + mat[1].Y * a.Y + mat[2].Y * a.Z;
	a.Z =     mat[0].Z * a.X + mat[1].Z * a.Y + mat[2].Z * a.Z;
	a.X = x;
	a.Y = y;
	return a;
}

bool CMatrix3::Compare(const CMatrix3& a) const
{
	return _mat[0].Compare(a[0]) && _mat[1].Compare(a[1]) && _mat[2].Compare(a[2]);
}

bool CMatrix3::Compare(const CMatrix3& a, const f32 epsilon) const
{
	return _mat[0].Compare(a[0], epsilon) && _mat[1].Compare(a[1], epsilon) && _mat[2].Compare(a[2], epsilon);
}

bool CMatrix3::operator==(const CMatrix3& a) const
{
	return Compare(a);
}

bool CMatrix3::operator!=(const CMatrix3& a) const
{
	return !Compare(a);
}

void CMatrix3::Zero()
{
	_mat[0] = CVector3(0, 0, 0);
	_mat[1] = CVector3(0, 0, 0);
	_mat[2] = CVector3(0, 0, 0);
}

void CMatrix3::Identify()
{
	_mat[0] = CVector3(1, 0, 0);
	_mat[1] = CVector3(0, 1, 0);
	_mat[2] = CVector3(0, 0, 1);
}

bool CMatrix3::IsIdentify(const f32 epsilon) const
{
	return GetIdentity().Compare(*this, epsilon);
}

bool CMatrix3::IsSymmetric(const f32 epsilon) const
{
	return Abs(_mat[0][1] - _mat[1][0]) < epsilon &&
		   Abs(_mat[0][2] - _mat[2][0]) < epsilon &&
		   Abs(_mat[1][2] - _mat[2][1]) < epsilon;
}

bool CMatrix3::IsDiagonal(const f32 epsilon) const
{
	return (Abs(_mat[0][1]) <= epsilon && 
			Abs(_mat[0][2]) <= epsilon && 
			Abs(_mat[1][0]) <= epsilon && 
			Abs(_mat[1][2]) <= epsilon && 
			Abs(_mat[2][0]) <= epsilon && 
			Abs(_mat[2][1]) <= epsilon);
}

CMatrix3 CMatrix3::OrthoNormalize() const
{
	CMatrix3 v = *this;
	v[0].Normalize();
	v[2] = _mat[0].Cross(_mat[1]);
	v[2].Normalize();
	v[1] = _mat[2].Cross(_mat[0]);
	v[1].Normalize();
	return v;
}
			
CMatrix3 CMatrix3::Normalize() const
{
	CMatrix3 output;
	f32 determinant = Determinant();

	for (u32 i = 0; i < 3; i++) 
		for (u32 j = 0; j < 3; j++ ) 
			output[i][j] = _mat[i][j] / determinant;

	return output;
}

CVector3 CMatrix3::ProjectVector(const CVector3& src) const
{
	CVector3 dst;
	dst.X = src * _mat[0];
	dst.Y = src * _mat[1];
	dst.Z = src * _mat[2];
	return dst;
}

CVector3 CMatrix3::UnprojectVector(const CVector3& src) const
{
	CVector3 dst;
	dst = _mat[0] * src.X + _mat[1] * src.Y + _mat[2] * src.Z;
	return dst;
}

f32	CMatrix3::Trace() const
{
	return (_mat[0][0] + _mat[1][1] + _mat[2][2]);
}

f32	CMatrix3::Determinant() const
{
	f32 det2_12_01 = _mat[1][0] * _mat[2][1] - _mat[1][1] * _mat[2][0];
	f32 det2_12_02 = _mat[1][0] * _mat[2][2] - _mat[1][2] * _mat[2][0];
	f32 det2_12_12 = _mat[1][1] * _mat[2][2] - _mat[1][2] * _mat[2][1];
	return _mat[0][0] * det2_12_12 - _mat[0][1] * det2_12_02 + _mat[0][2] * det2_12_01;
}

CMatrix3 CMatrix3::Inverse() const
{
	CMatrix3 inverse;
	f32 det, invDet;

	inverse[0][0] = _mat[1][1] * _mat[2][2] - _mat[1][2] * _mat[2][1];
	inverse[1][0] = _mat[1][2] * _mat[2][0] - _mat[1][0] * _mat[2][2];
	inverse[2][0] = _mat[1][0] * _mat[2][1] - _mat[1][1] * _mat[2][0];

	det = _mat[0][0] * inverse[0][0] + _mat[0][1] * inverse[1][0] + _mat[0][2] * inverse[2][0];

	if (Abs( det ) < Engine::Math::EPSILON) 
	{
		return *this;
	}

	invDet = 1.0f / det;

	inverse[0][1] = _mat[0][2] * _mat[2][1] - _mat[0][1] * _mat[2][2];
	inverse[0][2] = _mat[0][1] * _mat[1][2] - _mat[0][2] * _mat[1][1];
	inverse[1][1] = _mat[0][0] * _mat[2][2] - _mat[0][2] * _mat[2][0];
	inverse[1][2] = _mat[0][2] * _mat[1][0] - _mat[0][0] * _mat[1][2];
	inverse[2][1] = _mat[0][1] * _mat[2][0] - _mat[0][0] * _mat[2][1];
	inverse[2][2] = _mat[0][0] * _mat[1][1] - _mat[0][1] * _mat[1][0];

	CMatrix3 final;
	final[0][0] = inverse[0][0] * invDet;
	final[0][1] = inverse[0][1] * invDet;
	final[0][2] = inverse[0][2] * invDet;
	final[1][0] = inverse[1][0] * invDet;
	final[1][1] = inverse[1][1] * invDet;
	final[1][2] = inverse[1][2] * invDet;
	final[2][0] = inverse[2][0] * invDet;
	final[2][1] = inverse[2][1] * invDet;
	final[2][2] = inverse[2][2] * invDet;

	return final;
}

CMatrix3 CMatrix3::Transpose() const
{
	return CMatrix3(_mat[0][0], _mat[1][0], _mat[2][0],
					_mat[0][1], _mat[1][1], _mat[2][1],
					_mat[0][2], _mat[1][2], _mat[2][2]);
}

Engine::Containers::CString	CMatrix3::ToString()
{
	return S("[(") + _mat[0][0] + "," + _mat[0][1] + "," + _mat[0][2] + "),(" + _mat[1][0] + "," + _mat[1][1] + "," + _mat[1][2] + "),(" + _mat[2][0] + "," + _mat[2][1] + "," + _mat[2][2] + ")]";
}

bool CMatrix3::FromString(Engine::Containers::CString str, CMatrix3& a)
{
	if (str.Length() <= 0 ||								// Needs a string :3
		str[0] != '[' || str[str.Length() - 1] != ']' ||	// Needs the braces around it.
		str.Count(',') != 8)								// Needs exactly 1 seperating comma.
		return false;

	str = str.Trim("[] ");

	Engine::Containers::CArray<Engine::Containers::CString> split = str.Split(',');
	a._mat[0][0] = split[0].Trim("() ").ToFloat();
	a._mat[0][1] = split[1].Trim("() ").ToFloat();
	a._mat[0][2] = split[2].Trim("() ").ToFloat();
	
	a._mat[1][0] = split[3].Trim("() ").ToFloat();
	a._mat[1][1] = split[4].Trim("() ").ToFloat();
	a._mat[1][2] = split[5].Trim("() ").ToFloat();
	
	a._mat[2][0] = split[6].Trim("() ").ToFloat();
	a._mat[2][1] = split[7].Trim("() ").ToFloat();
	a._mat[2][2] = split[8].Trim("() ").ToFloat();

	return true;
}
		
CMatrix3 CMatrix3::GetIdentity()
{
	return CMatrix3(CVector3(1, 0, 0),
					CVector3(0, 1, 0),
					CVector3(0, 0, 1));
}

CMatrix3 CMatrix3::GetZero()
{
	return CMatrix3(CVector3(0, 0, 0),
					CVector3(0, 0, 0),
					CVector3(0, 0, 0));
}

CQuaternion	CMatrix3::ToQuaternion() const
{
	CQuaternion q;
	f32 trace, s, t;
	s32 i, j, k;

	static s32 next[3] = { 1, 2, 0 };

	trace = _mat[0][0] + _mat[1][1] + _mat[2][2];
	if (trace > 0.0f)
	{
		t = trace + 1.0f;
		s = Math::InvSqrt(t) * 0.5f;

		q[3] = s * t;
		q[0] = (_mat[2][1] - _mat[1][2]) * s;
		q[1] = (_mat[0][2] - _mat[2][0]) * s;
		q[2] = (_mat[1][0] - _mat[0][1]) * s;
	}
	else
	{
		i = 0;
		if (_mat[1][1] > _mat[0][0])
		{
			i = 1;
		}
		if (_mat[2][2] > _mat[i][i])
		{
			i = 2;
		}
		j = next[i];
		k = next[j];

		t = (_mat[i][i] - (_mat[j][j] + _mat[k][k])) + 1.0f;
		s = Math::InvSqrt(t) * 0.5f;

		q[i] = s * t;
		q[3] = (_mat[k][j] - _mat[j][k]) * s;
		q[j] = (_mat[j][i] + _mat[i][j]) * s;
		q[k] = (_mat[k][i] + _mat[i][k]) * s;
	}

	return q;
}

CMatrix4 CMatrix3::ToMatrix4() const
{
	return CMatrix4(_mat[0][0], _mat[1][0], _mat[2][0], 0.0f,
					_mat[0][1], _mat[1][1], _mat[2][1], 0.0f,
					_mat[0][2], _mat[1][2], _mat[2][2], 0.0f,
					0.f,	    0.f,		0.f,		1.0f);
}