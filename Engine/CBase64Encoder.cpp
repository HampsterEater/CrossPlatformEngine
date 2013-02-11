///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CDataTransformer.h"
#include "CBase64Encoder.h"

using namespace Engine::Data;
using namespace Engine::Data::Compression;

CBase64Encoder::CBase64Encoder()
{
	_input = "";
}

void CBase64Encoder::Initialize()
{
	_input = "";
}

void CBase64Encoder::AddBuffer(const u8* buffer, u32 size)
{
	_input += Engine::Containers::CString(buffer, size);
}

CTransformedData CBase64Encoder::Calculate()
{
	Engine::Containers::CString output = "";	
	u32 chars = 0;
	u32 i = 0;

	while (i < _input.Length())
	{
		u32 remainingChars = _input.Length() - i;

		// Encoder next characters based on how mayn remain. 
		switch (remainingChars)
		{
			// 2 characters remaining!
			case 2:
				output += BASE64_ENCODE_CHARS[(_input[i + 0] >> 2)];
				output += BASE64_ENCODE_CHARS[((_input[i + 0] << 4) | (_input[i + 1] >> 4))];
				output += BASE64_ENCODE_CHARS[((_input[i + 1] << 4) & 0x3F)];
				output += "=";
				break;

			// 1 character remaining.
			case 1:
				output += BASE64_ENCODE_CHARS[(_input[i + 0] >> 2)];
				output += BASE64_ENCODE_CHARS[((_input[i + 0] << 4) & 0x3F)];
				output += "==";
				break;

			// 3 or more characters remaining.
			default:
				output += BASE64_ENCODE_CHARS[(_input[i + 0] >> 2)];
				output += BASE64_ENCODE_CHARS[(((_input[i + 0] << 4) | (_input[i + 1] >> 4)) & 0x3F)];
				output += BASE64_ENCODE_CHARS[(((_input[i + 1] << 2) | (_input[i + 2] >> 6)) & 0x3F)];
				output += BASE64_ENCODE_CHARS[(_input[i + 2] & 0x3F)];
				break;
		}

		// Add a new line every 76 characters,
		i += 3;
		chars += 4;
		if (chars == 76)
		{
			output += "\n";
			chars = 0;
		}

	} 

	// Add final new line.
	output += "\n";

	return CTransformedData(output.c_str(), output.Length());
}