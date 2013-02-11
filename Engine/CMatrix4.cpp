///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CMatrix4.h"
#include "CVector4.h"
#include "CVector3.h"
#include "CArray.h"

using namespace Engine::Math;

CMatrix4::CMatrix4()
{
	_mat[0][0] = 0; _mat[0][1] = 0; _mat[0][2] = 0; _mat[0][3] = 0;
	_mat[1][0] = 0; _mat[1][1] = 0; _mat[1][2] = 0; _mat[1][3] = 0;
	_mat[2][0] = 0; _mat[2][1] = 0; _mat[2][2] = 0; _mat[2][3] = 0;
	_mat[3][0] = 0; _mat[3][1] = 0; _mat[3][2] = 0; _mat[3][3] = 0;
}

CMatrix4::CMatrix4(const CVector4& x, const CVector4& y, const CVector4& z, const CVector4& w)
{
	_mat[0] = x;
	_mat[1] = y;
	_mat[2] = z;
	_mat[3] = w;
}

CMatrix4::CMatrix4(const f32 xx, const f32 xy, const f32 xz, const f32 xw,
				   const f32 yx, const f32 yy, const f32 yz, const f32 yw, 
				   const f32 zx, const f32 zy, const f32 zz, const f32 zw,
				   const f32 wx, const f32 wy, const f32 wz, const f32 ww)
{
	_mat[0].X = xx; _mat[0].Y = xy; _mat[0].Z = xz; _mat[0].W = xw;
	_mat[1].X = yx; _mat[1].Y = yy; _mat[1].Z = yz; _mat[1].W = yw;
	_mat[2].X = zx; _mat[2].Y = zy; _mat[2].Z = zz; _mat[2].W = zw;
	_mat[3].X = wx; _mat[3].Y = wy; _mat[3].Z = wz; _mat[3].W = ww;
}

CMatrix4::CMatrix4(const f32 src[4][4])
{
	_mat[0].X = src[0][0]; _mat[0].Y = src[0][1]; _mat[0].Z = src[0][2]; _mat[0].W = src[0][3];
	_mat[1].X = src[1][0]; _mat[1].Y = src[1][1]; _mat[1].Z = src[1][2]; _mat[1].W = src[1][3];
	_mat[2].X = src[2][0]; _mat[2].Y = src[2][1]; _mat[2].Z = src[2][2]; _mat[2].W = src[2][3];
	_mat[3].X = src[3][0]; _mat[3].Y = src[3][1]; _mat[3].Z = src[3][2]; _mat[3].W = src[3][3];
}

const CVector4& CMatrix4::operator[](u32 index) const
{
	return _mat[index];
}

const CVector4& CMatrix4::operator[](s32 index) const
{
	return _mat[index];
}

CVector4& CMatrix4::operator[](u32 index)
{
	return _mat[index];
}

CVector4& CMatrix4::operator[](s32 index)
{
	return _mat[index];
}

CMatrix4 CMatrix4::operator-() const
{
	return CMatrix4(-_mat[0][0], -_mat[0][1], -_mat[0][2], -_mat[0][3],
					-_mat[1][0], -_mat[1][1], -_mat[1][2], -_mat[1][3],
					-_mat[2][0], -_mat[2][1], -_mat[2][2], -_mat[2][3],
					-_mat[3][0], -_mat[3][1], -_mat[3][2], -_mat[3][3]);
}

CMatrix4 CMatrix4::operator*(const f32 a) const
{
	return CMatrix4(_mat[0][0] * a, _mat[0][1] * a, _mat[0][2] * a, _mat[0][3] * a,
					_mat[1][0] * a, _mat[1][1] * a, _mat[1][2] * a, _mat[1][3] * a,
					_mat[2][0] * a, _mat[2][1] * a, _mat[2][2] * a, _mat[2][3] * a,
					_mat[3][0] * a, _mat[3][1] * a, _mat[3][2] * a, _mat[3][3] * a);
}

CMatrix4 CMatrix4::operator/(const f32 a) const
{
	f32 inv = 1.0f / a;
	return CMatrix4(_mat[0][0] * inv, _mat[0][1] * inv, _mat[0][2] * inv, _mat[0][3] * inv,
					_mat[1][0] * inv, _mat[1][1] * inv, _mat[1][2] * inv, _mat[1][3] * inv,
					_mat[2][0] * inv, _mat[2][1] * inv, _mat[2][2] * inv, _mat[2][3] * inv,
					_mat[3][0] * inv, _mat[3][1] * inv, _mat[3][2] * inv, _mat[3][3] * inv);
}

CVector4 CMatrix4::operator*(const CVector4& a) const
{
	return CVector4(_mat[0].X * a.X + _mat[1].X * a.Y + _mat[2].X * a.Z + _mat[3].X * a.W,
					_mat[0].Y * a.X + _mat[1].Y * a.Y + _mat[2].Y * a.Z + _mat[3].Y * a.W,
					_mat[0].Z * a.X + _mat[1].Z * a.Y + _mat[2].Z * a.Z + _mat[3].Z * a.W,
					_mat[0].W * a.X + _mat[1].W * a.Y + _mat[2].W * a.Z + _mat[3].W * a.W);
}

CVector3 CMatrix4::operator*(const CVector3& a) const
{
	f32 s = _mat[3].X * a.X + _mat[3].Y * a.Y + _mat[3].Z * a.Z + _mat[3].W;
	if (s == 0.0f) 
	{
		return CVector3(0.0f, 0.0f, 0.0f);
	}
	if ( s == 1.0f ) 
	{
		return CVector3(_mat[0].X * a.X + _mat[0].Y * a.Y + _mat[0].Z * a.Z + _mat[0].W,
						_mat[1].X * a.X + _mat[1].Y * a.Y + _mat[1].Z * a.Z + _mat[1].W,
						_mat[2].X * a.X + _mat[2].Y * a.Y + _mat[2].Z * a.Z + _mat[2].W);
	}
	else 
	{
		f32 invS = 1.0f / s;
		return CVector3((_mat[ 0 ].X * a.X + _mat[0].Y * a.Y + _mat[0].Z * a.Z + _mat[0].W) * invS,
					    (_mat[ 1 ].X * a.X + _mat[1].Y * a.Y + _mat[1].Z * a.Z + _mat[1].W) * invS,
					    (_mat[ 2 ].X * a.X + _mat[2].Y * a.Y + _mat[2].Z * a.Z + _mat[2].W) * invS);
	}
}

CMatrix4 CMatrix4::operator*(const CMatrix4& a) const
{
	CMatrix4 dst;

	for (u32 column = 0; column < 4; column++)
	{
		for (u32 row = 0; row < 4; row++)
		{
			dst[column][row] = (_mat[column][0] * a[0][row]) +
							   (_mat[column][1] * a[1][row]) +
							   (_mat[column][2] * a[2][row]) +
							   (_mat[column][3] * a[3][row]);
		}
	}

	return dst;
}

CMatrix4 CMatrix4::operator+(const CMatrix4& a) const
{
	return CMatrix4(_mat[0].X + a._mat[0].X, _mat[0].Y + a._mat[0].Y, _mat[0].Z + a._mat[0].Z, _mat[0].W + a._mat[0].W,
					_mat[1].X + a._mat[1].X, _mat[1].Y + a._mat[1].Y, _mat[1].Z + a._mat[1].Z, _mat[0].W + a._mat[1].W,
					_mat[2].X + a._mat[2].X, _mat[2].Y + a._mat[2].Y, _mat[2].Z + a._mat[2].Z, _mat[2].W + a._mat[2].W,
					_mat[3].X + a._mat[3].X, _mat[3].Y + a._mat[3].Y, _mat[3].Z + a._mat[3].Z, _mat[3].W + a._mat[3].W);
}

CMatrix4 CMatrix4::operator-(const CMatrix4& a) const
{
	return CMatrix4(_mat[0].X - a._mat[0].X, _mat[0].Y - a._mat[0].Y, _mat[0].Z - a._mat[0].Z, _mat[0].W - a._mat[0].W,
					_mat[1].X - a._mat[1].X, _mat[1].Y - a._mat[1].Y, _mat[1].Z - a._mat[1].Z, _mat[0].W - a._mat[1].W,
					_mat[2].X - a._mat[2].X, _mat[2].Y - a._mat[2].Y, _mat[2].Z - a._mat[2].Z, _mat[2].W - a._mat[2].W,
					_mat[3].X - a._mat[3].X, _mat[3].Y - a._mat[3].Y, _mat[3].Z - a._mat[3].Z, _mat[3].W - a._mat[3].W);
}

CMatrix4& CMatrix4::operator/=(const f32 a)
{
	float inv = 1.0f / a;
	_mat[0].X *= inv; _mat[0].Y *= inv; _mat[0].Z *= inv; _mat[0].W *= inv;
	_mat[1].X *= inv; _mat[1].Y *= inv; _mat[1].Z *= inv; _mat[1].W *= inv; 
	_mat[2].X *= inv; _mat[2].Y *= inv; _mat[2].Z *= inv; _mat[2].W *= inv;
	_mat[3].X *= inv; _mat[3].Y *= inv; _mat[3].Z *= inv; _mat[3].W *= inv;
	return *this;
}

CMatrix4& CMatrix4::operator*=(const f32 a)
{
	_mat[0].X *= a; _mat[0].Y *= a; _mat[0].Z *= a; _mat[0].W *= a;
	_mat[1].X *= a; _mat[1].Y *= a; _mat[1].Z *= a; _mat[1].W *= a; 
	_mat[2].X *= a; _mat[2].Y *= a; _mat[2].Z *= a; _mat[2].W *= a;
	_mat[3].X *= a; _mat[3].Y *= a; _mat[3].Z *= a; _mat[3].W *= a;
	return *this;
}

CMatrix4& CMatrix4::operator*=(const CMatrix4& a)
{
	CMatrix4 dst;

	for (u32 column = 0; column < 4; column++)
	{
		for (u32 row = 0; row < 4; row++)
		{
			dst[column][row] = (_mat[column][0] * a[0][row]) +
							   (_mat[column][1] * a[1][row]) +
							   (_mat[column][2] * a[2][row]) +
							   (_mat[column][3] * a[3][row]);
		}
	}

	*this = dst;
	return *this;
}

CMatrix4& CMatrix4::operator+=(const CMatrix4& a)
{
	_mat[0].X += a[0].X; _mat[0].Y += a[0].Y; _mat[0].Z += a[0].Z; _mat[0].W += a[0].W;
	_mat[1].X += a[1].X; _mat[1].Y += a[1].Y; _mat[1].Z += a[1].Z; _mat[1].W += a[1].W; 
	_mat[2].X += a[2].X; _mat[2].Y += a[2].Y; _mat[2].Z += a[2].Z; _mat[2].W += a[2].W;
	_mat[3].X += a[3].X; _mat[3].Y += a[3].Y; _mat[3].Z += a[3].Z; _mat[3].W += a[3].W;
	return *this;
}

CMatrix4& CMatrix4::operator-=(const CMatrix4& a)
{
	_mat[0].X -= a[0].X; _mat[0].Y -= a[0].Y; _mat[0].Z -= a[0].Z; _mat[0].W -= a[0].W;
	_mat[1].X -= a[1].X; _mat[1].Y -= a[1].Y; _mat[1].Z -= a[1].Z; _mat[1].W -= a[1].W; 
	_mat[2].X -= a[2].X; _mat[2].Y -= a[2].Y; _mat[2].Z -= a[2].Z; _mat[2].W -= a[2].W;
	_mat[3].X -= a[3].X; _mat[3].Y -= a[3].Y; _mat[3].Z -= a[3].Z; _mat[3].W -= a[3].W;
	return *this;
}

CMatrix4 Engine::Math::operator*(const f32 a, const CMatrix4& mat)
{
	return mat * a;
}

CVector4 Engine::Math::operator*(const CVector4& a, const CMatrix4& mat)
{
	return mat * a;
}

CVector4& Engine::Math::operator*=(CVector4& a, const CMatrix4& mat)
{
	a = mat * a;
	return a;
}

CVector3& Engine::Math::operator*=(CVector3& a, const CMatrix4& mat)
{
	a = mat * a;
	return a;
}

bool CMatrix4::Compare(const CMatrix4& a) const
{
	return _mat[0].Compare(a[0]) && _mat[1].Compare(a[1]) && _mat[2].Compare(a[2]) && _mat[3].Compare(a[3]);
}

bool CMatrix4::Compare(const CMatrix4& a, const f32 epsilon) const
{
	return _mat[0].Compare(a[0], epsilon) && _mat[1].Compare(a[1], epsilon) && _mat[2].Compare(a[2], epsilon) && _mat[3].Compare(a[3], epsilon);
}

bool CMatrix4::operator==(const CMatrix4& a) const
{
	return Compare(a);
}

bool CMatrix4::operator!=(const CMatrix4& a) const
{
	return !Compare(a);
}

void CMatrix4::Zero()
{
	_mat[0] = CVector4(0, 0, 0, 0);
	_mat[1] = CVector4(0, 0, 0, 0);
	_mat[2] = CVector4(0, 0, 0, 0);
	_mat[3] = CVector4(0, 0, 0, 0);
}

void CMatrix4::Identify()
{
	_mat[0] = CVector4(1, 0, 0, 0);
	_mat[1] = CVector4(0, 1, 0, 0);
	_mat[2] = CVector4(0, 0, 1, 0);
	_mat[3] = CVector4(0, 0, 0, 1);
}

bool CMatrix4::IsIdentify(const f32 epsilon) const
{
	return GetIdentity().Compare(*this, epsilon);
}

bool CMatrix4::IsSymmetric(const f32 epsilon) const
{
	for (u32 x = 0; x < 4; x++)
		for (u32 y = 0; y < 4; y++)
			if (Abs(_mat[x][y] - _mat[y][x]) > epsilon)
				return false;
	return true;
}

bool CMatrix4::IsDiagonal(const f32 epsilon) const
{
	for (u32 x = 0; x < 4; x++)
		for (u32 y = 0; y < 4; y++)
			if (x != y && Abs(_mat[x][y]) > epsilon)
				return false;
	return true;
}

CMatrix4 CMatrix4::Normalize() const
{
	CMatrix4 output;
	f32 determinant = Determinant();

	for (u32 i = 0; i < 4; i++) 
		for (u32 j = 0; j < 4; j++ ) 
			output[i][j] = _mat[i][j] / determinant;

	return output;
}
				
CVector4 CMatrix4::ProjectVector(const CVector4& src) const
{
	CVector4 dst;
	dst.X = src * _mat[0];
	dst.Y = src * _mat[1];
	dst.Z = src * _mat[2];
	dst.W = src * _mat[3];
	return dst;
}

CVector4 CMatrix4::UnprojectVector(const CVector4& src) const
{
	CVector4 dst;
	dst = _mat[0] * src.X + _mat[1] * src.Y + _mat[2] * src.Z + _mat[3] * src.W;
	return dst;
}

f32	CMatrix4::Trace() const
{
	return (_mat[0][0] + _mat[1][1] + _mat[2][2] + _mat[3][3]);
}

f32	CMatrix4::Determinant() const
{
	// 2x2 sub-determinants
	f32 det2_01_01 = _mat[0][0] * _mat[1][1] - _mat[0][1] * _mat[1][0];
	f32 det2_01_02 = _mat[0][0] * _mat[1][2] - _mat[0][2] * _mat[1][0];
	f32 det2_01_03 = _mat[0][0] * _mat[1][3] - _mat[0][3] * _mat[1][0];
	f32 det2_01_12 = _mat[0][1] * _mat[1][2] - _mat[0][2] * _mat[1][1];
	f32 det2_01_13 = _mat[0][1] * _mat[1][3] - _mat[0][3] * _mat[1][1];
	f32 det2_01_23 = _mat[0][2] * _mat[1][3] - _mat[0][3] * _mat[1][2];

	// 3x3 sub-determinants
	f32 det3_201_012 = _mat[2][0] * det2_01_12 - _mat[2][1] * det2_01_02 + _mat[2][2] * det2_01_01;
	f32 det3_201_013 = _mat[2][0] * det2_01_13 - _mat[2][1] * det2_01_03 + _mat[2][3] * det2_01_01;
	f32 det3_201_023 = _mat[2][0] * det2_01_23 - _mat[2][2] * det2_01_03 + _mat[2][3] * det2_01_02;
	f32 det3_201_123 = _mat[2][1] * det2_01_23 - _mat[2][2] * det2_01_13 + _mat[2][3] * det2_01_12;

	return ( - det3_201_123 * _mat[3][0] + det3_201_023 * _mat[3][1] - det3_201_013 * _mat[3][2] + det3_201_012 * _mat[3][3] );

}

CMatrix4 CMatrix4::Inverse() const
{
	f32 det, invDet;

	f32 det2_01_01 = _mat[0][0] * _mat[1][1] - _mat[0][1] * _mat[1][0];
	f32 det2_01_02 = _mat[0][0] * _mat[1][2] - _mat[0][2] * _mat[1][0];
	f32 det2_01_03 = _mat[0][0] * _mat[1][3] - _mat[0][3] * _mat[1][0];
	f32 det2_01_12 = _mat[0][1] * _mat[1][2] - _mat[0][2] * _mat[1][1];
	f32 det2_01_13 = _mat[0][1] * _mat[1][3] - _mat[0][3] * _mat[1][1];
	f32 det2_01_23 = _mat[0][2] * _mat[1][3] - _mat[0][3] * _mat[1][2];

	f32 det3_201_012 = _mat[2][0] * det2_01_12 - _mat[2][1] * det2_01_02 + _mat[2][2] * det2_01_01;
	f32 det3_201_013 = _mat[2][0] * det2_01_13 - _mat[2][1] * det2_01_03 + _mat[2][3] * det2_01_01;
	f32 det3_201_023 = _mat[2][0] * det2_01_23 - _mat[2][2] * det2_01_03 + _mat[2][3] * det2_01_02;
	f32 det3_201_123 = _mat[2][1] * det2_01_23 - _mat[2][2] * det2_01_13 + _mat[2][3] * det2_01_12;

	det = ( - det3_201_123 * _mat[3][0] + det3_201_023 * _mat[3][1] - det3_201_013 * _mat[3][2] + det3_201_012 * _mat[3][3] );

	if (Abs( det ) < Engine::Math::EPSILON) 
	{
		return *this;
	}

	invDet = 1.0f / det;

	f32 det2_03_01 = _mat[0][0] * _mat[3][1] - _mat[0][1] * _mat[3][0];
	f32 det2_03_02 = _mat[0][0] * _mat[3][2] - _mat[0][2] * _mat[3][0];
	f32 det2_03_03 = _mat[0][0] * _mat[3][3] - _mat[0][3] * _mat[3][0];
	f32 det2_03_12 = _mat[0][1] * _mat[3][2] - _mat[0][2] * _mat[3][1];
	f32 det2_03_13 = _mat[0][1] * _mat[3][3] - _mat[0][3] * _mat[3][1];
	f32 det2_03_23 = _mat[0][2] * _mat[3][3] - _mat[0][3] * _mat[3][2];

	f32 det2_13_01 = _mat[1][0] * _mat[3][1] - _mat[1][1] * _mat[3][0];
	f32 det2_13_02 = _mat[1][0] * _mat[3][2] - _mat[1][2] * _mat[3][0];
	f32 det2_13_03 = _mat[1][0] * _mat[3][3] - _mat[1][3] * _mat[3][0];
	f32 det2_13_12 = _mat[1][1] * _mat[3][2] - _mat[1][2] * _mat[3][1];
	f32 det2_13_13 = _mat[1][1] * _mat[3][3] - _mat[1][3] * _mat[3][1];
	f32 det2_13_23 = _mat[1][2] * _mat[3][3] - _mat[1][3] * _mat[3][2];

	f32 det3_203_012 = _mat[2][0] * det2_03_12 - _mat[2][1] * det2_03_02 + _mat[2][2] * det2_03_01;
	f32 det3_203_013 = _mat[2][0] * det2_03_13 - _mat[2][1] * det2_03_03 + _mat[2][3] * det2_03_01;
	f32 det3_203_023 = _mat[2][0] * det2_03_23 - _mat[2][2] * det2_03_03 + _mat[2][3] * det2_03_02;
	f32 det3_203_123 = _mat[2][1] * det2_03_23 - _mat[2][2] * det2_03_13 + _mat[2][3] * det2_03_12;

	f32 det3_213_012 = _mat[2][0] * det2_13_12 - _mat[2][1] * det2_13_02 + _mat[2][2] * det2_13_01;
	f32 det3_213_013 = _mat[2][0] * det2_13_13 - _mat[2][1] * det2_13_03 + _mat[2][3] * det2_13_01;
	f32 det3_213_023 = _mat[2][0] * det2_13_23 - _mat[2][2] * det2_13_03 + _mat[2][3] * det2_13_02;
	f32 det3_213_123 = _mat[2][1] * det2_13_23 - _mat[2][2] * det2_13_13 + _mat[2][3] * det2_13_12;

	f32 det3_301_012 = _mat[3][0] * det2_01_12 - _mat[3][1] * det2_01_02 + _mat[3][2] * det2_01_01;
	f32 det3_301_013 = _mat[3][0] * det2_01_13 - _mat[3][1] * det2_01_03 + _mat[3][3] * det2_01_01;
	f32 det3_301_023 = _mat[3][0] * det2_01_23 - _mat[3][2] * det2_01_03 + _mat[3][3] * det2_01_02;
	f32 det3_301_123 = _mat[3][1] * det2_01_23 - _mat[3][2] * det2_01_13 + _mat[3][3] * det2_01_12;
	
	CMatrix4 final;
	final[0][0] =	- det3_213_123 * invDet;
	final[1][0] = + det3_213_023 * invDet;
	final[2][0] = - det3_213_013 * invDet;
	final[3][0] = + det3_213_012 * invDet;

	final[0][1] = + det3_203_123 * invDet;
	final[1][1] = - det3_203_023 * invDet;
	final[2][1] = + det3_203_013 * invDet;
	final[3][1] = - det3_203_012 * invDet;

	final[0][2] = + det3_301_123 * invDet;
	final[1][2] = - det3_301_023 * invDet;
	final[2][2] = + det3_301_013 * invDet;
	final[3][2] = - det3_301_012 * invDet;

	final[0][3] = - det3_201_123 * invDet;
	final[1][3] = + det3_201_023 * invDet;
	final[2][3] = - det3_201_013 * invDet;
	final[3][3] = + det3_201_012 * invDet;

	return final;
}

CMatrix4 CMatrix4::Transpose() const
{
	CMatrix4 transpose;
   
	for (u32 i = 0; i < 4; i++) 
		for (u32 j = 0; j < 4; j++ ) 
			transpose[i][j] = _mat[j][i];

	return transpose;
}

Engine::Containers::CString	CMatrix4::ToString()
{
	return S("[(") + _mat[0][0] + "," + _mat[0][1] + "," + _mat[0][2] + "," + _mat[0][3] + "),(" + _mat[1][0] + "," + _mat[1][1] + "," + _mat[1][2] + "," + _mat[1][3] + "),(" + _mat[2][0] + "," + _mat[2][1] + "," + _mat[2][2] + "," + _mat[2][3] + "),(" + _mat[3][0] + "," + _mat[3][1] + "," + _mat[3][2] + "," + _mat[3][3] + ")]";
}

bool CMatrix4::FromString(Engine::Containers::CString str, CMatrix4& a)
{
	if (str.Length() <= 0 ||								// Needs a string :3
		str[0] != '[' || str[str.Length() - 1] != ']' ||	// Needs the braces around it.
		str.Count(',') != 15)								// Needs exactly 1 seperating comma.
		return false;

	str = str.Trim("[] ");

	Engine::Containers::CArray<Engine::Containers::CString> split = str.Split(',');
	a._mat[0][0] = split[0].Trim("() ").ToFloat();
	a._mat[0][1] = split[1].Trim("() ").ToFloat();
	a._mat[0][2] = split[2].Trim("() ").ToFloat();
	a._mat[0][3] = split[3].Trim("() ").ToFloat();
	
	a._mat[1][0] = split[4].Trim("() ").ToFloat();
	a._mat[1][1] = split[5].Trim("() ").ToFloat();
	a._mat[1][2] = split[6].Trim("() ").ToFloat();
	a._mat[1][3] = split[7].Trim("() ").ToFloat();
	
	a._mat[2][0] = split[8].Trim("() ").ToFloat();
	a._mat[2][1] = split[9].Trim("() ").ToFloat();
	a._mat[2][2] = split[10].Trim("() ").ToFloat();
	a._mat[2][3] = split[11].Trim("() ").ToFloat();

	a._mat[3][0] = split[12].Trim("() ").ToFloat();
	a._mat[3][1] = split[13].Trim("() ").ToFloat();
	a._mat[3][2] = split[14].Trim("() ").ToFloat();
	a._mat[3][3] = split[15].Trim("() ").ToFloat();

	return true;
}
		
CMatrix4 CMatrix4::GetIdentity()
{
	return CMatrix4(CVector4(1, 0, 0, 0),
					CVector4(0, 1, 0, 0),
					CVector4(0, 0, 1, 0),
					CVector4(0, 0, 0, 1));
}

CMatrix4 CMatrix4::GetZero()
{
	return CMatrix4(CVector4(0, 0, 0, 0),
					CVector4(0, 0, 0, 0),
					CVector4(0, 0, 0, 0),
					CVector4(0, 0, 0, 0));
}
