///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptCompileContext.h"
#include "CScriptManager.h"
#include "CScriptStringObject.h"
#include "CScriptListObject.h"

#include "CScriptVirtualMachine.h"

#include <stdio.h>

#include <cstring>
#include <stdio.h>
#include <cctype>
#include <cstdlib>
#include <cassert>

using namespace Engine::Scripting;
using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Objects;

CScriptStringObject::CScriptStringObject(CScriptExecutionContext* context, const Engine::Containers::CString& str)
{
	_string = str;
}

// Metadata.
Engine::Containers::CString	CScriptStringObject::GetName()
{
	return "string";
}

// Casting.
bool CScriptStringObject::CoerceToInt(CScriptExecutionContext* context, s32& result)
{
	result = _string.ToInt();
	return true;
}

bool CScriptStringObject::CoerceToFloat(CScriptExecutionContext* context, f32& result)
{
	result = _string.ToFloat();
	return true;
}

bool CScriptStringObject::CoerceToString(CScriptExecutionContext* context, Engine::Containers::CString& result)
{
	result = _string;
	return true;
}

bool CScriptStringObject::CoerceToBool(CScriptExecutionContext* context, bool& result)
{
	result = (_string != "");
	return true;
}

// General instructions.
bool CScriptStringObject::Assign(CScriptExecutionContext* context, CScriptValue& dest)
{
	dest.Type = SCRIPT_VALUE_TYPE_OBJECT;
	dest.Object = GetScriptAllocator()->NewObj<CScriptStringObject>(context, _string);
	context->GCAdd(dest.Object);

	return true;
}

bool CScriptStringObject::Add(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	Engine::Containers::CString str = "";
	if (!rvalue.Object->CoerceToString(context, str))
		return false;

	// Special casting is done with strings so we can assume the dest value will be a string as well.;
	lvalue.Type = SCRIPT_VALUE_TYPE_OBJECT;
	lvalue.Object = GetScriptAllocator()->NewObj<CScriptStringObject>(context,  _string + str);
	context->GCAdd(lvalue.Object);

	return true;
}

bool CScriptStringObject::Mul(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{	
	s32 val = context->CoerceValueToInt(rvalue);

	Engine::Containers::CString str = "";
	while (val > 0)
	{
		str += _string;
		val--;
	}

	lvalue.Type = SCRIPT_VALUE_TYPE_OBJECT;
	lvalue.Object = GetScriptAllocator()->NewObj<CScriptStringObject>(context, str);
	context->GCAdd(lvalue.Object);

	return true;
}

bool CScriptStringObject::Mod(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{	
	if (rvalue.Type != SCRIPT_VALUE_TYPE_OBJECT ||
		rvalue.Object == NULL ||
		dynamic_cast<CScriptListObject*>(rvalue.Object) == NULL)
	{
		return false;
	}

	CScriptListObject* list = dynamic_cast<CScriptListObject*>(rvalue.Object);

	// Format the output.
	s32 formatIndex = -1;
	Engine::Containers::CString output = "";

	for (u32 i = 0; i < _string.Length(); i++)
	{
		u8 chr = _string[i];
		if (chr == '%' && i < _string.Length() - 1)
		{
			if (_string[i + 1] != '%')
			{
				Engine::Containers::CString format = "";
				while (i < _string.Length())
				{
					u8 formatChr = _string[i++];
					format += formatChr;

					if (formatChr == 'c' || formatChr == 'd' || formatChr == 'i' || formatChr == 'e' || formatChr == 'E' ||
						formatChr == 'f' || formatChr == 'g' || formatChr == 'G' || formatChr == 'o' || formatChr == 's' ||
						formatChr == 'u' || formatChr == 'x' || formatChr == 'X' || formatChr == 'p' || formatChr == 'n')
					{
						i--;
						break;
					}
				}

				u8 formatType = format[format.Length() - 1];
				Engine::Containers::CString formatOutput = "";

				// Get the value from the list.
				formatIndex++;
				if (formatIndex < (s32)list->GetArray().Size())
				{
					CScriptValue& listVal = list->GetArray()[formatIndex];
					
					// Char
					if (formatType == 'c')
					{
						Engine::Containers::CString val = context->CoerceValueToString(listVal);
						u8 valChr = val.Length() <= 0 ? ' ' : val[0];

						u32 size = snprintf(NULL, NULL, format.c_str(), valChr);
	
						u8* buffer = (u8*)GetScriptAllocator()->Alloc(size + 1);
						snprintf(buffer, size, format.c_str(), valChr);

						buffer[size] = '\0';
						formatOutput = buffer;

						GetScriptAllocator()->Free(&buffer);
					}
				
					// String
					else if (formatType == 's')
					{
						Engine::Containers::CString val = context->CoerceValueToString(listVal);

						u32 size = snprintf(NULL, NULL, format.c_str(), val.c_str());
	
						u8* buffer = (u8*)GetScriptAllocator()->Alloc(size + 1);
						snprintf(buffer, size, format.c_str(), val.c_str());

						buffer[size] = '\0';
						formatOutput = buffer;

						GetScriptAllocator()->Free(&buffer);
					}

					// Float
					else if (formatType == 'f')
					{
						f32 val = context->CoerceValueToFloat(listVal);

						u32 size = snprintf(NULL, NULL, format.c_str(), val);
	
						u8* buffer = (u8*)GetScriptAllocator()->Alloc(size + 1);
						snprintf(buffer, size, format.c_str(), val);

						buffer[size] = '\0';
						formatOutput = buffer;

						GetScriptAllocator()->Free(&buffer);
					}

					// Integer
					else
					{
						s32 val = context->CoerceValueToInt(listVal);

						u32 size = snprintf(NULL, NULL, format.c_str(), val);
	
						u8* buffer = (u8*)GetScriptAllocator()->Alloc(size + 1);
						snprintf(buffer, size, format.c_str(), val);

						buffer[size] = '\0';
						formatOutput = buffer;

						GetScriptAllocator()->Free(&buffer);
					}

				}
				else
				{
					formatOutput = format;
				}

				output += formatOutput;
			}
			else
			{
				output += "%";
				i++;
			}
		}
		else
		{
			output += chr;
		}
	}

	// Return the output string.
	lvalue.Type = SCRIPT_VALUE_TYPE_OBJECT;
	lvalue.Object = GetScriptAllocator()->NewObj<CScriptStringObject>(context, output);
	context->GCAdd(lvalue.Object);

	return true;
}


// Comparison.
bool CScriptStringObject::Cmp(CScriptExecutionContext* context, const CScriptValue& dest, s32& result)
{
	Engine::Containers::CString str = context->CoerceValueToString(dest);
	result = (_string == str);
	return true;
}

// Subscript operations.
bool CScriptStringObject::GetIndex(CScriptExecutionContext* context, CScriptValue& dest, const CScriptValue& index)
{
	s32 realIndex = context->CoerceValueToInt(index);

	if (realIndex < 0 || realIndex >= (s32)_string.Length())
		context->InvalidIndex(dest, realIndex);

	dest.Type = SCRIPT_VALUE_TYPE_OBJECT;
	dest.Object = GetScriptAllocator()->NewObj<CScriptStringObject>(context, _string[realIndex]);
	context->GCAdd(dest.Object);

	return true;
}

bool CScriptStringObject::SetIndex(CScriptExecutionContext* context, const CScriptValue& dest, const CScriptValue& index, const CScriptValue& value)
{
	context->ImmutableError(dest);
	return false;
}

// Indirection operations.
bool CScriptStringObject::GetAttribute(CScriptExecutionContext* context, CScriptValue& dest, const Engine::Containers::CString& name)
{
	return false;
}

bool CScriptStringObject::SetAttribute(CScriptExecutionContext* context, const Engine::Containers::CString& name, const CScriptValue& value)
{
	context->ImmutableError(GetName());
	return false;
}

