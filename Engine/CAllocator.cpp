///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CAllocator.h"
#include "CString.h"

using namespace Engine::Memory::Allocators;

Engine::Containers::CString CAllocator::GetName()
{
	return _name;
}

void CAllocator::SetName(Engine::Containers::CString n)
{
	_name = n;
}