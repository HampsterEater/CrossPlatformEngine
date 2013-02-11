///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

// The pool allocater is very simple. It assumes all allocations are the same
// size as the initial allocation. Everything is tightly packed in memory, with
// minimal headers (only a allocated/deallocated flag). 

#include <stdio.h>

#include "CPoolAllocator.h"

using namespace Engine::Memory::Allocators;

void* CPoolAllocator::Alloc(u32 size, u32 align)
{
	if (size == 0) _blockSize = size;
	LOG_ASSERT(size == _blockSize);


}

void CPoolAllocator::Free(void* ptr)
{
}

u32 CPoolAllocator::Size(void* ptr)
{
	LOG_ASSERT(_blockSize != 0);
	return _blockSize;
}

CPoolAllocator::CPoolAllocator(Engine::Types::CString name, CAllocator* parent)
{
	_name = name;
    _parent = parent;
}

CPoolAllocator::~CPoolAllocator()
{
    _parent = NULL;
}

