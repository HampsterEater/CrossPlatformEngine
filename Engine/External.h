///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Memory.h"
#include "CProxyAllocator.h"

namespace Engine
{
	namespace External
	{

	    extern Engine::Memory::Allocators::CProxyAllocator* g_external_allocator;

        // Our interface for interfacing with the default allocator.
        void									InitExternalAllocator();
        void									FreeExternalAllocator();
        Engine::Memory::Allocators::CAllocator* GetExternalAllocator ();

		bool InitializeLibs		();
		bool DeinitializeLibs	();

	}
}