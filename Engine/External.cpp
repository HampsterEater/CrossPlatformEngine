///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "External.h"

Engine::Memory::Allocators::CProxyAllocator* Engine::External::g_external_allocator = NULL;

void Engine::External::InitExternalAllocator()
{
    Engine::Memory::Allocators::CAllocator* alloc = Engine::Memory::GetDefaultAllocator();
    Engine::External::g_external_allocator = alloc->NewObj<Engine::Memory::Allocators::CProxyAllocator>("External Allocator", alloc);
}

void Engine::External::FreeExternalAllocator()
{
    Engine::Memory::GetDefaultAllocator()->FreeObj(&Engine::External::g_external_allocator);
    Engine::External::g_external_allocator = NULL;
}

Engine::Memory::Allocators::CAllocator* Engine::External::GetExternalAllocator()
{
	return Engine::External::g_external_allocator;
}

bool Engine::External::InitializeLibs()
{
	InitExternalAllocator();
	return true;
}

bool Engine::External::DeinitializeLibs()
{
	FreeExternalAllocator();
	return true;
}