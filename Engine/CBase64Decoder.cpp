///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CDataTransformer.h"
#include "CBase64Decoder.h"

using namespace Engine::Data;
using namespace Engine::Data::Compression;

CBase64Decoder::CBase64Decoder()
{
	_input = "";
}

void CBase64Decoder::Initialize()
{
	_input = "";
}

void CBase64Decoder::AddBuffer(const u8* buffer, u32 size)
{
	_input += Engine::Containers::CString(buffer, size);
}

CTransformedData CBase64Decoder::Calculate()
{
	Engine::Containers::CString output = "";
	Engine::Containers::CString base64Chars = BASE64_DECODE_CHARS;

	// Strip out any newlines.
	_input = _input.Replace("\n", "");

	s32 i = 0;
	s32 charIndex = 0;
	s32 oct = 0;
	s32 qc = 0;

	do
	{
		charIndex = base64Chars.IndexOf(_input[i]) + 1;
		if (charIndex > 0)
		{
			if (charIndex == 65) 
				charIndex = 1;
			oct = (oct << 6) | ((charIndex - 1) & 0x3F);
			qc = qc + 1;
		}

		if (qc == 4)
		{
			output += (u8)((oct >> 16) & 0xFF);
			output += (u8)((oct >> 8) & 0xFF);
			output += (u8)(oct & 0xFF);
			oct = 0;
			qc = 0;
		}

		i++;
	} while (i < (s32)_input.Length());

	return CTransformedData(output.c_str(), output.Length());
}