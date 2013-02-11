///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CBreakpoint.h"

using namespace Engine::Debug;

CBreakpoint::CBreakpoint()
{
	_initialized = false;
}

CBreakpoint::~CBreakpoint()
{
	if (_initialized == true)
		Dispose();
}

void CBreakpoint::Force()
{
	Engine::Platform::DebugBreak();
}

bool CBreakpoint::Initialize(void* ptr, u32 size, Engine::Platform::BreakpointMode flags)
{
	if (Engine::Platform::DebugCreateBreakpoint(&_handle, ptr, size, flags) == true)
	{
		_initialized = true;
		return true;
	}
	return false;
}

void CBreakpoint::Dispose()
{
	Engine::Platform::DebugDisposeBreakpoint(&_handle);
	_initialized = false;
}