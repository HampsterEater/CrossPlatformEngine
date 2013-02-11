///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

// The heap allocator class takes a preallocated block of memory and portions
// it out for individual allocations. Allocation and deallocation of the actual
// block of memory is up to the initialiser. Typically this class is only
// used for the "boot-strapper" default allocator, the one that all other allocators
// allocate from.
//
// The heap allocator is not speedy :). This is meant for large allocations where
// speed is not that important. If you want fast allocations use one of the 
// other allocator types.

#include <stdio.h>
#include <cassert>

#include "CHeapAllocator.h"
#include "CString.h"
#include "CLog.h"

#include <Windows.h>

using namespace Engine::Memory::Allocators;

u32 CHeapAllocator::GetUsedMemory()
{
	u32 total = 0;

	CHeapAllocatorBlock* c = _firstBlock;
	while (c != NULL)
	{
		if (c->free == false)
		{
			total += c->size;
		}
		c = c->nextBlock;
	}

	return total;
}

u32 CHeapAllocator::GetFreeMemory()
{
	u32 total = 0;

	CHeapAllocatorBlock* c = _firstBlock;
	while (c != NULL)
	{
		if (c->free == true)
		{
			total += c->size;
		}
		c = c->nextBlock;
	}

	return total;
}

// Free list operations.
void CHeapAllocator::AddToFreeList(CHeapAllocatorBlock* block)
{
	_lastFreeBlock->nextFreeBlock = block;
	block->prevFreeBlock = _lastFreeBlock;
	block->nextFreeBlock = NULL;

	_lastFreeBlock = block;

	block->free = true;
}
void CHeapAllocator::RemoveFromFreeList(CHeapAllocatorBlock* block)
{
	bool isFirst = (block == _firstFreeBlock);
	bool isLast  = (block == _lastFreeBlock);

	if (isFirst == true && isLast == true)
	{
		_firstFreeBlock = NULL;
		_lastFreeBlock = NULL;
	}
	else if (isFirst == true)
	{
		_firstFreeBlock = _firstFreeBlock->nextFreeBlock;
		_firstFreeBlock->prevFreeBlock = NULL;
	}
	else if (isLast == true)
	{
		_lastFreeBlock = _lastFreeBlock->prevFreeBlock;
		_lastFreeBlock->nextFreeBlock = NULL;
	}
	else
	{
		block->nextFreeBlock->prevFreeBlock = block->prevFreeBlock;
		block->prevFreeBlock->nextFreeBlock = block->nextFreeBlock;
	}

	block->nextFreeBlock = NULL;
	block->prevFreeBlock = NULL;
	block->free = false;
}

// Finds first free block with the given amount of free space.
CHeapAllocatorBlock* CHeapAllocator::FindFreeBlock(u32 size)
{
	CHeapAllocatorBlock* block = _firstFreeBlock;

	while (block != NULL)
	{
		LOG_ASSERT_FAST(block->free == true);		
		
		if (block->size >= size)
		{
			return block;
		}

		block = block->nextFreeBlock;
	}

	return NULL;
}

// Merges two neighbouring blocks.
CHeapAllocatorBlock* CHeapAllocator::MergeBlocks(CHeapAllocatorBlock* left, CHeapAllocatorBlock* right)
{
	// Make sure blocks are neighbouring addresses.
	u8* endOfLeft    = ((u8*)left) + left->size;
	u8* startOfRight = ((u8*)right);
	LOG_ASSERT_FAST(endOfLeft == startOfRight);

	// Add size to the left block.
	left->size += right->size;

	// Remove the right block.
	RemoveFromFreeList(right);

	/// Remove right from block list as well.
	if (right->nextBlock != NULL)
		right->nextBlock->prevBlock = right->prevBlock;
	if (right->prevBlock != NULL)
		right->prevBlock->nextBlock = right->nextBlock;

	// Zero memory.
	#ifdef MEMORY_INIT_ZERO
		memset(((u8*)left) + sizeof(CHeapAllocatorBlock), 0, left->size - sizeof(CHeapAllocatorBlock));
	#endif	

	return left;
}

void* CHeapAllocator::InternalAlloc(u32 size, u32 align)
{
	u32 originalSize = size;

	// Alignment fun!
	u32						alignExtra				= align + 4;   // The extra memz we need to allocate so we can align the pointer correctly.
	u32						alignMask				= (align - 1); // work out the mask that has the bottom n bits set to 1, 
																   // we can use this to mask out the bottom bits of an address to align it.
	// Increase size by the extra info we need for alignment.
	size += alignExtra;
	
	// Thread safety ;3.
	Engine::Platform::MutexLock(&_mutex);

	// Try and get a free block.
	CHeapAllocatorBlock* block = FindFreeBlock(sizeof(CHeapAllocatorBlock) + size);
	
	// if we didn't get a block see if we can allocate another chunk to get the memory from.
	if (block == NULL)
	{	
		u32 chunkSize = max(_memoryChunkSize, size); // Each chunk always has one "main" block, so we need space for that header too.
		AllocateChunk(chunkSize);

		// Now try, if not, we have no memory, fail :(.
		block = FindFreeBlock(sizeof(CHeapAllocatorBlock) + size);
		LOG_ASSERT_FAST(block != NULL); 
	}

	// Do we need to split this block?
	// If we do we need to make sure there is enough space to allocate
	// another block header and data in the left over space.
	// We need:
	//	  header-size  : (NOT NEEDED) We use the header of the block we are splitting for this.
	//	  size		   : Size of the data we are going to store in the chunk we allocated.
	//	  header-size  : header for left of block.
	//    64           : At least 64 bytes for allocation.
	//
	u32 splitMinimum = sizeof(CHeapAllocatorBlock) + size + MEMORY_MINIMUM_BLOCK_SPLIT;
	if (block->size >= splitMinimum)
	{
		// We allocated from higher addresses to lower addresses;
		// meaning we split the block in two and take the right
		// side block for this allocation.
		CHeapAllocatorBlock* left  = block;
		CHeapAllocatorBlock* right = (CHeapAllocatorBlock*)(((u8*)block) + (block->size - (sizeof(CHeapAllocatorBlock) + size))); // endofblock - size 
		
		CHeapAllocatorBlock* leftNext = left->nextBlock;
		CHeapAllocatorBlock* leftNextFree = left->nextFreeBlock;

		// Remove size of allocated right block from left block.
		s32 signedSize = (s32)left->size - ((s32)sizeof(CHeapAllocatorBlock) + (s32)size);
		left->size = max(0, signedSize);
		left->nextBlock = right;
		left->nextFreeBlock = right;

		// Initialize right block and add it to the free list (we will remove it later).
		right->chunk = left->chunk;
		right->size  = size + sizeof(CHeapAllocatorBlock); // Header size is assumed.
		right->free  = true; 
		right->nextBlock = leftNext;
		right->prevBlock = left;
		right->nextFreeBlock = leftNextFree;
		right->prevFreeBlock = left;

		if (right->nextBlock != NULL)
			right->nextBlock->prevBlock = right;

		// Update end block if we have to.
		if (left == _lastFreeBlock)
		{
			_lastFreeBlock = right;
		}

		// We are going to use the right block :3.
		block = right;
	}

	// Remove block from freelist and return it.
	RemoveFromFreeList(block);

	// Store teh callstack for this block if required.
	#ifdef MEMORY_TRACK_CALL_STACK
		block->allocationCallStack = Engine::Platform::DebugTraceCallStack(1);
	#endif
		
	// Increment allocation index :3.
	block->allocIndex = _allocationIndex++;
	block->allocSize = originalSize;

	// Zero out the memory.
	u8* basePtr = ((u8*)block) + sizeof(CHeapAllocatorBlock); // Point to the main block, not the header..
	#ifdef MEMORY_INIT_ZERO
		memset(basePtr, 0, block->size - sizeof(CHeapAllocatorBlock));
	#endif

	// Align the pointer and return it.
	basePtr	+= align + 4; // Add space for alignment padding and pointer to header.

	// Mask the bottom bits of the memory address so we can align it.
	basePtr = (u8*)((u32)basePtr & ~ alignMask); 

	// Store the pointer to the top of the block in the 4 bytes before the actual 
	// aligned memory starts.
	((u32*)(basePtr - 4))[0] = (u32)block;

	Engine::Platform::MutexUnlock(&_mutex);
	
	return basePtr;
}

void CHeapAllocator::InternalFree(void* ptr)
{
	// Thread safety :)
	Engine::Platform::MutexLock(&_mutex);
	
	// Get a pointer to the block header (the pointer is the 4 bytes before the pointer to the actual memory buffer).
	u32* blockAddr = (u32*)((u32*)((u8*)ptr - 4))[0]; 
	CHeapAllocatorBlock* block = reinterpret_cast<CHeapAllocatorBlock*>(blockAddr); 

	// FREEEEEEEEEEEEEEEEEEEEEEDOOOOOOOOOOOOOOOM
	LOG_ASSERT_FAST(block->free == false);
	AddToFreeList(block);
	_freeCount++;

	// Zero out the blocks memory.
	#ifdef MEMORY_INIT_ZERO
		memset(((u8*)block) + sizeof(CHeapAllocatorBlock), 0, block->size - sizeof(CHeapAllocatorBlock));
	#endif	

	// Find the first block before us thats free.
	CHeapAllocatorBlock* firstBlock = block;
	while (true)
	{
		if (firstBlock->prevBlock == NULL)
			break;

		if (firstBlock->prevBlock->free == false)
			break;

		if (firstBlock->prevBlock->chunk != block->chunk)
			break;
		
		firstBlock = firstBlock->prevBlock;
	}

	// Merge forwards!
	while (true)
	{
		if (firstBlock->nextBlock == NULL)
			break;

		if (firstBlock->nextBlock->free == false)
			break;

		if (firstBlock->nextBlock->chunk != block->chunk)
			break;

		// Merge blocks.
		CHeapAllocatorBlock* nextBlock = firstBlock->nextBlock;
		firstBlock = MergeBlocks(firstBlock, nextBlock);
	}
	
	// Thread safety :)
	Engine::Platform::MutexUnlock(&_mutex);
}

u32 CHeapAllocator::InternalSize(void* ptr)
{	
	// Thread safety :)
	Engine::Platform::MutexLock(&_mutex);
	
	u32* blockAddr = (u32*)((u32*)((u8*)ptr - 4))[0]; 
	CHeapAllocatorBlock* block = reinterpret_cast<CHeapAllocatorBlock*>(blockAddr); 

	u32 size = block->allocSize;

	// Thread safety :)
	Engine::Platform::MutexUnlock(&_mutex);

	return size;
}

#ifdef MEMORY_DEBUG_LEAKS
void CHeapAllocator::DumpLeaks()
{
	Engine::Platform::MutexLock(&_mutex);

	CHeapAllocatorBlock* block = _firstBlock;

	printf("\n");
	printf("Dumping Memory Leaks For %s\n", _name.c_str());
	printf("--------------------------------------------------------------\n");

	bool leaking = false;
	u32  index	 = 0;
	while (block != NULL)
	{
		if (block->free == false)
		{
			printf("Address: %p\n", block);
			printf("Index  : %i\n", index);
			printf("Size   : %i\n", block->size);

#ifdef MEMORY_TRACK_CALL_STACK
			for (u32 i = 0; i < block->allocationCallStack.frameCount; i++)
			{
				Engine::Platform::StackFrame frame = Engine::Platform::DebugResolveAddressToStackFrame(block->allocationCallStack.frames[i]);				
				printf("[%i] %s (%i): %s\n", i, frame.file, frame.line, frame.name); 
			}
#endif
			printf("\n");

			leaking = true;
		}

		index++;

		block = block->nextBlock;
	}

	LOG_ASSERT_FAST(leaking == false);

	Engine::Platform::MutexUnlock(&_mutex);
}
#endif

void CHeapAllocator::AllocateChunk(u32 size)
{
	Engine::Platform::MutexLock(&_mutex);

	// Work out how much memory we have already.
	s32 allocatedMemory = 0;
	CHeapAllocatorChunk* allocatedChunk = _firstChunk;
	while (allocatedChunk != NULL)
	{
		allocatedMemory += allocatedChunk->size;
		allocatedChunk = allocatedChunk->nextChunk;
	}

	// Have we got limited memory?
	if (_maxMemory != 0)
	{
		// Check we actually have enough memory left to allocated 
		// some realistic amount of memory. 
		s32 memoryLeft = (s32)_maxMemory - allocatedMemory;
		LOG_ASSERT_FAST(memoryLeft > 0 && memoryLeft > (s32)sizeof(CHeapAllocatorChunk)); // No more memz available :(.

		// Make sure we don't try and allocated more than this chunks amount.
		size = min(size, (u32)memoryLeft);
	}

	// Allocate the chunks memory.
	u32 chunkSize = sizeof(CHeapAllocatorChunk) + size; 
	void* memory = NULL;
	if (_parent == NULL)
	{
		memory = Engine::Platform::MemoryAlloc(chunkSize);
	}
	else
	{
		memory = _parent->InternalAlloc(chunkSize);
	}
	LOG_ASSERT_FAST(memory != NULL);

	#ifdef MEMORY_INIT_ZERO
		memset(memory, 0, chunkSize);
	#endif

	void* chunkMemory = (u8*)memory + sizeof(CHeapAllocatorChunk); 
	CHeapAllocatorChunk* chunk = reinterpret_cast<CHeapAllocatorChunk*>(memory);
	chunk->size		   = chunkSize;
	chunk->memoryBlock = chunkMemory;

	CHeapAllocatorBlock* memBlock = reinterpret_cast<CHeapAllocatorBlock*>(chunk->memoryBlock);
	memBlock->size = chunk->size - sizeof(CHeapAllocatorBlock);
	memBlock->nextBlock = NULL;
	memBlock->prevBlock = NULL;
	memBlock->nextFreeBlock = NULL;
	memBlock->prevFreeBlock = NULL;
	memBlock->free = true;
	memBlock->chunk = chunk;
	memBlock->allocIndex = _allocationIndex++;

	// If this is the first chunk allocated we need
	// to update the block pointers.
	if (_firstChunk == NULL)
	{
		_firstBlock = memBlock;
		_firstFreeBlock = _firstBlock;
		_lastFreeBlock = _firstBlock;

		_firstChunk = chunk;
		_firstChunk->nextChunk = NULL;
	}

	// If not, we need to make this block the first available. 
	else
	{
		// Add to start of block list.
		_firstBlock->prevBlock = memBlock;
		memBlock->nextBlock = _firstBlock;
		memBlock->prevBlock = NULL;
		_firstBlock = memBlock;

		// We don't need to care about last block seeing as we are adding to start, last will stay same.

		// Add to start of free-block list.		
		if (_firstFreeBlock != NULL)
		{
			_firstFreeBlock->prevFreeBlock = memBlock;
			memBlock->nextFreeBlock = _firstFreeBlock;
			memBlock->prevFreeBlock = NULL;
			_firstFreeBlock = memBlock;
		}
		else
		{
			_firstFreeBlock = memBlock;
			_lastFreeBlock = memBlock;
		}

		// Add to chunk list.
		CHeapAllocatorChunk* oldFirstChunk = _firstChunk;
		_firstChunk = chunk;
		_firstChunk->nextChunk = oldFirstChunk;
	}
	
	Engine::Platform::MutexUnlock(&_mutex);
}

CHeapAllocator::CHeapAllocator(Engine::Containers::CString name, u32 start_memory, u32 max_memory, u32 memory_chunk_size)
{
	Engine::Platform::MutexCreate(&_mutex);

	_name = name;
	_parent = NULL;

	_startMemory = start_memory;
	_maxMemory = max_memory;
	_memoryChunkSize = memory_chunk_size;

	_firstBlock = NULL;
	_firstFreeBlock = NULL;
	_lastFreeBlock = NULL;

	_allocationIndex = 0;

	_freeCount = 0;
	_firstChunk = NULL;

	AllocateChunk(_startMemory);
}

CHeapAllocator::CHeapAllocator(Engine::Containers::CString name, Engine::Memory::Allocators::CAllocator* parent, u32 start_memory, u32 max_memory, u32 memory_chunk_size)
{
	Engine::Platform::MutexCreate(&_mutex);

	_name = name;
	_parent = parent;

	_startMemory = start_memory;
	_maxMemory = max_memory;
	_memoryChunkSize = memory_chunk_size;

	_firstBlock = NULL;
	_firstFreeBlock = NULL;
	_lastFreeBlock = NULL;

	_allocationIndex = 0;
	
	_freeCount = 0;
	_firstChunk = NULL;

	AllocateChunk(_startMemory);
}

CHeapAllocator::~CHeapAllocator()
{
#ifdef MEMORY_DEBUG_LEAKS
	DumpLeaks();
#endif

	// Deallocate chunks.
	CHeapAllocatorChunk* allocatedChunk = _firstChunk;
	while (allocatedChunk != NULL)
	{
		CHeapAllocatorChunk* next = allocatedChunk->nextChunk;

		if (_parent == NULL)
		{
			Engine::Platform::MemoryFree(allocatedChunk);
		}
		else
		{
			_parent->Free(&allocatedChunk);
		}

		allocatedChunk = next;
	}

	_parent = NULL;

	_firstBlock = NULL;
	_firstFreeBlock = NULL;

	Engine::Platform::MutexDelete(&_mutex);
}
