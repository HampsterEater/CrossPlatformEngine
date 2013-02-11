///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include <new>
#include <stdio.h>

#include "CLog.h"

#include "Memory.h"
#include "CHeapAllocator.h"

// Define our base allocator.
Engine::Memory::Allocators::CHeapAllocator* Engine::Memory::g_default_allocator       = NULL;

// Initializes the default allocator!
void Engine::Memory::InitDefaultAllocator(u32 start_memory, u32 max_memory, u32 memory_chunk_size)
{
    if (Engine::Memory::g_default_allocator != NULL)
        throw "Default allocator already initialized!";

	u32 alloc_size = sizeof(Engine::Memory::Allocators::CHeapAllocator);
    void* allocatorMemoryBlock = Engine::Platform::MemoryAlloc(alloc_size);

#ifdef MEMORY_INIT_ZERO
	memset(allocatorMemoryBlock, 0, alloc_size);
#endif

    Engine::Memory::g_default_allocator = new (allocatorMemoryBlock) Engine::Memory::Allocators::CHeapAllocator("Heap Allocator", start_memory, max_memory, memory_chunk_size);
}

// Deallocates the default allocator.
void Engine::Memory::FreeDefaultAllocator()
{
    if (Engine::Memory::g_default_allocator == NULL)
        throw "Default allocator not initialized!";

    Engine::Memory::g_default_allocator->~CHeapAllocator();
    Engine::Platform::MemoryFree(Engine::Memory::g_default_allocator);
    Engine::Memory::g_default_allocator = NULL;
}

// Get a pointer to the default allocator!
Engine::Memory::Allocators::CAllocator* Engine::Memory::GetDefaultAllocator()
{
    return (Engine::Memory::Allocators::CAllocator*)Engine::Memory::g_default_allocator;
}

// Free, malloc, new and delete are all overridden here
// to make sure nobody is tempted to use them ;).
/*void* my_malloc(size_t size)
{
    LOG_ASSERT(false);
	return NULL;
}
void my_free(void * ptr)
{
    LOG_ASSERT(false);
}*/
void* operator new(size_t size)
{
    LOG_ASSERT(false);
	return NULL;
}
void operator delete(void *p)
{
    LOG_ASSERT(false);
}
void* operator new[](size_t size)
{
    LOG_ASSERT(false);
	return NULL;
}
void operator delete[](void *p)
{
    LOG_ASSERT(false);
}