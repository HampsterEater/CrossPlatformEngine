///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CArray.h"

using namespace Engine::Containers;

Engine::Memory::Allocators::CProxyAllocator* Engine::Containers::g_array_allocator = NULL;

void Engine::Containers::InitArrayAllocator()
{
    Engine::Memory::Allocators::CAllocator* alloc = Engine::Memory::GetDefaultAllocator();
    Engine::Containers::g_array_allocator = alloc->NewObj<Engine::Memory::Allocators::CProxyAllocator>("Array Allocator", alloc);
}

void Engine::Containers::FreeArrayAllocator()
{
    Engine::Memory::GetDefaultAllocator()->FreeObj(&Engine::Containers::g_array_allocator);
    Engine::Containers::g_array_allocator = NULL;
}


