///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptCompileContext.h"
#include "CScriptManager.h"
#include "CScriptListObject.h"
#include "CScriptStringObject.h"

#include "CScriptVirtualMachine.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Objects;

CScriptListIteratorObject::CScriptListIteratorObject(CScriptExecutionContext* context, CScriptObject* obj)
{
	_index = 0;
	_obj = obj;
	_obj->IncRef();
}

bool CScriptListIteratorObject::IsFinished(CScriptExecutionContext* context)
{
	CScriptListObject* obj = reinterpret_cast<CScriptListObject*>(_obj);
	return (_index >= obj->GetArray().Size());
}

CScriptValue CScriptListIteratorObject::NextValue(CScriptExecutionContext* context)
{
	CScriptListObject* obj = reinterpret_cast<CScriptListObject*>(_obj);
	CScriptValue& val = obj->GetArray()[_index++];
	return val;
}

CScriptListObject::CScriptListObject(CScriptExecutionContext* context)
{

}

void CScriptListObject::Finalize(CScriptExecutionContext* context)
{
	for (u32 i = 0; i < _array.Size(); i++)
	{
		CScriptValue& val = _array[i];
		if (val.Type == SCRIPT_VALUE_TYPE_OBJECT && val.Object != NULL)
			val.Object->DecRef();
	}
	_array.Clear();
	
	_finalized = true;
}

// Metadata.
Engine::Containers::CString	CScriptListObject::GetName()
{
	return "list";
}

// Array stuff.
Engine::Containers::CArray<CScriptValue>& CScriptListObject::GetArray()
{
	return _array;
}

void CScriptListObject::AddItem(CScriptExecutionContext* context, const CScriptValue& val)
{
	CScriptValue value = val;

	_array.AddToEnd(value);

 	if (value.Type == SCRIPT_VALUE_TYPE_OBJECT && value.Object != NULL)
		value.Object->IncRef();
}

// Iteration.
CScriptIteratorObject* CScriptListObject::CreateIterator(CScriptExecutionContext* context)
{
	CScriptListIteratorObject* iter = Engine::Scripting::GetScriptAllocator()->NewObj<CScriptListIteratorObject>(context, this);
	context->GCAdd(iter);

	return iter;
}

// Casting.
bool CScriptListObject::CoerceToInt(CScriptExecutionContext* context, s32& result)
{
	return false;
}

bool CScriptListObject::CoerceToFloat(CScriptExecutionContext* context, f32& result)
{
	return false;
}

bool CScriptListObject::CoerceToString(CScriptExecutionContext* context, Engine::Containers::CString& result)
{
	Engine::Containers::CString output = "[ ";
	
	for (u32 i = 0; i < _array.Size(); i++)
	{
		CScriptValue&				val = _array[i];
		Engine::Containers::CString str = context->CoerceValueToString(val);

		if (val.Type == SCRIPT_VALUE_TYPE_OBJECT && typeid(*val.Object) == typeid(CScriptStringObject))
			str = "\"" + str + "\"";

		output += str;

		if (i < _array.Size() - 1)
			output += ", ";
	}

	output += " ]";

	result = output;

	return true;
}

bool CScriptListObject::CoerceToBool(CScriptExecutionContext* context, bool& result)
{
	result = true;
	return true;
}

// Subscript operations.
bool CScriptListObject::GetIndex(CScriptExecutionContext* context, CScriptValue& dest, const CScriptValue& index)
{
	s32 realIndex = context->CoerceValueToInt(index);

	if (realIndex < 0 || realIndex >= (s32)_array.Size())
		context->InvalidIndex(dest, realIndex);

	dest = _array[realIndex];

	return true;
}

bool CScriptListObject::SetIndex(CScriptExecutionContext* context, const CScriptValue& dest, const CScriptValue& index, const CScriptValue& value)
{
	s32 realIndex = context->CoerceValueToInt(index);

	if (realIndex < 0 || realIndex >= (s32)_array.Size())
	{
		context->InvalidIndex(dest, realIndex);
		return false;
	}

	// Update reference of old object.
 	if (_array[realIndex].Type == SCRIPT_VALUE_TYPE_OBJECT && _array[realIndex].Object != NULL)
		_array[realIndex].Object->DecRef();

	// Set the ne value.
	_array[realIndex] = value;

	// Update reference of new object.
 	if (_array[realIndex].Type == SCRIPT_VALUE_TYPE_OBJECT && _array[realIndex].Object != NULL)
		_array[realIndex].Object->IncRef();

	return true;
}

// Indirection operations.
bool CScriptListObject::GetAttribute(CScriptExecutionContext* context, CScriptValue& dest, const Engine::Containers::CString& name)
{
	return false;
}

bool CScriptListObject::SetAttribute(CScriptExecutionContext* context, const Engine::Containers::CString& name, const CScriptValue& value)
{
	return false;
}
