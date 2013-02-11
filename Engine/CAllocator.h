///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <new>

#include "Conditionals.h"
#include "CString.h"
#include "CLog.h"
#include "TemplateHelper.h"

namespace Engine
{
    namespace Memory
    {
        namespace Allocators
        {

			// Defines the maximum amount of overhead from placement new arrays, if its 
			// over this then god help us. It shouldn't really be more than 4 (8 on 64bit),
			// but for maximum portability we assume a far higher number.
			#define MAXIMUM_PLACEMENT_NEW_ARRAY_OVERHEAD 32

            // Base class for all allocators, which are responsible for the allocation sceheme
            // of create and disposing of different objects and blocks of memory.
            class CAllocator
            {
			protected:
				Engine::Containers::CString _name;

				template <class T> void FreeArrayInternal(Engine::Misc::IsPointerType ptrtype, T** ptr)
				{
					(ptrtype);

					// Ignore pointers.
				}

				template <class T> void FreeArrayInternal(Engine::Misc::IsNotPointerType ptrtype, T** ptr)
				{
					(ptrtype);

					u32 elements = ArraySize(*ptr) / sizeof(T);

					// Destruct references pointers.					
					for (u32 i = 0; i < elements; i++)
					{
						(*ptr)[i].~T();
					}
				}


				template <class T> void InternalArrayCtor(Engine::Misc::IsPointerType ptrtype, T* ptr)
				{
					(ptrtype);

					// Ignore pointers.
				}

				template <class T> void InternalArrayCtor(Engine::Misc::IsNotPointerType ptrtype, T* ptr)
				{
					(ptrtype);
					
					u32 elements = ArraySize(*ptr) / sizeof(T);

					// Construct references pointers.					
					for (u32 i = 0; i < elements; i++)
					{
						ptr[i].T();
					}
				}

            public:
				Engine::Containers::CString GetName();
				void SetName(Engine::Containers::CString n);
				
                virtual void* InternalAlloc     (u32 size, u32 align=16)      = 0;
                virtual void  InternalFree      (void* ptr)                   = 0;
                virtual u32   InternalSize      (void* ptr)                   = 0;	// Note: This includes any extra memory used for alignment!


				template <class T> void FreeArray(T** ptr)           
				{
					Engine::Misc::IsPointer<T>::Result isptr = NULL;

					u8* ptrPtr = ((u8*)(*ptr));
					while (ptrPtr)
					{
						if (ptrPtr[0] == (u8)0xA1 &&
							ptrPtr[1] == (u8)0xB2 &&
							ptrPtr[2] == (u8)0xC3 &&
							ptrPtr[3] == (u8)0xD4)
							break;
						ptrPtr--;
					}
					
					FreeArrayInternal<T>(isptr, ptr);
					InternalFree(ptrPtr);
					*ptr = NULL;
				}
				
				template <class T> T* AllocArray(u32 size, u32 align=16)           
				{
					u32 full_size = sizeof(T) * size;
					u32 memBlockSize = full_size + (MAXIMUM_PLACEMENT_NEW_ARRAY_OVERHEAD + 4);

					// Allocate array and construct.
					void* mem = Alloc(memBlockSize, align);
					T* arr = new(((u8*)mem) + 4) T[size];

					// Write in the "start of block" marker.
					// We have to do this nonsense as we have no idea how large the allocated overhead 
					// will be, its all up the the CRT, its not standardized.
					((u8*)mem)[0] = (u8)0xA1;
					((u8*)mem)[1] = (u8)0xB2;
					((u8*)mem)[2] = (u8)0xC3;
					((u8*)mem)[3] = (u8)0xD4;

					// Work out size.
					u32 new_size = ArraySize(arr);

					return arr;
				}
				
				template <class T> u32 ArraySize(T ptr)           
				{
					u8* ptrPtr = ((u8*)ptr);
					while (ptrPtr)
					{
						if (ptrPtr[0] == (u8)0xA1 &&
							ptrPtr[1] == (u8)0xB2 &&
							ptrPtr[2] == (u8)0xC3 &&
							ptrPtr[3] == (u8)0xD4)
							break;
						ptrPtr--;
					}
					
					return InternalSize(ptrPtr);
				}

				template <class T> void Free(T** ptr)           
				{
					InternalFree((void*)(*ptr));
					*ptr = NULL;
				}            

				void* Alloc(u32 size, u32 align=16)           
				{
					return InternalAlloc(size, align);
				}                
				
				template <class T> u32 Size(T ptr)           
				{
					void* ptrPtr = (void*)ptr;
					return InternalSize(ptrPtr);
				}

                template <class T> void FreeObj(T** obj)
                {
					T* ptr = *obj;
                    if (ptr != NULL)
                    {
                        ptr->~T();
                        InternalFree((void*)ptr);
						*obj = NULL;
                    }
					else
					{
						LOG_ASSERT(false);
					}
                }

                template <class T> T* NewObj()
                {
					void* mem = InternalAlloc(sizeof(T), alignof(T));
                    return ::new (mem) T;
                }
                template <class T, class T2> T* NewObj(T2 a)
                {
					void* mem = InternalAlloc(sizeof(T), alignof(T));
                    return ::new (mem) T(a);
                }
                template <class T, class T2, class T3> T* NewObj(T2 a, T3 b)
                {
					void* mem = InternalAlloc(sizeof(T), alignof(T));
                    return ::new (mem) T(a, b);
                }
                template <class T, class T2, class T3, class T4> T* NewObj(T2 a, T3 b, T4 c)
                {
					void* mem = InternalAlloc(sizeof(T), alignof(T));
                    return ::new (mem) T(a, b, c);
                }
                template <class T, class T2, class T3, class T4, class T5> T* NewObj(T2 a, T3 b, T4 c, T5 d)
                {
					void* mem = InternalAlloc(sizeof(T), alignof(T));
                    return ::new (mem) T(a, b, c, d);
                }
                template <class T, class T2, class T3, class T4, class T5, class T6> T* NewObj(T2 a, T3 b, T4 c, T5 d, T6 e)
                {
					void* mem = InternalAlloc(sizeof(T), alignof(T));
                    return ::new (mem) T(a, b, c, d, e);
                }      

            };

        }
    }
}

