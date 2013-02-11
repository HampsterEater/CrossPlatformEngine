///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

// The frame allocator is very simple. When you allocate it you pass it a size.
// All allocations will then occur on that block of memory through pointer 
// nudging, and the memory block will be deallocated in one go at the end
// of the scope.
//
// Attempts to call free on this allocator will cause an assert to fail.
// Memory is only ever freed at the end.

#include <stdio.h>

#include "Platform.h"
#include "CFrameAllocator.h"

#include "CLog.h"

using namespace Engine::Memory::Allocators;

void* CFrameAllocator::InternalAlloc(u32 size, u32 align)
{
	// Alignment fun!
	u32						alignExtra				= align + 4;   // The extra memz we need to allocate so we can align the pointer correctly.
	u32						alignMask				= (align - 1); // work out the mask that has the bottom n bits set to 1, 
			
	Engine::Platform::MutexLock(&_mutex);

	// Find our next pointer.
	u32 alignedPtr = (u32)_framePointer + alignExtra;
	
	// Mask the bottom bits of the memory address so we can align it.
	alignedPtr = alignedPtr & ~ alignMask; 
	
	// Make sure pointer is valid.
	LOG_ASSERT(alignedPtr + size <= (u32)_frameBase + _size);

	// Store the size below the pointer.	
	*((u32*)(alignedPtr - 4)) = size;

	// Bump the frame pointer.
	_framePointer = (void*)((u32)_framePointer + size);

	Engine::Platform::MutexUnlock(&_mutex);

	return (void*)alignedPtr;
}

void CFrameAllocator::InternalFree(void* ptr)
{
	LOG_ASSERT(false);
}

u32 CFrameAllocator::InternalSize(void* ptr)
{	
	u32 size = ((u32*)((u8*)ptr - 4))[0];
	return size;
}

CFrameAllocator::CFrameAllocator(Engine::Containers::CString name, u32 size, CAllocator* parent)
{
	Engine::Platform::MutexCreate(&_mutex);

	_name = name;
    _parent = parent;
	_size = size;

	_frameBase = parent->InternalAlloc(size);
	_framePointer = _frameBase;
	LOG_ASSERT(_frameBase != NULL);
}

CFrameAllocator::~CFrameAllocator()
{
	if (_frameBase != NULL)
	{
	    _parent->Free(&_frameBase);
		_frameBase = NULL;
		_framePointer = NULL;
	}
	_parent = NULL;

	Engine::Platform::MutexDelete(&_mutex);
}

