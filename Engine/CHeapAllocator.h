///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CAllocator.h"

#include "Memory.h"

namespace Engine
{
	namespace Containers
	{
		class CString;
	}
    namespace Memory
    {
        namespace Allocators
        {
			struct CHeapAllocatorChunk
			{
				void*					memoryBlock;
				u32						size;

				CHeapAllocatorChunk*	nextChunk;
				CHeapAllocatorChunk*	prevChunk;

				//u32						allocationCount;
				//f64						lastAllocation;
			};

			struct CHeapAllocatorBlock
			{
			public:
				u32								size;
				u32								allocSize;
				bool							free;

				CHeapAllocatorChunk*			chunk;

				CHeapAllocatorBlock*			nextBlock;
				CHeapAllocatorBlock*			prevBlock;

				CHeapAllocatorBlock*			nextFreeBlock;
				CHeapAllocatorBlock*			prevFreeBlock;

			#ifdef MEMORY_TRACK_CALL_STACK	
				Engine::Platform::StackTrace	allocationCallStack;					
			#endif

				u32								allocIndex;
			};

            class CHeapAllocator : public CAllocator
            {
            private:
				CHeapAllocatorBlock* _firstBlock;
				CHeapAllocatorChunk* _firstChunk;

				CHeapAllocatorBlock* _firstFreeBlock;
				CHeapAllocatorBlock* _lastFreeBlock;				

				u32   _allocCount;
				u32   _freeCount;

				u32	  _allocTotal;
				u32	  _freeTotal;

				u32	  _startMemory;
				u32	  _maxMemory;
				u32	  _memoryChunkSize;

				u32	  _allocationIndex;
				
                Engine::Memory::Allocators::CAllocator* _parent;

				Engine::Platform::MutexHandle _mutex;

            public:

				inline CHeapAllocatorBlock* MergeBlocks				(CHeapAllocatorBlock* left, CHeapAllocatorBlock* right);
				
				inline void AddToFreeList							(CHeapAllocatorBlock* block);
				inline void RemoveFromFreeList						(CHeapAllocatorBlock* block);
		
				inline CHeapAllocatorBlock*			FindFreeBlock	(u32 size);

				u32 GetUsedMemory();
				u32 GetFreeMemory();

                virtual void* InternalAlloc  (u32 size, u32 align=16);
                virtual void  InternalFree   (void* ptr);
                virtual u32   InternalSize   (void* ptr);

				#ifdef MEMORY_DEBUG_LEAKS
					void DumpLeaks();
				#endif

				void AllocateChunk(u32 size);

                CHeapAllocator(Engine::Containers::CString name, u32 start_memory, u32 max_memory, u32 memory_chunk_size);
                CHeapAllocator(Engine::Containers::CString name, Engine::Memory::Allocators::CAllocator*, u32 start_memory, u32 max_memory, u32 memory_chunk_size);
                ~CHeapAllocator();

            };

        }
    }
}

