///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

namespace Engine
{
    namespace Platform
    {

		bool ThreadSpawn(ThreadHandle* handle, const Engine::Containers::CString& name, THREAD_ENTRY_FUNCTION entryPtr, void* args)
		{
			handle->_threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)entryPtr, args, CREATE_SUSPENDED, &handle->_threadHandleId);

            if (handle->_threadHandle ==  INVALID_HANDLE_VALUE)
            {
                u32 ec = GetLastError();
                LOG_ERROR(S("Failed to create thread, GetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));
                return false;
            }
			
			Win32SetVSThreadName(handle->_threadHandleId, name.c_str());
			handle->_threadName = name;

			return true;
		}

		void ThreadDelete(ThreadHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			CloseHandle(handle->_threadHandle);
			handle->_threadHandle = NULL;
		}

		void ThreadPause(ThreadHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			SuspendThread(handle->_threadHandle);
		}

        void ThreadResume(ThreadHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			ResumeThread(handle->_threadHandle);
		}
		
        void ThreadKill(ThreadHandle* handle, s32 exitcode)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			TerminateThread(handle->_threadHandle, exitcode);
		}

        bool ThreadWait(ThreadHandle* handle, s32 timeout)
		{
		    LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
		
			if (timeout == 0)
			{
				timeout = INFINITE;
			}

			return (WaitForSingleObject(handle->_threadHandle, timeout) == WAIT_OBJECT_0);
		}
		
		bool ThreadRunning(ThreadHandle* handle)
		{
		    LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
		
			DWORD exitCode;
			if (GetExitCodeThread(handle->_threadHandle, &exitCode))
			{
				if (exitCode == STILL_ACTIVE)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			return false;
		}

		void ThreadSetPriority(ThreadHandle* handle, Priority priority)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			SetThreadPriority(handle->_threadHandle, priority);
		}

		Priority ThreadGetPriority(ThreadHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			return (Priority)GetThreadPriority(handle->_threadHandle);
		}

		s32	ThreadGetExitCode(ThreadHandle* handle)
		{
			LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			
			DWORD exitCode = 0;
			LOG_ASSERT(GetExitCodeThread(&handle->_threadHandle, &exitCode));

			return (s32)exitCode;
		}
		
		bool ThreadSetAffinity(ThreadHandle* handle, s32 affinity)
		{
			LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			
			if (!SetThreadIdealProcessor(handle->_threadHandle, affinity))
			{
                u32 ec = GetLastError();
                LOG_ERROR(S("Failed to set thread affinity, GetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));
                return false;
			}

			return true;
		}

		s32 ThreadGetAffinity(ThreadHandle* handle)
		{
			LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			
			PROCESSOR_NUMBER procnum;
			if (!GetThreadIdealProcessorEx(handle->_threadHandle, &procnum))
			{
                u32 ec = GetLastError();
                LOG_ERROR(S("Failed to set thread affinity, GetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));
                return 0;
			}

			return procnum.Number;
		}

		void ThreadSetName(ThreadHandle* handle, const Engine::Containers::CString& name)
		{
			LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			
			Win32SetVSThreadName(handle->_threadHandleId, name.c_str());
			handle->_threadName = name;
		}

		Engine::Containers::CString ThreadGetName(ThreadHandle* handle)
		{
			LOG_ASSERT_MSG(handle != NULL && handle->_threadHandle != INVALID_HANDLE_VALUE, "Thread handle passed was NULL.");
			return handle->_threadName;
		}

		/*
		ThreadHandle ThreadMain()
		{
			ThreadHandle h;
			h._threadHandleId = g_main_thread_id;
			h._threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, h._threadHandleId); // TODO: Close handle.
			return h;
		}

		ThreadHandle ThreadCurrent()
		{
			ThreadHandle h;
			h._threadHandleId = GetCurrentThreadId();
			h._threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, h._threadHandleId); // TODO: Close handle. (Same with normal thread handle, ffs).
			return h;
		}*/

        bool MutexCreate(MutexHandle* handle)
		{
			InitializeCriticalSection(&handle->_mutexHandle);
			return true;
		}

        void MutexDelete(MutexHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL, "Mutex handle passed was NULL.");
			DeleteCriticalSection(&handle->_mutexHandle);
		}

        bool MutexTryLock(MutexHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL, "Mutex handle passed was NULL.");
			return (TryEnterCriticalSection(&handle->_mutexHandle) != FALSE);
		}

        void MutexLock(MutexHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL, "Mutex handle passed was NULL.");
			EnterCriticalSection(&handle->_mutexHandle);
		}

        void MutexUnlock(MutexHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL, "Mutex handle passed was NULL.");
			LeaveCriticalSection(&handle->_mutexHandle);
		}
		
        bool SemaphoreCreate(SemaphoreHandle* handle)
		{
			handle->_semaphoreHandle = CreateSemaphore(NULL, 0, 2147483647 /* Max value of signed int */, NULL);
			return true;
		}

        void SemaphoreDelete(SemaphoreHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_semaphoreHandle != INVALID_HANDLE_VALUE, "Semaphore handle passed was NULL.");
			CloseHandle(handle->_semaphoreHandle);
			handle->_semaphoreHandle = NULL;
		}
		
        bool SemaphoreWait(SemaphoreHandle* handle, s32 timeout)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_semaphoreHandle != INVALID_HANDLE_VALUE, "Semaphore handle passed was NULL.");
		
			if (timeout == 0)
			{
				timeout = INFINITE;
			}

			return (WaitForSingleObject(handle->_semaphoreHandle, timeout) == WAIT_OBJECT_0);
		}

        void SemaphorePost(SemaphoreHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_semaphoreHandle != INVALID_HANDLE_VALUE, "Semaphore handle passed was NULL.");
			ReleaseSemaphore(handle->_semaphoreHandle, 1, NULL);
		}

        bool ConditionVarCreate(ConditionVarHandle* handle)
		{
			InitializeConditionVariable(&handle->_conVarHandle);
			return true;
		}

		void ConditionVarDelete(ConditionVarHandle* handle)
		{
			// Nothing to do here.
		}

        void ConditionVarBroadcast(ConditionVarHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL, "Condition variable handle passed was NULL.");
			WakeAllConditionVariable(&handle->_conVarHandle);
		}

        void ConditionVarSignal(ConditionVarHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL, "Condition variable handle passed was NULL.");
			WakeConditionVariable(&handle->_conVarHandle);
		}

        bool ConditionVarWait(ConditionVarHandle* handle, MutexHandle* mutexHandle, s32 timeout)
		{
            LOG_ASSERT_MSG(handle != NULL, "Condition variable handle passed was NULL.");
            LOG_ASSERT_MSG(mutexHandle != NULL, "Mutex handle passed was NULL.");

			if (timeout == 0)
			{
				timeout = INFINITE;
			}

			return (SleepConditionVariableCS(&handle->_conVarHandle, &mutexHandle->_mutexHandle, timeout) != 0);
		}

        bool ThreadLocalDataCreate(ThreadLocalDataHandle* handle)
		{
			handle->_slot = TlsAlloc();
			return (handle->_slot == TLS_OUT_OF_INDEXES);
		}

        void ThreadLocalDataDelete(ThreadLocalDataHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_slot != TLS_OUT_OF_INDEXES, "Thread local data handle passed was NULL.");
			TlsFree(handle->_slot);
			handle->_slot = TLS_OUT_OF_INDEXES;
		}

        void ThreadLocalDataSet(ThreadLocalDataHandle* handle, void* ptr)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_slot != TLS_OUT_OF_INDEXES, "Thread local data handle passed was NULL.");
			TlsSetValue(handle->_slot, ptr);
		}

        void* ThreadLocalDataGet(ThreadLocalDataHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_slot != TLS_OUT_OF_INDEXES, "Thread local data handle passed was NULL.");
			return TlsGetValue(handle->_slot);
		}

        s32 AtomicAdd(s32* target, s32 value)
		{
			return InterlockedExchangeAdd((LONG*)target, (LONG)value);
		}

        s32 AtomicSwap(s32* target, s32 value)
		{
			return InterlockedExchange((LONG*)target, (LONG)value);
		}

        s32 AtomicCompareAndSwap(s32* target, s32 newValue, s32 compareValue)
		{
			return InterlockedCompareExchange((LONG*)target, (LONG)newValue, (LONG)compareValue);
		}


	}
}