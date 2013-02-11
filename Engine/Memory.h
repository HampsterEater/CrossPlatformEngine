///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Platform.h"
#include "CAllocator.h"
#include "CHeapAllocator.h"

// ----------------------------------------------------------------------------
// If defined then all allocators will print out leak information when 
// they are deallocated.
// ----------------------------------------------------------------------------
#ifdef DEBUG
#define MEMORY_DEBUG_LEAKS 
#endif

// ----------------------------------------------------------------------------
// If defined then a call stack will be generated and stored for each allocations.
// THIS IS SLOW, only enable it if your having real trouble detected leaks.
// ----------------------------------------------------------------------------
// Note: This changes the size of allocation blocks, this can cause differences
//		 in allocation patterns when on or off.
// ----------------------------------------------------------------------------
#ifdef DEBUG
#define MEMORY_TRACK_CALL_STACK 
#endif

// ----------------------------------------------------------------------------
// When combined with the above define it tracks call stacks using the fastest
// method available, rather than the most accurate.
// (So on windows, using CaptureStackBackTrace instead of StackWalk64).
// ----------------------------------------------------------------------------
#ifdef DEBUG
#define MEMORT_TRACK_CALL_STACK_FAST
#endif

// ----------------------------------------------------------------------------
// memset's the memory to 0 when its initially allocated from the OS if defined.
// This won't memset it if its reallocated though, just when its initially
// allocated from the OS.
// ----------------------------------------------------------------------------
#ifdef DEBUG
//#define MEMORY_INIT_ZERO
#endif

// ----------------------------------------------------------------------------
// If set then blocks are validated when added/removed from freechain.
// This is very slow, should only be done if you think something is wrong!
// ----------------------------------------------------------------------------
#ifdef DEBUG
//#define MEMORY_VALIDATE_BLOCKS
#endif

// ----------------------------------------------------------------------------
// This is the minimum left over data required for us to be able to split a 
// block. If this much can not be left over after a split, the entire block
// will be allocated to the callee.
// ----------------------------------------------------------------------------
#define MEMORY_MINIMUM_BLOCK_SPLIT	64

// Free, malloc, new and delete are all overridden here
// to make sure nobody is tempted to use them ;).
//void* my_malloc(size_t size);
//void my_free(void * ptr);
//#define malloc(x) my_malloc(x)
//#define free(x) my_free(x)
void* operator new(size_t size);
void operator delete(void *p);
void* operator new[](size_t size);
void operator delete[](void *p);

namespace Engine
{
    namespace Memory
    {
		namespace Allocators
		{
			class CAllocator;
			class CHeapAllocator;
		}

        // The base allocator is responsible for allocating the allocators :). It's a crazy world.
        extern Allocators::CHeapAllocator* g_default_allocator;

        // Our interface for interfacing with the default allocator.
        void                    InitDefaultAllocator(u32 start_memory, u32 max_memory, u32 memory_chunk_size);
        void                    FreeDefaultAllocator();
        Allocators::CAllocator* GetDefaultAllocator ();
    }
}
