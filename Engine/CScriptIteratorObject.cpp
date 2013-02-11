///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptCompileContext.h"
#include "CScriptManager.h"
#include "CScriptIteratorObject.h"
#include "CScriptStringObject.h"

#include "CScriptVirtualMachine.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Objects;

CScriptIteratorObject::CScriptIteratorObject()
{
	_index = 0;
	_obj = NULL;
}

CScriptIteratorObject::CScriptIteratorObject(CScriptExecutionContext* context, CScriptObject* obj)
{
	_index = 0;
	_obj = obj;
	_obj->IncRef();
}

void CScriptIteratorObject::Finalize(CScriptExecutionContext* context)
{
	_finalized = true;
	if (_obj != NULL)
		_obj->DecRef();
}

// Metadata.
Engine::Containers::CString	CScriptIteratorObject::GetName()
{
	return "iterator";
}

// Casting.
bool CScriptIteratorObject::CoerceToString(CScriptExecutionContext* context, Engine::Containers::CString& result)
{
	result = "iterator";
	return true;
}
