///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CThread.h"
#include "CLog.h"

using namespace Engine::Threading;

CThread::CThread(const Engine::Containers::CString& name, Engine::Platform::THREAD_ENTRY_FUNCTION entry, void* args)
{
	if (!Engine::Platform::ThreadSpawn(&_threadHandle, name, entry, args))
		LOG_ASSERT(false);
}

CThread::~CThread()
{
	Engine::Platform::ThreadDelete(&_threadHandle);
}

void CThread::SetName(const Engine::Containers::CString& name)
{
	Engine::Platform::ThreadSetName(&_threadHandle, name);
}

Engine::Containers::CString CThread::GetName()
{
	return Engine::Platform::ThreadGetName(&_threadHandle);
}

void CThread::Kill(s32 exitcode)
{
	Engine::Platform::ThreadKill(&_threadHandle, exitcode);
}

void CThread::Resume()
{
	Engine::Platform::ThreadResume(&_threadHandle);
}

void CThread::Pause()
{
	Engine::Platform::ThreadPause(&_threadHandle);
}

bool CThread::Wait(s32 timeout)
{
	return Engine::Platform::ThreadWait(&_threadHandle, timeout);
}

bool CThread::IsRunning()
{
	return Engine::Platform::ThreadRunning(&_threadHandle);
}

void CThread::SetPriority(Engine::Platform::Priority priority)
{
	Engine::Platform::ThreadSetPriority(&_threadHandle, priority);
}

Engine::Platform::Priority CThread::GetPriority()
{
	return Engine::Platform::ThreadGetPriority(&_threadHandle);
}

void CThread::SetAffinity(u32 affinity)
{
	Engine::Platform::ThreadSetAffinity(&_threadHandle, affinity);
}

u32	CThread::GetAffinity()
{
	return Engine::Platform::ThreadGetAffinity(&_threadHandle);
}

s32 CThread::GetExitCode()
{
	return Engine::Platform::ThreadGetExitCode(&_threadHandle);
}