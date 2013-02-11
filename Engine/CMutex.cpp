///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CMutex.h"
#include "CLog.h"

using namespace Engine::Threading;

CMutex::CMutex()
{
	if (!Engine::Platform::MutexCreate(&_mutexHandle))
		LOG_ASSERT(false);
}

CMutex::~CMutex()
{
	Engine::Platform::MutexDelete(&_mutexHandle);
}

void CMutex::Lock()
{
	Engine::Platform::MutexLock(&_mutexHandle);
}

void CMutex::Unlock()
{
	Engine::Platform::MutexUnlock(&_mutexHandle);
}

bool CMutex::TryLock()
{	
	return Engine::Platform::MutexTryLock(&_mutexHandle);
}