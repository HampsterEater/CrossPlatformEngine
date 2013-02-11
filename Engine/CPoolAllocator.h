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

            class CPoolAllocator : public CAllocator
            {
            private:
                CAllocator* _parent;

				u32 _blockSize;

            public:
                virtual void* Alloc  (u32 size, u32 align=16);
                virtual void  Free   (void* ptr);
                virtual u32   Size   (void* ptr);

                CPoolAllocator(Engine::Types::CString name, CAllocator* parentAllocator);
                ~CPoolAllocator();
            };

        }
    }
}


