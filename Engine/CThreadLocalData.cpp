///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CThreadLocalData.h"
#include "CLog.h"

using namespace Engine::Threading;

CThreadLocalData::CThreadLocalData()
{
	if (!Engine::Platform::ThreadLocalDataCreate(&_tldHandle))
		LOG_ASSERT(false);
}

CThreadLocalData::CThreadLocalData(void* value)
{
	if (!Engine::Platform::ThreadLocalDataCreate(&_tldHandle))
		LOG_ASSERT(false);

	Set(value);
}

CThreadLocalData::~CThreadLocalData()
{
	Engine::Platform::ThreadLocalDataDelete(&_tldHandle);
}
				
CThreadLocalData& CThreadLocalData::operator=(void* &rhs)
{
	Set(rhs);
	return *this;
}

void* CThreadLocalData::operator()()
{
	return Get();
}

void CThreadLocalData::Set(void* ptr)
{
	Engine::Platform::ThreadLocalDataSet(&_tldHandle, ptr);
}

void* CThreadLocalData::Get()
{
	return Engine::Platform::ThreadLocalDataGet(&_tldHandle);
}