///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CStream.h"
#include "Endianness.h"
#include "Memory.h"
#include "CLog.h"

using namespace Engine::FileSystem::Streams;

template <typename T> T	CStream::ReadPOD()
{
	T buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(T));
	return buff;
}				

u64 CStream::ReadU64()
{
	u64 buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(buff));
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&buff), sizeof(buff));
	return buff;
}

s64 CStream::ReadS64()
{
	s64 buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(buff));
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&buff), sizeof(buff));
	return buff;
}

u32 CStream::ReadU32()
{
	u32 buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(buff));
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&buff), sizeof(buff));
	return buff;
}

s32 CStream::ReadS32()
{
	s32 buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(buff));
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&buff), sizeof(buff));
	return buff;
}

u16 CStream::ReadU16()
{
	u16 buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(buff));
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&buff), sizeof(buff));
	return buff;
} 

s16 CStream::ReadS16()
{
	s16 buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(buff));
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&buff), sizeof(buff));
	return buff;
}

u8 CStream::ReadU8()
{
	u8 buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(buff));
	return buff;
}

s8 CStream::ReadS8()
{
	s8 buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(buff));
	return buff;
}

f64 CStream::ReadF64()
{
	f64 buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(buff));
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&buff), sizeof(buff));
	return buff;
}

f32 CStream::ReadF32()
{
	f32 buff;
	ReadBytes(reinterpret_cast<u8*>(&buff), sizeof(buff));
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&buff), sizeof(buff));
	return buff;
}



Engine::Containers::CString CStream::ReadLine()
{
	Engine::Containers::CString line = "";
	while (!AtEnd())
	{
		u8 c = ReadU8();
		if (c == '\r')
		{
			continue;
		}
		else if (c != '\n')
		{
			line += c;
		}
		else
		{
			break;
		}
	}
	return line;
}

Engine::Containers::CString CStream::ReadToEnd()
{
	u64 len = Length() - Position();
	if (len <= 0)
	{
		return "";
	}

	u8* data = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc((u32)len + 1);
	ReadBytes(data, len);
	data[len] = '\0';

	Engine::Containers::CString str(data, (u32)len);
	Engine::Memory::GetDefaultAllocator()->Free(&data);

	return str;
}

Engine::Containers::CString CStream::ReadText(u32 len)
{	
	LOG_ASSERT_MSG(Position() + len <= Length(), "Failed to read string, string length goes beyond stream length.");

	u8* data = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(len + 1);
	ReadBytes(data, len);	
	data[len] = '\0';

	Engine::Containers::CString str = data;
	Engine::Memory::GetDefaultAllocator()->Free(&data);

	return str;
}

Engine::Containers::CString CStream::ReadString()
{
	u32 len = ReadU32();	
	LOG_ASSERT_MSG(Position() + len <= Length(), "Failed to read string, string length goes beyond stream length.");

	u8* data = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(len + 1);
	ReadBytes(data, len);	
	data[len] = '\0';

	Engine::Containers::CString str = data;
	Engine::Memory::GetDefaultAllocator()->Free(&data);

	return str;
}




template <typename T> void CStream::WritePOD(T data)
{
	T* buff = &data;
	WriteBytes(reinterpret_cast<u8*>(buff), sizeof(T));
}

void CStream::WriteU64(u64 data)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&data), sizeof(data));
	WriteBytes(reinterpret_cast<u8*>(&data), sizeof(data));
}

void CStream::WriteS64(s64 data)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&data), sizeof(data));
	WriteBytes(reinterpret_cast<u8*>(&data), sizeof(data));
}

void CStream::WriteU32(u32 data)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&data), sizeof(data));
	WriteBytes(reinterpret_cast<u8*>(&data), sizeof(data));
}

void CStream::WriteS32(s32 data)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&data), sizeof(data));
	WriteBytes(reinterpret_cast<u8*>(&data), sizeof(data));
}

void CStream::WriteU16(u16 data)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&data), sizeof(data));
	WriteBytes(reinterpret_cast<u8*>(&data), sizeof(data));
}

void CStream::WriteS16(s16 data)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&data), sizeof(data));
	WriteBytes(reinterpret_cast<u8*>(&data), sizeof(data));
}

void CStream::WriteU8(u8 data)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&data), sizeof(data));
	WriteBytes(reinterpret_cast<u8*>(&data), sizeof(data));
}

void CStream::WriteS8(s8 data)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&data), sizeof(data));
	WriteBytes(reinterpret_cast<u8*>(&data), sizeof(data));
}

void CStream::WriteF64(f64 data)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&data), sizeof(data));
	WriteBytes(reinterpret_cast<u8*>(&data), sizeof(data));
}

void CStream::WriteF32(f32 data)
{
	if (Engine::Misc::IsBigEndian())
		Engine::Misc::SwapEndian(reinterpret_cast<u8*>(&data), sizeof(data));
	WriteBytes(reinterpret_cast<u8*>(&data), sizeof(data));
}




void CStream::WriteLine(Engine::Containers::CString s)
{
	WriteBytes(s.c_str(), s.Length());
	WriteU8('\n');
}

void CStream::WriteText(Engine::Containers::CString s)
{
	WriteBytes(s.c_str(), s.Length());
}

void CStream::WriteString(Engine::Containers::CString s)
{
	WriteU32(s.Length());
	WriteBytes(s.c_str(), s.Length());
}