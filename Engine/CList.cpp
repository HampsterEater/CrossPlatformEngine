///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CList.h"

using namespace Engine::Containers;

Engine::Memory::Allocators::CProxyAllocator* Engine::Containers::g_list_allocator = NULL;

void Engine::Containers::InitListAllocator()
{
    Engine::Memory::Allocators::CAllocator* alloc = Engine::Memory::GetDefaultAllocator();
    Engine::Containers::g_list_allocator = alloc->NewObj<Engine::Memory::Allocators::CProxyAllocator>("List Allocator", alloc);
}

void Engine::Containers::FreeListAllocator()
{
    Engine::Memory::GetDefaultAllocator()->FreeObj(&Engine::Containers::g_list_allocator);
    Engine::Containers::g_list_allocator = NULL;
}

