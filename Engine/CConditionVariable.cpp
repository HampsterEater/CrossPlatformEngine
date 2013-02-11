///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CConditionVariable.h"
#include "CLog.h"

using namespace Engine::Threading;

CConditionVariable::CConditionVariable()
{	
	if (!Engine::Platform::ConditionVarCreate(&_conVarHandle))
		LOG_ASSERT(false);
}

CConditionVariable::~CConditionVariable()
{
	Engine::Platform::ConditionVarDelete(&_conVarHandle);
}

void CConditionVariable::Broadcast()
{
	Engine::Platform::ConditionVarBroadcast(&_conVarHandle);
}

void CConditionVariable::Signal()
{
	Engine::Platform::ConditionVarSignal(&_conVarHandle);
}

bool CConditionVariable::Wait(Engine::Threading::CMutex* mutex, u32 timeout)
{
	return Engine::Platform::ConditionVarWait(&_conVarHandle, &mutex->_mutexHandle, timeout);
}