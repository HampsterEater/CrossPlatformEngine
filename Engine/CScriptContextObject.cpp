///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptCompileContext.h"
#include "CScriptManager.h"
#include "CScriptContextObject.h"
#include "CScriptStringObject.h"

#include "CScriptVirtualMachine.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Objects;

CScriptContextIteratorObject::CScriptContextIteratorObject(CScriptExecutionContext* context, CScriptObject* obj)
{
	_index = 0;
	_obj = obj;
	_obj->IncRef();
	_isFinished = false;

	CScriptContextObject* o = reinterpret_cast<CScriptContextObject*>(obj);
	_context = o->_context;
}

bool CScriptContextIteratorObject::IsFinished(CScriptExecutionContext* context)
{
	CScriptContextObject* obj = reinterpret_cast<CScriptContextObject*>(_obj);
	return _isFinished;
}

CScriptValue CScriptContextIteratorObject::NextValue(CScriptExecutionContext* context)
{
	CScriptValue nxt = _nextValue;
	CalcNext(context);
	return nxt;
}

void CScriptContextIteratorObject::CalcNext(CScriptExecutionContext* context)
{
	CScriptContextObject* obj = reinterpret_cast<CScriptContextObject*>(_obj);
	_context.GeneratorIterator = this;
	context->PushCallContext(_context);
}

void CScriptContextIteratorObject::CallbackComplete()
{
	_isFinished = true;
}

void CScriptContextIteratorObject::CallbackNext(CScriptCallContext& callContext, CScriptValue& value)
{
	_nextValue = value;
	_context   = callContext;
}

CScriptContextObject::CScriptContextObject(CScriptExecutionContext* context, CScriptCallContext& callContext)
{
	_context = callContext;
}

void CScriptContextObject::Finalize(CScriptExecutionContext* context)
{
	_context.Dispose();
	_finalized = true;
}

// Metadata.
Engine::Containers::CString	CScriptContextObject::GetName()
{
	return "context";
}

// Casting.
bool CScriptContextObject::CoerceToString(CScriptExecutionContext* context, Engine::Containers::CString& result)
{
	result = "context";
	return true;
}

// Iteration.
CScriptIteratorObject* CScriptContextObject::CreateIterator(CScriptExecutionContext* context)
{
	CScriptContextIteratorObject* iter = Engine::Scripting::GetScriptAllocator()->NewObj<CScriptContextIteratorObject>(context, this);
	context->GCAdd(iter);

	iter->CalcNext(context);

	return iter;
}