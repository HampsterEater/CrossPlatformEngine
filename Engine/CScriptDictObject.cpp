///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptCompileContext.h"
#include "CScriptManager.h"
#include "CScriptDictObject.h"
#include "CScriptStringObject.h"

#include "CScriptVirtualMachine.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Objects;

CScriptDictIteratorObject::CScriptDictIteratorObject(CScriptExecutionContext* context, CScriptObject* obj)
{
	_index = 0;
	_obj = obj;
	_obj->IncRef();
}

bool CScriptDictIteratorObject::IsFinished(CScriptExecutionContext* context)
{
	CScriptDictObject* obj = reinterpret_cast<CScriptDictObject*>(_obj);
	return (_index >= obj->GetKeys().Size());
}

CScriptValue CScriptDictIteratorObject::NextValue(CScriptExecutionContext* context)
{
	CScriptDictObject* obj = reinterpret_cast<CScriptDictObject*>(_obj);
	CScriptValue& val = obj->GetKeys()[_index++];
	return val;
}

CScriptDictObject::CScriptDictObject(CScriptExecutionContext* context)
{

}

void CScriptDictObject::Finalize(CScriptExecutionContext* context)
{
	for (u32 i = 0; i < _keys.Size(); i++)
	{
		CScriptValue& val = _keys[i];
		if (val.Type == SCRIPT_VALUE_TYPE_OBJECT && val.Object != NULL)
			val.Object->DecRef();
	}
	for (u32 i = 0; i < _values.Size(); i++)
	{
		CScriptValue& val = _values[i];
		if (val.Type == SCRIPT_VALUE_TYPE_OBJECT && val.Object != NULL)
			val.Object->DecRef();
	}

	_keys.Clear();
	_values.Clear();

	_finalized = true;
}

// Metadata.
Engine::Containers::CString	CScriptDictObject::GetName()
{
	return "dict";
}

// Iteration.
CScriptIteratorObject* CScriptDictObject::CreateIterator(CScriptExecutionContext* context)
{
	CScriptDictIteratorObject* iter = Engine::Scripting::GetScriptAllocator()->NewObj<CScriptDictIteratorObject>(context, this);
	context->GCAdd(iter);

	return iter;
}

// Array stuff.
Engine::Containers::CArray<CScriptValue>& CScriptDictObject::GetKeys()
{
	return _keys;
}

Engine::Containers::CArray<CScriptValue>& CScriptDictObject::GetValues()
{
	return _values;
}

s32 CScriptDictObject::GetIndex(CScriptExecutionContext* context, const CScriptValue& key)
{
	CScriptValue k = key;

	for (u32 i = 0; i < _keys.Size(); i++)
	{
		if (context->CompareScriptValues(k, _keys[i]))
		{
			return i;
		}
	}
	return -1;
}

void CScriptDictObject::AddItem(CScriptExecutionContext* context, const CScriptValue& keyInternal, const CScriptValue& valInternal)
{
	CScriptValue key   = keyInternal;
	CScriptValue value = valInternal;

	// Check key is not a duplicate.
	if (GetIndex(context, key) >= 0)
	{
		CScriptValue v;
		v.Type = SCRIPT_VALUE_TYPE_OBJECT;
		v.Object = this;

		context->DuplicateIndex(v, key);
		return;
	}

	// Add key.
	_keys.AddToEnd(key);
 	if (key.Type == SCRIPT_VALUE_TYPE_OBJECT && key.Object != NULL)
		key.Object->IncRef();

	// Add value.
	_values.AddToEnd(value);
 	if (value.Type == SCRIPT_VALUE_TYPE_OBJECT && value.Object != NULL)
		value.Object->IncRef();
}

// Casting.
bool CScriptDictObject::CoerceToInt(CScriptExecutionContext* context, s32& result)
{
	return false;
}

bool CScriptDictObject::CoerceToFloat(CScriptExecutionContext* context, f32& result)
{
	return false;
}

bool CScriptDictObject::CoerceToString(CScriptExecutionContext* context, Engine::Containers::CString& result)
{
	Engine::Containers::CString output = "{ ";
	
	for (u32 i = 0; i < _keys.Size(); i++)
	{
		CScriptValue&				key = _keys[i];
		CScriptValue&				val = _values[i];
		Engine::Containers::CString keyStr = context->CoerceValueToString(key);
		Engine::Containers::CString valStr = context->CoerceValueToString(val);

		if (val.Type == SCRIPT_VALUE_TYPE_OBJECT && typeid(*val.Object) == typeid(CScriptStringObject))
			keyStr = "\"" + keyStr + "\"";
		if (val.Type == SCRIPT_VALUE_TYPE_OBJECT && typeid(*val.Object) == typeid(CScriptStringObject))
			valStr = "\"" + valStr + "\"";
		
		output += keyStr + ":" + valStr;

		if (i < _keys.Size() - 1)
			output += ", ";
	}

	output += " }";

	result = output;

	return true;
}

bool CScriptDictObject::CoerceToBool(CScriptExecutionContext* context, bool& result)
{
	result = true;
	return true;
}

// Subscript operations.
bool CScriptDictObject::GetIndex(CScriptExecutionContext* context, CScriptValue& dest, const CScriptValue& index)
{
	s32 realIndex = GetIndex(context, index);

	if (realIndex < 0 || realIndex >= (s32)_keys.Size())
	{
		context->InvalidIndex(dest, index);
		return false;
	}

	dest = _values[realIndex];

	return true;
}

bool CScriptDictObject::SetIndex(CScriptExecutionContext* context, const CScriptValue& dest, const CScriptValue& index, const CScriptValue& value)
{
	s32 realIndex = GetIndex(context, index);

	if (realIndex < 0 || realIndex >= (s32)_keys.Size())
	{
		context->InvalidIndex(dest, index);
		return false;
	}

	// Update reference of old object.
 	if (_values[realIndex].Type == SCRIPT_VALUE_TYPE_OBJECT && _values[realIndex].Object != NULL)
		_values[realIndex].Object->DecRef();

	// Set the ne value.
	_values[realIndex] = value;

	// Update reference of new object.
 	if (_values[realIndex].Type == SCRIPT_VALUE_TYPE_OBJECT && _values[realIndex].Object != NULL)
		_values[realIndex].Object->IncRef();

	return true;
}

// Indirection operations.
bool CScriptDictObject::GetAttribute(CScriptExecutionContext* context, CScriptValue& dest, const Engine::Containers::CString& name)
{
	return false;
}

bool CScriptDictObject::SetAttribute(CScriptExecutionContext* context, const Engine::Containers::CString& name, const CScriptValue& value)
{
	return false;
}
