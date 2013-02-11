///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CSemaphore.h"
#include "CLog.h"

using namespace Engine::Threading;

CSemaphore::CSemaphore()
{
	if (!Engine::Platform::SemaphoreCreate(&_semaphoreHandle))
		LOG_ASSERT(false);
}

CSemaphore::~CSemaphore()
{
	Engine::Platform::SemaphoreDelete(&_semaphoreHandle);
}

void CSemaphore::Wait(u32 timeout)
{
	Engine::Platform::SemaphoreWait(&_semaphoreHandle, timeout);
}

void CSemaphore::Post()
{
	Engine::Platform::SemaphorePost(&_semaphoreHandle);
}