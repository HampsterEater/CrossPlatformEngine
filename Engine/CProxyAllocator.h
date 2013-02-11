///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CAllocator.h"

namespace Engine
{
    namespace Memory
    {
        namespace Allocators
        {

            class CProxyAllocator : public CAllocator
            {
            private:
                CAllocator* _parent;

            public:
                virtual void* InternalAlloc  (u32 size, u32 align=16);
                virtual void  InternalFree   (void* ptr);
                virtual u32   InternalSize   (void* ptr);

                CProxyAllocator(Engine::Containers::CString name, CAllocator* parentAllocator);
                ~CProxyAllocator();
            };

        }
    }
}


