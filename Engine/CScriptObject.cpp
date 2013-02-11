///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptCompileContext.h"
#include "CScriptManager.h"
#include "CScriptObject.h"

#include "CScriptVirtualMachine.h"
#include "CScriptIteratorObject.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Objects;

CScriptObject::CScriptObject()
{
	_refCount = 0;
	_finalized = false;
}

void CScriptObject::IncRef()
{
	_refCount++;
}

void CScriptObject::DecRef()
{
	_refCount--;
	//LOG_ASSERT(_refCount >= 0);
}

s32 CScriptObject::GetRefs()
{
	return _refCount;
}				

void CScriptObject::Finalize(CScriptExecutionContext* context)
{
	_finalized = true;
}

// Casting.
bool CScriptObject::CoerceToInt(CScriptExecutionContext* context, s32& result)
{
	return false;
}

bool CScriptObject::CoerceToFloat(CScriptExecutionContext* context, f32& result)
{
	return false;
}

bool CScriptObject::CoerceToString(CScriptExecutionContext* context, Engine::Containers::CString& result)
{
	return false;
}

bool CScriptObject::CoerceToBool(CScriptExecutionContext* context, bool& result)
{
	result = true;
	return true;
}

bool CScriptObject::CastTo(CScriptExecutionContext* context, CScriptValue& dest, const Engine::Containers::CString& type)
{
	if (type == "object")
	{
		return true;
	}

	return false;
}

bool CScriptObject::DerivedFrom(CScriptExecutionContext* context, const Engine::Containers::CString& type)
{
	if (type == "object")
	{
		return true;
	}

	return false;
}

// Iteration.
CScriptIteratorObject* CScriptObject::CreateIterator(CScriptExecutionContext* context)
{
	return NULL;
}

// General instructions.
bool CScriptObject::Assign(CScriptExecutionContext* context, CScriptValue& dest)
{
	dest.Type = SCRIPT_VALUE_TYPE_OBJECT;
	dest.Object = this;
	return true;
}

bool CScriptObject::Add(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	return false;
}

bool CScriptObject::Sub(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	return false;
}

bool CScriptObject::Mul(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	return false;
}

bool CScriptObject::Div(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	return false;
}

bool CScriptObject::Inc(CScriptExecutionContext* context, CScriptValue& lvalue)
{
	return false;
}

bool CScriptObject::Dec(CScriptExecutionContext* context, CScriptValue& lvalue)
{
	return false;
}

bool CScriptObject::Neg(CScriptExecutionContext* context, CScriptValue& lvalue)
{
	return false;
}

bool CScriptObject::Abs(CScriptExecutionContext* context, CScriptValue& lvalue)
{
	return false;
}

bool CScriptObject::Mod(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	return false;
}

bool CScriptObject::BWOr(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	return false;
}

bool CScriptObject::BWXor(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	return false;
}

bool CScriptObject::BWAnd(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	return false;
}

bool CScriptObject::BWShl(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	return false;
}

bool CScriptObject::BWShr(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue)
{
	return false;
}

bool CScriptObject::BWNot(CScriptExecutionContext* context, CScriptValue& lvalue)
{
	return false;
}

// Comparison.
bool CScriptObject::Cmp(CScriptExecutionContext* context, const CScriptValue& dest, s32& result)
{
	result = (dest.Type == SCRIPT_VALUE_TYPE_OBJECT && dest.Object == this) ? 1 : 0;
	return true;
}

// Subscript operations.
bool CScriptObject::GetIndex(CScriptExecutionContext* context, CScriptValue& dest, const CScriptValue& index)
{
	return false;
}

bool CScriptObject::SetIndex(CScriptExecutionContext* context, const CScriptValue& dest, const CScriptValue& index, const CScriptValue& value)
{
	return false;
}

// Invokation operations.
bool CScriptObject::Invoke(CScriptExecutionContext* context, u32 parameterCount)
{
	return false;
}

// Indirection operations.
bool CScriptObject::GetAttribute(CScriptExecutionContext* context, CScriptValue& dest, const Engine::Containers::CString& name)
{
	return false;
}

bool CScriptObject::SetAttribute(CScriptExecutionContext* context, const Engine::Containers::CString& name, const CScriptValue& value)
{
	return false;
}
