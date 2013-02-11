///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright � 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

// This code is derived from work by RSA Data Security, Inc. MD5 Message-Digest Algorithm
// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.

#include "CDataTransformer.h"
#include "CMD5Encoder.h"

#include <cstring>

using namespace Engine::Data;
using namespace Engine::Data::Hashing;

CMD5Encoder::CMD5Encoder()
{
}

void CMD5Encoder::Initialize()
{
	_state[0] = 0x67452301; 
	_state[1] = 0xefcdab89;
	_state[2] = 0x98badcfe;
	_state[3] = 0x10325476;
	
	_count[0] = 0;
	_count[1] = 0;

	memset(_buffer, 0, 64);
}

void CMD5Encoder::Encode(u8* output, u32* input, u32 len)
{
	u32 i, j;

    for (i = 0, j = 0; j < len; i++, j += 4) 
	{
		output[j]     = (u8) (input[i] & 0xff);
		output[j + 1] = (u8)((input[i] >> 8) & 0xff);
		output[j + 2] = (u8)((input[i] >> 16) & 0xff);
		output[j + 3] = (u8)((input[i] >> 24) & 0xff);
    }
}

void CMD5Encoder::Decode(u32* output,  u8* input, u32 len)
{
	u32 i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
	{
		output[i] = ((u32)input[j]) | (((u32)input[j + 1]) << 8) | (((u32)input[j + 2]) << 16) | (((u32)input[j + 3]) << 24);
	}
}

void CMD5Encoder::Transform(u32 state[4], u8 block[64])
{
	u32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	Decode(x, block, 64);

	/* Round 1 */
    MD5_FF(a, b, c, d, x[ 0], 7, 0xd76aa478); /* 1 */
    MD5_FF(d, a, b, c, x[ 1], 12, 0xe8c7b756); /* 2 */
    MD5_FF(c, d, a, b, x[ 2], 17, 0x242070db); /* 3 */
    MD5_FF(b, c, d, a, x[ 3], 22, 0xc1bdceee); /* 4 */
    MD5_FF(a, b, c, d, x[ 4], 7, 0xf57c0faf); /* 5 */
    MD5_FF(d, a, b, c, x[ 5], 12, 0x4787c62a); /* 6 */
    MD5_FF(c, d, a, b, x[ 6], 17, 0xa8304613); /* 7 */
    MD5_FF(b, c, d, a, x[ 7], 22, 0xfd469501); /* 8 */
    MD5_FF(a, b, c, d, x[ 8], 7, 0x698098d8); /* 9 */
    MD5_FF(d, a, b, c, x[ 9], 12, 0x8b44f7af); /* 10 */
    MD5_FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
    MD5_FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
    MD5_FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */
    MD5_FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
    MD5_FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
    MD5_FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */
	
    /* Round 2 */
    MD5_GG(a, b, c, d, x[ 1], 5, 0xf61e2562); /* 17 */
    MD5_GG(d, a, b, c, x[ 6], 9, 0xc040b340); /* 18 */
    MD5_GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
    MD5_GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); /* 20 */
    MD5_GG(a, b, c, d, x[ 5], 5, 0xd62f105d); /* 21 */
    MD5_GG(d, a, b, c, x[10], 9,  0x2441453); /* 22 */
    MD5_GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
    MD5_GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); /* 24 */
    MD5_GG(a, b, c, d, x[ 9], 5, 0x21e1cde6); /* 25 */
    MD5_GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */
    MD5_GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); /* 27 */
    MD5_GG(b, c, d, a, x[ 8], 20, 0x455a14ed); /* 28 */
    MD5_GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */
    MD5_GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8); /* 30 */
    MD5_GG(c, d, a, b, x[ 7], 14, 0x676f02d9); /* 31 */
    MD5_GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    MD5_HH(a, b, c, d, x[ 5], 4, 0xfffa3942); /* 33 */
    MD5_HH(d, a, b, c, x[ 8], 11, 0x8771f681); /* 34 */
    MD5_HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
    MD5_HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
    MD5_HH(a, b, c, d, x[ 1], 4, 0xa4beea44); /* 37 */
    MD5_HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9); /* 38 */
    MD5_HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); /* 39 */
    MD5_HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
    MD5_HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */
    MD5_HH(d, a, b, c, x[ 0], 11, 0xeaa127fa); /* 42 */
    MD5_HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); /* 43 */
    MD5_HH(b, c, d, a, x[ 6], 23,  0x4881d05); /* 44 */
    MD5_HH(a, b, c, d, x[ 9], 4, 0xd9d4d039); /* 45 */
    MD5_HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
    MD5_HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
    MD5_HH(b, c, d, a, x[ 2], 23, 0xc4ac5665); /* 48 */
	
    /* Round 4 */
    MD5_II(a, b, c, d, x[ 0], 6, 0xf4292244); /* 49 */
    MD5_II(d, a, b, c, x[ 7], 10, 0x432aff97); /* 50 */
    MD5_II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
    MD5_II(b, c, d, a, x[ 5], 21, 0xfc93a039); /* 52 */
    MD5_II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */
    MD5_II(d, a, b, c, x[ 3], 10, 0x8f0ccc92); /* 54 */
    MD5_II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
    MD5_II(b, c, d, a, x[ 1], 21, 0x85845dd1); /* 56 */
    MD5_II(a, b, c, d, x[ 8], 6, 0x6fa87e4f); /* 57 */
    MD5_II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
    MD5_II(c, d, a, b, x[ 6], 15, 0xa3014314); /* 59 */
    MD5_II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
    MD5_II(a, b, c, d, x[ 4], 6, 0xf7537e82); /* 61 */
    MD5_II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
    MD5_II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); /* 63 */
    MD5_II(b, c, d, a, x[ 9], 21, 0xeb86d391); /* 64 */
	
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

void CMD5Encoder::AddBuffer(const u8* input, u32 inputLen)
{
	u32 i, index, partLen;

	// Compute number of bytes mod 64.
	index = (u32)((_count[0] >> 3) & 0x3F);

	// Update number of  bits.
    if ((_count[0] += ((u32)inputLen << 3)) < ((u32)inputLen << 3))
		_count[1]++;
	_count[1] += ((u32)inputLen >> 29);

	partLen = 64 - index;

	// Transform as many times as possible.
	if (inputLen >= partLen)
	{
		memcpy((_buffer + index), input, partLen);
		Transform(_state, _buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
		{
			Transform(_state, const_cast<u8*>(input + i));
		}

		index = 0;
	}
	else
	{
		i = 0;
	}

	// Buffer remaining input.
	memcpy((_buffer + index), (input + i), inputLen - i);
}

CTransformedData CMD5Encoder::Calculate()
{
	u8  digest[16];
	u8  bits[8];
	u32 index, padLen;

	// Save number of bits.
	Encode(bits, _count, 8);
	
	// Pad out to 56 mod 64.
	index  = (u32)((_count[0] >> 3) & 0x3F);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	AddBuffer((u8*)MD5_PADDING, padLen);

	// Append length (before padding)
	AddBuffer(bits, 8);

	// Store state in digest.
	Encode(digest, _state, 16);

	return CTransformedData(digest, 16);
}