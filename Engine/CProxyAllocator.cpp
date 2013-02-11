///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

// The block allocator is very simple. All requests just get passed through
// to another allocator, while tracking memory usage.

#include <stdio.h>

#include "CProxyAllocator.h"

using namespace Engine::Memory::Allocators;

void* CProxyAllocator::InternalAlloc(u32 size, u32 align)
{
	return _parent->InternalAlloc(size, align);
}

void CProxyAllocator::InternalFree(void* ptr)
{
	_parent->InternalFree(ptr);
}

u32 CProxyAllocator::InternalSize(void* ptr)
{
	return _parent->Size(ptr);
}

CProxyAllocator::CProxyAllocator(Engine::Containers::CString name, CAllocator* parent)
{
	_name = name;
    _parent = parent;
}

CProxyAllocator::~CProxyAllocator()
{
    _parent = NULL;
}

