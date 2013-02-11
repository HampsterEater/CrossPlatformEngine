///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <stdio.h>
#include <cctype>
#include <cstdlib>
#include <cassert>

#include "CString.h"
#include "CArray.h"

#include "Memory.h"

using namespace Engine::Containers;

Engine::Memory::Allocators::CProxyAllocator* Engine::Containers::g_string_allocator = NULL;

void Engine::Containers::InitStringAllocator()
{
    Engine::Memory::Allocators::CAllocator* alloc = Engine::Memory::GetDefaultAllocator();
    Engine::Containers::g_string_allocator = alloc->NewObj<Engine::Memory::Allocators::CProxyAllocator>("String Allocator", alloc);
}

void Engine::Containers::FreeStringAllocator()
{
    Engine::Memory::GetDefaultAllocator()->FreeObj(&Engine::Containers::g_string_allocator);
    Engine::Containers::g_string_allocator = NULL;
}

void CString::Initialize()
{
    _data       = _startBuffer;
    _allocated  = STRING_ALLOC_START;
    _length     = 0;
	_data[0]	= '\0';
	// memset(_data, 0, STRING_ALLOC_START);
}

CString::~CString()
{
    if (_data != NULL && _data != _startBuffer)
	{
        GetStringAllocator()->Free(&_data);
		_data = NULL;
	}
}

CString::CString()
{
    Initialize();
    Allocate(0);
}

CString::CString(const u8* v, u32 length)
{
    Initialize();
    CopyFrom(v, length);
}

CString::CString(const u8* c)
{
    Initialize();
    if (c != NULL)
    {
        CopyFrom(c, strlen(c));
    }
    else
    {
        Allocate(0);
    }
}

CString::CString(const CString& c)
{
    Initialize();
	if (c._length == 0)
		return;

    CopyFrom(c._data, c._length);
}

CString::CString(const u8 v, u32 length)
{
    Initialize();
    Allocate(length);
    memset(_data, v, length);
}

CString::CString(const CString& v, u32 length)
{
    LOG_ASSERT(v._length > 0);

    Initialize();
    Allocate(length);
    for (u32 i = 0; i < length; i++)
    {
        _data[i] = v._data[i % v._length];
    }
}

CString::CString(const CString& v1, const CString& v2, const CString& v3, const CString& v4, const CString& v5, const CString& v6, const CString& v7, const CString& v8, const CString& v9, const CString& v10)
{
    Initialize();
    CopyFrom(v1._data, v1._length);

    Append(v2);
    Append(v3);
    Append(v4);
    Append(v5);
    Append(v6);
    Append(v7);
    Append(v8);
    Append(v9);
    Append(v10);
}

CString::CString(const bool v)
{
    Initialize();
    Allocate(v == true ? 4 : 5);

    if (v == true)
    {
        _data[0] = 't';
        _data[1] = 'r';
        _data[2] = 'u';
        _data[3] = 'e';
    }
    else
    {
        _data[0] = 'f';
        _data[1] = 'a';
        _data[2] = 'l';
        _data[3] = 's';
        _data[4] = 'e';
    }
}

CString::CString(const u8 v)
{
    Initialize();
    Allocate(1);
    _data[0] = v;
}

CString::CString(const s32 v)
{
    u8  buffer[128];
    s32 length = 0;
    Initialize();

    length = sprintf_s( buffer, 128, "%d", v );
    Allocate(length);

    memcpy(_data, buffer, length);
    _data[length] = '\0';
}

CString::CString(const u32 v)
{
    u8  buffer [128];
    s32 length = 0;
    Initialize();

    length = sprintf_s( buffer, 128, "%u", v );
    Allocate(length);

    memcpy(_data, buffer, length);
    _data[length] = '\0';
}

CString::CString(const s64 v)
{
    u8  buffer[128];
    s32 length = 0;
    Initialize();

    length = sprintf_s( buffer, 128, "%lld", v );
    Allocate(length);

    memcpy(_data, buffer, length);
    _data[length] = '\0';
}

CString::CString(const u64 v)
{
    u8  buffer [128];
    s32 length = 0;
    Initialize();

    length = sprintf_s( buffer, 128, "%llu", v );
    Allocate(length);

    memcpy(_data, buffer, length);
    _data[length] = '\0';
}

CString::CString(const f32 v)
{
    u8  buffer [128];
    s32 length = 0;
    Initialize();

    length = sprintf_s( buffer, 128, "%f", v );
    Allocate(length);

    memcpy(_data, buffer, length);
    _data[length] = '\0';
}

void CString::CopyFrom(const u8* buffer, u32 length)
{
	if (length == 0 && _length == 0)
		return;

    Allocate(length);
    memcpy(_data, buffer, length);
}

void CString::Append(const u8* buffer, u32 length)
{
    if (length == 0)
        return;

    s32 oldLength = _length;
    Allocate(_length + length, true);
    memcpy(_data + oldLength, buffer, length);
}

void CString::Append(const CString& a)
{
    Append(a._data, a._length);
}

void CString::Allocate(u32 size, bool keepOld)
{
    u8* oldData      = _data;
    u32 allocateSize = size + 1; // Always allocate 1 more for the null byte when returning c_str'ings!

    if (allocateSize > _allocated || _data == NULL)
    {
        // Make sure we only go up in increments of our string allocation s32erval.
        /*s32 newSize = allocateSize % STRING_ALLOC_s32ERVAL;
        if (newSize == 0)
        {
            newSize = allocateSize;
        }
        else
        {
            newSize = (allocateSize + STRING_ALLOC_s32ERVAL) - newSize;
        }*/

        s32 newSize = (s32)(allocateSize * STRING_ALLOC_INTERVAL);
        //prsintf("ALLOCATIONS:%i\n", newSize);

		_data = (char*)GetStringAllocator()->Alloc(newSize);//new u8[newSize];
        _allocated = newSize;

        if (keepOld == true)
        {
            memcpy(_data, oldData, _length);
        }

        if (oldData != NULL && oldData != _startBuffer)
        {
            GetStringAllocator()->Free(&oldData);
        }
    }

    _length = size;
    _data[_length] = '\0'; // Add null byte for c_str'ings!
}

const u8 CString::operator[](u32 index) const 
{
    LOG_ASSERT(index >= 0 && index < _length);
    return _data[index];
}

const u8 CString::operator[](s32 index) const 
{
    LOG_ASSERT(index >= 0 && index < (s32)_length);
    return _data[index];
}

void CString::operator=(const CString &str)
{
    CopyFrom(str._data, str._length);
}

void CString::operator=(const u8* text)
{
    if (text == NULL)
    {
        Allocate(0);
    }
    else
    {
        CopyFrom(text, strlen(text));
    }
}

CString Engine::Containers::operator+(const CString& a, const CString& b)
{
    CString str(a);
    str.Append(b);
    return str;
}

CString Engine::Containers::operator+(const u8* a, const CString& b)
{
    CString str(a);
    str.Append(b);
    return str;
}

CString Engine::Containers::operator+(const CString& a, const u8* b)
{
    CString str(a);
    str.Append(b, strlen(b));
    return str;
}

CString Engine::Containers::operator+(const CString& a, const bool b)
{
    CString str(a);
    str.Append(b);
    return str;
}

CString Engine::Containers::operator+(const CString& a, const f32 b)
{
    CString str(a);
    str.Append(b);
    return str;
}

CString Engine::Containers::operator+(const CString& a, const s32 b)
{
    CString str(a);
    str.Append(b);
    return str;
}

CString Engine::Containers::operator+(const CString& a, const u32 b)
{
    CString str(a);
    str.Append(b);
    return str;
}

CString Engine::Containers::operator+(const CString& a, const s64 b)
{
    CString str(a);
    str.Append(b);
    return str;
}

CString Engine::Containers::operator+(const CString& a, const u64 b)
{
    CString str(a);
    str.Append(b);
    return str;
}

CString Engine::Containers::operator+(const CString& a, const u8 b)
{
    CString str(a);
    str.Append(b);
    return str;
}

CString& CString::operator+=(const CString& a)
{
    Append(a._data, a._length);
    return *this;
}

CString& CString::operator+=(const u8* a)
{
    Append(a, strlen(a));
    return *this;
}

CString& CString::operator+=(const f32 a)
{
    u8 buffer[64];
    s32  len = sprintf_s(buffer, 64, "%f", a);

    Append(buffer, len);

    return *this;
}

CString& CString::operator+=(const u8 a)
{
    Append(&a, 1);
    return *this;
}

CString& CString::operator+=(const s32 a)
{
    u8 buffer[64];
    s32  len = sprintf_s(buffer, 64, "%d", a);

    Append(buffer, len);

    return *this;
}

CString& CString::operator+=(const u32 a)
{
    u8 buffer[64];
    s32  len = sprintf_s(buffer, 64, "%u", a);

    Append(buffer, len);

    return *this;
}

CString& CString::operator+=(const s64 a)
{
    u8 buffer[64];
    s32  len = sprintf_s(buffer, 64, "%lld", a);

    Append(buffer, len);

    return *this;
}

CString& CString::operator+=(const u64 a)
{
    u8 buffer[64];
    s32  len = sprintf_s(buffer, 64, "%llu", a);

    Append(buffer, len);

    return *this;
}

CString& CString::operator+=(const bool a)
{
    Append(a == true ? "true" : "false", a == true ? 4 : 5);
    return *this;
}

bool Engine::Containers::operator==(const CString& a, const CString& b)
{
    return CString::Compare(a._data, a._length, b._data, b._length);
}

bool Engine::Containers::operator==(const CString& a, const u8* b)
{
    return CString::Compare(a._data, a._length, b, strlen(b));
}

bool Engine::Containers::operator==(const u8*     a, const CString& b)
{
    return CString::Compare(a, strlen(a), b._data, b._length);
}

bool Engine::Containers::operator!=(const CString& a, const CString& b)
{
    return !CString::Compare(a._data, a._length, b._data, b._length);
}

bool Engine::Containers::operator!=(const CString& a, const u8* b)
{
    return !CString::Compare(a._data, a._length, b, strlen(b));
}

bool Engine::Containers::operator!=(const u8*     a, const CString& b)
{
    return !CString::Compare(a, strlen(a), b._data, b._length);
}

bool CString::Compare(const u8* a, u32 alen, const u8* b, u32 blen)
{
    // Sanity checks for obvious situations!
    if (alen == 0 || blen == 0)
        return (alen == blen);
    if (alen != blen)
        return false;

    // Go check the u8acters!
    for (u32 i = 0; i < alen; i++)
    {
        if (a[i] != b[i])
            return false;
    }

    return true;
}

CString CString::ToLower() const
{
    CString str;
    str.Allocate(_length);

    for (u32 i = 0; i < _length; i++)
    {
        str._data[i] = (u8)tolower(_data[i]);
    }

    return str;
}

CString CString::ToUpper() const
{
    CString str;
    str.Allocate(_length);

    for (u32 i = 0; i < _length; i++)
    {
        str._data[i] = (u8)toupper(_data[i]);
    }

    return str;
}

s32 CString::ToInt() const
{
    return atoi(_data);
}

f32 CString::ToFloat() const
{
    return (f32)atof(_data);
}

bool CString::ToBool() const
{
    CString trimmedLower = CString(*this).Trim().ToLower();

    if (_length == 0 || trimmedLower == "false" || trimmedLower == "0")
    {
        return false;
    }
    else
    {
        return true;
    }
}

u32 CString::ToHashCode() const
{
    u32 hash = 0;
    for (u32 i = 0; i < _length; i++)
    {
        hash = 31 * hash + _data[i];
    }
    return hash;
}

CString CString::TrimStart(const CString& a, bool aschars) const
{
	if (_length <= 0)
	{
		return *this;
	}

    u32 startIndex = 0;

    LOG_ASSERT(_length > 0);
    LOG_ASSERT(a._length > 0);

    if (aschars == true)
    {
        for (u32 i = 0; i < _length; i++)
        {
            bool strip = false;

            // See if this u8acter is in strip string.
            for (u32 j = 0; j < a._length; j++)
            {
                if (a._data[j] == _data[i])
                {
                    strip = true;
                    break;
                }
            }

            if (strip == false)
            {
                break;
            }
            else
            {
                startIndex += 1;
            }
        }
    }
    else
    {
        for (u32 i = 0; i < _length; i++)
        {
            if (_data[i] == a._data[i % a._length])
                startIndex++;
            else
                break;
        }
    }

    // Trim string and return.
    CString str;
    str.Allocate(_length - startIndex);

    memcpy(str._data, _data + startIndex, _length - startIndex);

    return str;
}

CString CString::TrimEnd(const CString& a, bool aschars) const
{
	if (_length <= 0)
	{
		return *this;
	}

    u32 endIndex = _length - 1;

    LOG_ASSERT(_length > 0);
    LOG_ASSERT(a._length > 0);

    if (aschars == true)
    {
        for (u32 i = _length - 1; i >= 0; i--)
        {
            bool strip = false;

            // See if this u8acter is in strip string.
            for (u32 j = 0; j < a._length; j++)
            {
                if (a._data[j] == _data[i])
                {
                    strip = true;
                    break;
                }
            }

            if (strip == false)
            {
                break;
            }
            else
            {
                endIndex -= 1;
            }
        }
    }
    else
    {
        s32 tOffset = a._length - 1;
        for (u32 i = 0; i < _length; i++)
        {
            s32 c = _data[_length - (i + 1)];
            s32 t = a._data[tOffset];

            if (c == t)
            {
                endIndex--;
            }
            else
            {
                break;
            }

            if (--tOffset < 0)
            {
                tOffset = a._length - 1;
            }
        }
    }

    // Trim string and return.
    CString str;
    str.Allocate(endIndex + 1);

    memcpy(str._data, _data, endIndex + 1);

    return str;
}

CString CString::Trim(const CString& a, bool aschars) const
{
    return TrimStart(a, aschars).TrimEnd(a, aschars);
}

CString CString::SubString(u32 start, u32 length) const
{
    if (length == 0)
    {
		if (start == 0)
			return "";

        length = (_length - start);
    }

    LOG_ASSERT(start >= 0 && start < _length);
    LOG_ASSERT(start + length >= 0 && start + length <= _length);

    // Trim string and return.
    CString str;
    str.Allocate(length);

    memcpy(str._data, _data + start, length);

//	s32 off = _length - (start + length);
//	printf("OFFSET:%i\n", off);

    return str;
}

CString CString::Start(u32 length) const
{
    LOG_ASSERT(length <= _length);

    CString str;
    str.Allocate(length);
    memcpy(str._data, _data, length);

    return str;
}

CString CString::End(u32 length) const
{
    LOG_ASSERT(length <= _length);

    CString str;
    str.Allocate(length);
    memcpy(str._data, ((_data + _length) - 1) - length, length);

    return str;
}

u8 CString::Last() const
{
    LOG_ASSERT(_length > 0);
    return _data[_length - 1];
}

u8 CString::First() const
{
    LOG_ASSERT(_length > 0);
    return _data[0];
}

CString CString::PadStart(u32 length, const CString& fill)
{
    LOG_ASSERT(length > 0 && fill._length > 0);
    if (_length > length)
    {
        return CString(*this);
    }

    CString str;
    str.Allocate(length);

    u32 excess = length - _length;
    for (u32 i = 0; i < excess; i++)
    {
        str._data[i] = fill._data[i % fill._length];
    }

    memcpy((str._data + length) - _length, _data, _length);

    return str;
}

CString CString::PadStart(u32 length, const u8 fill)
{
    LOG_ASSERT(length > 0);
    if (_length > length)
    {
        return CString(*this);
    }

    CString str;
    str.Allocate(length);

    memset(str._data, fill, length);
    memcpy((str._data + length) - _length, _data, _length);

    return str;
}

CString CString::PadEnd(u32 length, const CString& fill)
{
    LOG_ASSERT(length > 0 && fill._length > 0);

    if (_length > length)
    {
        return CString(*this);
    }

    CString str;
    str.Allocate(length);

    memcpy(str._data, _data, _length);

    u32 excess = length - _length;
    for (u32 i = 0; i < excess; i++)
    {
        str._data[_length + i] = fill._data[i % fill._length];
    }

    return str;
}

CString CString::PadEnd(u32 length, const u8 fill)
{
    LOG_ASSERT(length > 0);
    if (_length > length)
    {
        return CString(*this);
    }

    CString str;
    str.Allocate(length);

    memset(str._data, fill, length);
    memcpy(str._data, _data, _length);

    return str;
}

CString CString::Repeat(u32 count)
{
    CString str;
    str.Allocate(_length * count);

    for (u32 i = 0; i < count; i++)
    {
        memcpy(str._data + (i * _length), _data, _length);
    }

    return str;
}

CString CString::Fill(u8 c)
{
    return CString(c, _length);
}

CString CString::Fill(const CString& s)
{
    return CString(s, _length);
}

s32 CString::IndexOf(const CString& a, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length - start;
    }
    if (_length == 0)
    {
        return -1;
    }
	if (start >= _length)
	{
		return -1;
	}

    LOG_ASSERT(start >= 0 && start < _length);
    LOG_ASSERT(start + length >= 0 && start + length <= _length);

    s32 index = -1;
    for (u32 i = start; i < (start + length) - (a._length - 1); i++)
    {
        bool found = true;
        for (u32 offset = 0; offset < a._length; offset++)
        {
            if (a._data[offset] != _data[i + offset])
            {
                found = false;
                break;
            }
        }

        if (found == true)
        {
            index = i;
            break;
        }
    }

    return index;
}

s32 CString::LastIndexOf(const CString& a, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length - start;
    }
    if (_length == 0)
    {
        return -1;
    }
	if (start >= _length)
	{
		return -1;
	}

    LOG_ASSERT(start >= 0 && start < _length);
    LOG_ASSERT(start + length >= 0 && start + length <= _length);

    s32 index = -1;
    for (u32 i = ((start + length) - (a._length - 1)) - 1; i >= 0; i--)
    {
        bool found = true;
        for (u32 offset = 0; offset < a._length; offset++)
        {
            if (a._data[offset] != _data[i + offset])
            {
                found = false;
                break;
            }
        }

        if (found == true)
        {
            index = i;
            break;
        }

        // Fucking unsigned for loops.
        if (i == 0)
            break;
    }

    return index;
}

s32 CString::IndexOf(u8 a, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length - start;
    }
    if (_length == 0)
    {
        return -1;
    }
	if (start >= _length)
	{
		return -1;
	}

	//printf("IndexOf: %i/%i Length:%i\n", start, length, _length);

    LOG_ASSERT(start >= 0 && start < _length);
    LOG_ASSERT(start + length >= 0 && start + length <= _length);

    s32 index = -1;
    for (u32 i = start; i < start + length; i++)
    {
        if (_data[i] == a)
        {
            index = i;
            break;
        }
    }

    return index;
}

s32 CString::LastIndexOf(u8 b, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length - start;
    }
    if (_length == 0)
    {
        return -1;
    }
	if (start >= _length)
	{
		return -1;
	}

    LOG_ASSERT(start >= 0 && start < _length);
    LOG_ASSERT(start + length >= 0 && start + length <= _length);

    s32 index = -1;
    for (u32 i = (start + length) - 1; i >= start; i--)
    {
        if (_data[i] == b)
        {
            index = i;
            break;
        }
    }

    return index;
}

s32 CString::IndexOfAny(CArray<CString> delims, u32 start, u32 length, CString* deliminator)
{
    s32 offset = -1;
    u32 size = delims.Size();

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = IndexOf(delims[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
            if (deliminator != NULL)
            {
                *deliminator = delims[i];
            }
        }
    }

    return offset;
}

s32 CString::LastIndexOfAny(CArray<CString> delims, u32 start, u32 length, CString* deliminator)
{
    s32 offset = -1;
    u32 size = delims.Size();

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = LastIndexOf(delims[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
            if (deliminator != NULL)
            {
                *deliminator = delims[i];
            }
        }
    }

    return offset;
}
/*
s32 CString::IndexOfAny(CString a[], u32 size, u32 start, u32 length)
{
    s32 offset = -1;

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = IndexOf(a[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
        }
    }

    return offset;
}

s32 CString::LastIndexOfAny(CString b[], u32 size, u32 start, u32 length)
{
    s32 offset = -1;

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = LastIndexOf(b[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
        }
    }

    return offset;
}

s32 CString::IndexOfAny(u8 a[], u32 size, u32 start, u32 length)
{
    s32 offset = -1;

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = IndexOf(a[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
        }
    }

    return offset;
}

s32 CString::LastIndexOfAny(u8 b[], u32 size, u32 start, u32 length)
{
    s32 offset = -1;

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = LastIndexOf(b[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
        }
    }

    return offset;
}
*/


s32 CString::IndexNotOf(const CString& a, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length - start;
    }
    if (_length == 0)
    {
        return -1;
    }

    LOG_ASSERT(start >= 0 && start < _length);
    LOG_ASSERT(start + length >= 0 && start + length <= _length);

    s32 index = -1;
    for (u32 i = start; i < (start + length) - (a._length - 1); i++)
    {
        bool found = true;
        for (u32 offset = 0; offset < a._length; offset++)
        {
            if (a._data[offset] != _data[i + offset])
            {
                found = false;
                break;
            }
        }

        if (found == false)
        {
            index = i;
            break;
        }
        else
        {
            i += (a._length - 1);
        }
    }

    return index;
}

s32 CString::LastIndexNotOf(const CString& a, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length - start;
    }
    if (_length == 0)
    {
        return -1;
    }

    LOG_ASSERT(start >= 0 && start < _length);
    LOG_ASSERT(start + length >= 0 && start + length <= _length);

    s32 index = -1;
    for (u32 i = ((start + length) - (a._length - 1)) - 1; i >= 0; i--)
    {
        bool found = true;
        for (u32 offset = 0; offset < a._length; offset++)
        {
            if (a._data[offset] != _data[i + offset])
            {
                found = false;
                break;
            }
        }

        if (found == false)
        {
            index = i;
            break;
        }
        else
        {
            i -= (a._length - 1);
        }

        // Fucking unsigned for loops.
        if (i == 0)
            break;
    }

    return index;
}

s32 CString::IndexNotOf(u8 a, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length - start;
    }
    if (_length == 0)
    {
        return -1;
    }

    LOG_ASSERT(start >= 0 && start < _length);
    LOG_ASSERT(start + length >= 0 && start + length <= _length);

    s32 index = -1;
    for (u32 i = start; i < start + length; i++)
    {
        if (_data[i] != a)
        {
            index = i;
            break;
        }
    }

    return index;
}

s32 CString::LastIndexNotOf(u8 b, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length - start;
    }
    if (_length == 0)
    {
        return -1;
    }

    LOG_ASSERT(start >= 0 && start < _length);
    LOG_ASSERT(start + length >= 0 && start + length <= _length);

    s32 index = -1;
    for (u32 i = (start + length) - 1; i >= start; i--)
    {
        if (_data[i] != b)
        {
            index = i;
            break;
        }
    }

    return index;
}

s32 CString::IndexNotOfAny(CArray<CString> delims, u32 start, u32 length, CString* deliminator)
{
    s32 offset = -1;
    u32 size = delims.Size();

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = IndexNotOf(delims[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
            if (deliminator != NULL)
            {
                *deliminator = delims[i];
            }
        }
    }

    return offset;
}

s32 CString::LastIndexNotOfAny(CArray<CString> delims, u32 start, u32 length, CString* deliminator)
{
    s32 offset = -1;
    u32 size = delims.Size();

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = LastIndexNotOf(delims[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
            if (deliminator != NULL)
            {
                *deliminator = delims[i];
            }
        }
    }

    return offset;
}

/*
s32 CString::IndexNotOfAny(CString a[], u32 size, u32 start, u32 length)
{
    s32 offset = -1;

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = IndexNotOf(a[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
        }
    }

    return offset;
}

s32 CString::LastIndexNotOfAny(CString b[], u32 size, u32 start, u32 length)
{
    s32 offset = -1;

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = LastIndexNotOf(b[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
        }
    }

    return offset;
}

s32 CString::IndexNotOfAny(u8 a[], u32 size, u32 start, u32 length)
{
    s32 offset = -1;

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = IndexNotOf(a[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
        }
    }

    return offset;
}

s32 CString::LastIndexNotOfAny(u8 b[], u32 size, u32 start, u32 length)
{
    s32 offset = -1;

    for (u32 i = 0; i < size; i++)
    {
        s32 thisOffset = LastIndexNotOf(b[i], start, length);
        if ((thisOffset < offset && thisOffset != -1) || offset == -1)
        {
            offset = thisOffset;
        }
    }

    return offset;
}
*/

CString CString::Remove(u32 start, u32 length)
{
    LOG_ASSERT(start >= 0 && start < _length);
    LOG_ASSERT(start + length >= 0 && start + length <= _length);

    CString str;
    str.Allocate(_length - length);

    if (start > 0)
        memcpy(str._data, _data, start);

    s32 len = _length - (start + length);
    if (len > 0)
        memcpy(str._data + start, _data + start + length, len);

    return str;
}

CString CString::Insert(const CString& a, u32 start)
{
    LOG_ASSERT(start >= 0 && start <= _length);
    LOG_ASSERT(a._length > 0);

    CString str;
    str.Allocate(_length + a._length);

    if (start > 0)
        memcpy(str._data, _data, start);

    memcpy(str._data + start, a._data, a._length);

    if (start <= _length - 1)
        memcpy(str._data + start + a._length, _data + start, (_length - start));

    return str;
}

s32 CString::Count(const CString& a, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length;
    }
	if (length == 0)
	{
		return 0;
	}
	if (_length < a.Length())
	{
		return 0;
	}

    LOG_ASSERT(start >= 0 && start + length <= _length);

    s32 count = 0;

    for (u32 i = start; i < (start + length) - (a._length - 1); i++)
    {
        bool found = true;
        for (u32 offset = 0; offset < a._length; offset++)
        {
            if (a._data[offset] != _data[i + offset])
            {
                found = false;
                break;
            }
        }

        if (found == true)
        {
            count++;
            i += (a._length - 1);
        }
    }

    return count;
}

s32 CString::Count(u8 a, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length;
    }

    LOG_ASSERT(start >= 0 && start + length <= _length);

    s32 count = 0;

    for (u32 i = start; i < start + length; i++)
    {
        if (_data[i] == a)
        {
            count++;
        }
    }

    return count;
}

CString CString::Replace(const CString& a, const CString& b, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length;
    }

    LOG_ASSERT(start >= 0 && start + length <= _length);

    // Count how many times string occurs so we can work out how to replace.
    s32 occurances = Count(a);
	if (occurances == 0)
	{
		return *this;
	}

    CString str;
    str.Allocate((_length - (occurances * a._length)) + (occurances * b._length));

    s32 strOffset = 0;

    u32 end = (start + length) - (a._length - 1);
    for (u32 i = 0; i < _length; i++)
    {
        bool found = false;
        if (i >= start && i < end)
        {
            found = true;

            for (u32 offset = 0; offset < a._length; offset++)
            {
                if (a._data[offset] != _data[i + offset])
                {
                    found = false;
                    break;
                }
            }
        }

        if (found == true)
        {
            memcpy(str._data + strOffset, b._data, b._length);
            strOffset += b._length;
            i += (a._length - 1);
        }
        else
        {
            str._data[strOffset++] = _data[i];
        }
    }

    return str;
}

CString CString::Replace(u8 a, u8 b, u32 start, u32 length)
{
    if (length == 0)
    {
        length = _length;
    }

    LOG_ASSERT(start >= 0 && start + length <= _length);

    CString str;
    str.Allocate(_length);

    for (u32 i = start; i < start + length; i++)
    {
        str._data[i] = (_data[i] == a ? b : _data[i]);
    }

    return str;
}

CString CString::Limit(u32 length)
{
    if (length <= _length)
    {
        return CString(*this);
    }
    else
    {
        CString str;
        str.Allocate(length);

        if (length > 0)
        {
            memcpy(str._data, _data, length);
        }

        return str;
    }
}

bool CString::IsAlphaNumeric()
{
    bool valid = true;
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (!isalpha(c) && !isdigit(c))
        {
            valid = false;
            break;
        }
    }
    return valid;
}

bool CString::IsAlpha()
{
    bool valid = true;
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (!isalpha(c))
        {
            valid = false;
            break;
        }
    }
    return valid;
}

bool CString::IsNumeric()
{
    bool valid = true;
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (!isdigit(c))
        {
            valid = false;
            break;
        }
    }
    return valid;
}

bool CString::IsUppercase()
{
    bool valid = true;
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (!isupper(c))
        {
            valid = false;
            break;
        }
    }
    return valid;
}

bool CString::IsLowercase()
{
    bool valid = true;
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (!islower(c))
        {
            valid = false;
            break;
        }
    }
    return valid;
}

bool CString::IsWhitespace()
{
    bool valid = true;
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (!isspace(c))
        {
            valid = false;
            break;
        }
    }
    return valid;
}

bool CString::IsHex()
{
    bool valid = true;
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (!isxdigit(c))
        {
            valid = false;
            break;
        }
    }
    return valid;
}

bool CString::HasAlpha()
{
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (isalpha(c))
        {
            return true;
        }
    }
    return false;
}

bool CString::HasNumeric()
{
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (isdigit(c))
        {
            return true;
        }
    }
    return false;
}

bool CString::HasPunctuation()
{
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (ispunct(c))
        {
            return true;
        }
    }
    return false;
}

bool CString::HasUppercase()
{
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (isupper(c))
        {
            return true;
        }
    }
    return false;
}

bool CString::HasLowercase()
{
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (islower(c))
        {
            return true;
        }
    }
    return false;
}

bool CString::HasWhitespace()
{
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (isspace(c))
        {
            return true;
        }
    }
    return false;
}

bool CString::HasHex()
{
    for (u32 i = 0; i < _length; i++)
    {
        u8 c = _data[i];
        if (isxdigit(c))
        {
            return true;
        }
    }
    return false;
}

CString CString::FormatString(CString a, ...)
{
    va_list va;
    va_start(va, a);

    CString out = FormatStringVA(a, va);

    va_end(va);
    return out;
}

CString CString::FormatStringVA(CString a, va_list& va)
{
	// We use vsnprintf rather than vsnprintf_s because
	//	a) its cross platform.
	//	b) microsofts implementation does NOT return the output length for some dumbass reason, just gives a nice fat 0.
    u32 size = vsnprintf(NULL, NULL, a._data, va);

	CString out;
    out.Allocate(size + 1);

    vsnprintf(out._data, size, a._data, va);

    out._length = size;
    out._data[size] = '\0';

    return out;
}

CArray<CString> CString::Split(CArray<CString> delims, u32 maxSplits, bool ignoreDuplicates)
{
    CArray<CString> splits;
    CString deliminator;

    // Split every possible part!
    s32 delimIndex = 0;
    s32 len = _length;
    do
    {
        s32 oldIndex = delimIndex;
        delimIndex = IndexOfAny(delims, delimIndex, 0, &deliminator);

        if (delimIndex >= 0)
        {
            if (splits.Size() < maxSplits - 1 || delimIndex + (s32)deliminator._length >= len)
            {
                s32 l = delimIndex - oldIndex;
                if (l > 0)
                {
                    CString s = SubString(oldIndex, l);
                    if (ignoreDuplicates == false || splits.Size() <= 0 || (splits.Size() > 0 && splits[splits.Size() - 1] != s))
                    {
                        splits.AddToEnd(s);
                    }
                }
                else
                {
                    splits.AddToEnd(S(" "));
                }

                delimIndex += (s32)deliminator._length;
            }
            else
            {
                delimIndex = oldIndex;
                break;
            }
        }
        else
        {
            delimIndex = oldIndex;
            break;
        }

    } while (delimIndex >= 0 && delimIndex < len);

    // Add the final segment.
    if (delimIndex >= 0 && delimIndex < len)
    {
        CString s = SubString(delimIndex);
        if (ignoreDuplicates == false || s != "")
        {
            splits.AddToEnd(s);
        }
    }

    return splits;
}

Engine::Containers::CArray<CString> CString::Split(u8 delim, u32 maxSplits, bool ignoreDuplicates)
{
    CArray<CString> splits;

    // Split every possible part!
    s32 delimIndex = 0;
    s32 len = _length;
    do
    {
        s32 oldIndex = delimIndex;
        delimIndex = IndexOf(delim, delimIndex, 0);

        if (delimIndex >= 0)
        {
            if (splits.Size() < maxSplits - 1 || delimIndex + 1 >= len)
            {
                s32 l = delimIndex - oldIndex;
                if (l > 0)
                {
                    CString s = SubString(oldIndex, l);
                    if (ignoreDuplicates == false || splits.Size() <= 0 || (splits.Size() > 0 && splits[splits.Size() - 1] != s))
                    {
                        splits.AddToEnd(s);
                    }
                }
                else
                {
                    splits.AddToEnd(S(""));
                }

                delimIndex += (s32)1;
            }
            else
            {
                delimIndex = oldIndex;
                break;
            }
        }
        else
        {
            delimIndex = oldIndex;
            break;
        }

    } while (delimIndex >= 0 && delimIndex < len);

    // Add the final segment.
    if (delimIndex >= 0 && delimIndex < len)
    {
        CString s = SubString(delimIndex);
        if (ignoreDuplicates == false || s != "")
        {
            splits.AddToEnd(s);
        }
    }

    return splits;
}

/*
CArray<CString> CString::Split(const CString& delims, u32 maxSplits, bool treatAsChars, bool ignoreDuplicates)
{
    CArray<CString> splits;


    return splits;
}
*/

/*
CArray<CString> CString::Split(u8 delims[], u32 delimCount, u32 maxSplits, bool treatAsChars, bool ignoreDuplicates)
{
    CArray<CString> splits;

    return splits;
}

CString CString::Join(CString a[], u32 size)
{
    CString str;

    for (u32 i = 0; i < size; i++)
    {
        str += a[i];
        if (i < size - 1)
        {
            str += *this;
        }
    }

    return str;
}

CString CString::Join(u8 a[], u32 size)
{
    CString str;

    for (u32 i = 0; i < size; i++)
    {
        str += a[i];
        if (i < size - 1)
        {
            str += *this;
        }
    }

    return str;
}
*/

CString CString::Join(CArray<CString> bits)
{
    CString str;
    u32 size = bits.Size();

    for (u32 i = 0; i < size; i++)
    {
        str += bits[i];
        if (i < size - 1)
        {
            str += *this;
        }
    }

    return str;
}
