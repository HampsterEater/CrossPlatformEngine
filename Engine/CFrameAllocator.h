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

            class CFrameAllocator : public CAllocator
            {
            private:
                CAllocator* _parent;
				u32 _size;
				void* _frameBase;
				void* _framePointer;

				Engine::Platform::MutexHandle _mutex;


            public:
                virtual void* InternalAlloc  (u32 size, u32 align=16);
                virtual void  InternalFree   (void* ptr);
                virtual u32   InternalSize   (void* ptr);

                CFrameAllocator(Engine::Containers::CString name, u32 size, CAllocator* parentAllocator);
                ~CFrameAllocator();
            };

        }
    }
}


