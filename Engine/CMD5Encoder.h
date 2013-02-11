///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CDataTransformer.h"

namespace Engine
{
	namespace Data
	{
		namespace Hashing
		{
			static const u8 MD5_PADDING[64] = {
			  (u8)0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			};

			// F, G, H and I are basic MD5 functions.
			#define MD5_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
			#define MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
			#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
			#define MD5_I(x, y, z) ((y) ^ ((x) | (~z)))

			// ROTATE_LEFT rotates x left n bits.
			#define MD5_ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

			// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
			// Rotation is separate from addition to prevent recomputation.
			#define MD5_FF(a, b, c, d, x, s, ac) { \
			  (a) += MD5_F ((b), (c), (d)) + (x) + (u32)(ac); \
			  (a) = MD5_ROTATE_LEFT ((a), (s)); \
			  (a) += (b); \
			  }
			#define MD5_GG(a, b, c, d, x, s, ac) { \
			  (a) += MD5_G ((b), (c), (d)) + (x) + (u32)(ac); \
			  (a) = MD5_ROTATE_LEFT ((a), (s)); \
			  (a) += (b); \
			  }
			#define MD5_HH(a, b, c, d, x, s, ac) { \
			  (a) += MD5_H ((b), (c), (d)) + (x) + (u32)(ac); \
			  (a) = MD5_ROTATE_LEFT ((a), (s)); \
			  (a) += (b); \
			  }
			#define MD5_II(a, b, c, d, x, s, ac) { \
			  (a) += MD5_I ((b), (c), (d)) + (x) + (u32)(ac); \
			  (a) = MD5_ROTATE_LEFT ((a), (s)); \
			  (a) += (b); \
			  }

			// Converts a buffer of data into a 128bit (16 byte) md5 hash value.
			class CMD5Encoder : public CDataTransformer 
			{
				private:
					u32 _state			[4];
					u32 _count			[2];
					u8	_buffer			[64];

					void Encode					(u8*  output, u32* input, u32 len);
					void Decode					(u32* output,  u8* input, u32 len);
					void Transform				(u32 state[4], u8 block[64]);

				public:
					CMD5Encoder					();

					void Initialize				();
					void AddBuffer				(const u8* buffer, u32 size);
					CTransformedData Calculate	();			
			};

		}
	}
}