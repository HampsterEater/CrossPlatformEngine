///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

namespace Engine
{
    namespace Platform
    {

		struct ThreadHandle 
		{
			HANDLE					_threadHandle;
			DWORD					_threadHandleId;
            Engine::Containers::CString  _threadName;

            friend bool operator==(ThreadHandle& a, ThreadHandle& b)
			{
				if (a._threadHandle == INVALID_HANDLE_VALUE || b._threadHandle == INVALID_HANDLE_VALUE)
					return a._threadHandleId == b._threadHandleId;
				else
					return a._threadHandle == b._threadHandle;
			}
		};
		
		struct MutexHandle
		{
			CRITICAL_SECTION		_mutexHandle;
		};

		struct SemaphoreHandle
		{
			HANDLE					_semaphoreHandle;

            friend bool operator==(SemaphoreHandle& a, SemaphoreHandle& b)
			{
				return a._semaphoreHandle == b._semaphoreHandle;
			}
		};

		struct ConditionVarHandle
		{
			CONDITION_VARIABLE		_conVarHandle;
		};
		
		struct ThreadLocalDataHandle
		{
			DWORD					_slot;
		};


    }
}
