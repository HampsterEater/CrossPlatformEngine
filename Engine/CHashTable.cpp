///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CHashTable.h"

using namespace Engine::Containers;

Engine::Memory::Allocators::CProxyAllocator* Engine::Containers::g_hashtable_allocator = NULL;

void Engine::Containers::InitHashTableAllocator()
{
    Engine::Memory::Allocators::CAllocator* alloc = Engine::Memory::GetDefaultAllocator();
    Engine::Containers::g_hashtable_allocator = alloc->NewObj<Engine::Memory::Allocators::CProxyAllocator>("HashTable Allocator", alloc);
}

void Engine::Containers::FreeHashTableAllocator()
{
    Engine::Memory::GetDefaultAllocator()->FreeObj(&Engine::Containers::g_hashtable_allocator);
    Engine::Containers::g_hashtable_allocator = NULL;
}


