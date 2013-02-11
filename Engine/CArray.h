///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once
		
// --------------------------------------------------------------------------
// Warning: 
//	Removal of elements from this array implementation is slow as hell! 
//	The reason being that this array system prevents fragmentation
//	by shifting elements when elements are removed. This allows very fast
//  insertion and lookups, but causes very slow removals! If you need
//  fast removals too, use a linked list.
// --------------------------------------------------------------------------

#include <string>
#include <sstream>
#include <stdio.h>

#include "Conditionals.h"
#include "CString.h"

#include "CLog.h"

#include "Memory.h"
#include "CProxyAllocator.h"

#include "TemplateHelper.h"

namespace Engine
{
    namespace Containers
    {
        #define ARRAY_ALLOC_START                  1//32
        #define ARRAY_ALLOC_INTERVAL               2.0f

		// Allocators!
		extern Engine::Memory::Allocators::CProxyAllocator* g_array_allocator;
		void InitArrayAllocator();
		void FreeArrayAllocator();
		inline Engine::Memory::Allocators::CProxyAllocator* GetArrayAllocator() { return Engine::Containers::g_array_allocator; }

        // Our lovely ex-list class :3
        template <typename T>
		struct CArrayEntry
		{
			T	 Value;
			bool Used;
		};

        template <typename T>
        class CArray
        {
            protected:
                CArrayEntry<T>*      _data;
                CArrayEntry<T>       _startBuffer[ARRAY_ALLOC_START]; // This allocates initial space for us on the stack, much better than a call to new :3
               
				u32        _length;
                u32        _allocated;

                // Helper functions!
                inline void         Initialize     ();
                inline void         Allocate       (u32 size, bool keepOld=false);

				//void				DisposeIndex   (Engine::Misc::IsPointerType type, CArrayEntry<T>&  index);
				//void				DisposeIndex   (Engine::Misc::IsNotPointerType type, CArrayEntry<T>&  index);

            public:

                typedef signed int (*SORT_FUNCTION)(T&, T&);
				
                // Some generic sort methods.
                static s32 SortAsInteger(s32& x, s32& y);
                static s32 SortAsString(CString& x, CString& b);

                // Constructors.
                ~CArray                   ();
                CArray                    ();
                CArray                    (u32 length);
                CArray                    (const CArray<T>& v);
                CArray                    (T v[], u32 size);

                // Properties.
                inline bool	Empty         () const      { return _length <= 0; } 
                inline u32	Size          () const      { return _length; } 

                // Operator overloads!
                inline T&     operator[]     (u32 index) const;
					   void   operator=      (const CArray<T> &arr);

                // General manipulation functions.
                void   AddToFront          (T item);
                void   AddToEnd            (T item);
                void   Add                 (T item);

                T      RemoveFromFront     ();
                T      RemoveFromEnd       ();
                void   Remove              (T item);
                T      RemoveIndex         (u32 index);

                void   ShiftForward        ();
                void   ShiftBackward       ();

                s32    IndexOf             (T item);

                void   Clear               ();

                void   Insert              (T item, u32 index);
                void   InsertBefore        (T item, T at);
                void   InsertAfter         (T item, T at);

                u32    Count               (T item);

                bool   Contains            (T item);

                void   Sort                (SORT_FUNCTION func, bool ascending=true);
                void   QuickSort           (SORT_FUNCTION func, bool ascending, s32 start, s32 end);

        };
		
        // Sorting functions
        template <typename T>
        s32 CArray<T>::SortAsInteger(s32& x, s32& y)
        {
            return y - x;
        }

        template <typename T>
        s32 CArray<T>::SortAsString(CString& x, CString& y)
        {
            s32 xlen = x.Length();
            s32 ylen = y.Length();

            s32 k  = 0;
            s32 n  = 0;
            s32 sz = xlen < ylen ? xlen : ylen;

            const u8 * xbuff = x.c_str();
            const u8 * ybuff = y.c_str();

            for (k = 0; k < sz; ++k)
            {
                if (n = (xbuff[k] - ybuff[k]))
                    return n;
            }

            return xlen - ylen;
        }

        template <typename T>
        CArray<T>::~CArray()
        {
            if (_data != NULL)
			{				
				for (u32 i = 0; i < _length; i++)
				{
					//Engine::Misc::IsPointer<T>::Result type = NULL;
					if (_data[i].Used == true)
					{
						_data[i].~CArrayEntry<T>();
						_data[i].Used = false;
					}
					//DisposeIndex(type, _data[i]);
				}

				if (_data != _startBuffer)
					GetArrayAllocator()->Free(&_data);
	
				_data = NULL;
			}
        }
		
       /* template <typename T>
		void CArray<T>::DisposeIndex(Engine::Misc::IsPointerType type, CArrayEntry<T>& index)
		{
			// Ignore pointer.
		}

        template <typename T>
		void CArray<T>::DisposeIndex(Engine::Misc::IsNotPointerType type, CArrayEntry<T>& index)
		{
			index.Value.~T();
			index.Used = false;
		}*/

        template <typename T>
        CArray<T>::CArray()
        {
            Initialize();
            Allocate(0);
        }

        template <typename T>
        CArray<T>::CArray(u32 length)
        {
            Initialize();
		    Allocate(length);
			_length = 0;
        }

        template <typename T>
        CArray<T>::CArray(const CArray<T>& v)
        {
            Initialize();
            Allocate(v._length);

            for (u32 i = 0; i < v._length; i++)
                _data[i] = v._data[i];
        }

        template <typename T>
        CArray<T>::CArray(T v[], u32 size)
        {
            Initialize();
            Allocate(size);

            for (u32 i = 0; i < size; i++)
                _data[i] = v._data[i];
        }

        template <typename T>
        void CArray<T>::Initialize()
        {
            _data       = _startBuffer;
            _allocated  = ARRAY_ALLOC_START;
            _length     = 0;

            memset(_data, 0, ARRAY_ALLOC_START * sizeof(CArrayEntry<T>));
        }

        template <typename T>
        void CArray<T>::Allocate(u32 size, bool keepOld)
        {
            CArrayEntry<T>* oldData = _data;
			u32 oldAllocSize = _allocated;
            u32 allocateSize =  size;

            if (allocateSize > _allocated || _data == NULL)
            {
                s32 newSize = (s32)(allocateSize * ARRAY_ALLOC_INTERVAL);

                u32 oldAllocated = _allocated;
                _data = (CArrayEntry<T>*)GetArrayAllocator()->Alloc(newSize * sizeof(CArrayEntry<T>));
                _allocated = newSize;

                memset(_data, 0, newSize * sizeof(CArrayEntry<T>));
				
				// Copy data over.
				for (u32 i = 0; i < oldAllocated; i++)
				{
					if (keepOld == true)
					{
						if (oldData[i].Used = true)
						{
							_data[i] = oldData[i];
							_data[i].Used = true;
						}
					}
					else
					{
						_data[i].Used = false;
					}
				}

                if (oldData != NULL)
                {
					// Call destructors for data.
					for (u32 i = 0; i < oldAllocSize; i++)
					{	
						if (oldData[i].Used = true)
						{
							oldData[i].~CArrayEntry<T>();
							oldData[i].Used = false;
						}
					}

					if (oldData != _startBuffer)
					{
		                GetArrayAllocator()->Free(&oldData);
						oldData = NULL;
					}
                }
            }

            _length = size;
        }

        template <typename T>
        T& CArray<T>::operator[](u32 index) const
        {
     //       if (index >= _length)
      //      {
       //         Allocate(index + 1, true);
       //     }
            return _data[index].Value;
        }

        template <typename T>
        void CArray<T>::operator=(const CArray<T> &arr)
        {
            Allocate(arr._length);

            for (u32 i = 0; i < arr._length; i++)
                _data[i] = arr._data[i];
        }

        template <typename T>
		void CArray<T>::Sort(SORT_FUNCTION func, bool ascending)
		{
			QuickSort(func, ascending, 0, _length - 1);
		}

        template <typename T>
		void CArray<T>::QuickSort(SORT_FUNCTION func, bool ascending, s32 start, s32 end)
		{
			if ((end - start) < 1)
				return;

			s32 pivotIndex		= start + ((end - start) / 2);
			s32 i				= start;
			s32 k				= end;
			CArrayEntry<T> pivotValue = _data[pivotIndex];

			do 
			{
				while (i < k)
				{
					s32 cmp = func(_data[i].Value, pivotValue.Value);
					if (ascending == false)
						cmp = -cmp;
					if (cmp >= 0)
						break;
					i++;
				}

				while (k > i)
				{
					s32 cmp = func(_data[k].Value, pivotValue.Value);
					if (ascending == false)
						cmp = -cmp;
					if (cmp <= 0)
						break;
					k--;
				}

				if (i <= k)
				{
					CArrayEntry<T> tmp	= _data[i];
					_data[i]			= _data[k];
					_data[k]			= tmp;

					i++;
					k--;
				}
				else
				{
					break;
				}

			} while (true);

			if (start < k)
				QuickSort(func, ascending, start, k);
	
			if (i < end)
				QuickSort(func, ascending, i, end);
		}

        template <typename T>
        void CArray<T>::AddToFront(T item)
        {
            Allocate(_length + 1, true);
            ShiftForward();
            _data[0].Value = item;
			_data[0].Used = true;
        }

        template <typename T>
        void CArray<T>::AddToEnd(T item)
        {
            Allocate(_length + 1, true);
            _data[_length - 1].Value = item;
            _data[_length - 1].Used = true;
        }

        template <typename T>
        void CArray<T>::Add(T item)
        {
            for (u32 i = 0; i < _length; i++)
            {
                if (_data[i].Used == false)
                {
                    _data[i].Value = item;
					_data[i].Used = true;
                    return;
                }
            }

            AddToEnd(item);
        }

        template <typename T>
        T CArray<T>::RemoveFromFront()
        {
			/*
            T item = _data[0].Value;
            ShiftBackward();
            _length--;
            return item;
			*/
			return RemoveIndex(0);
        }

        template <typename T>
        T CArray<T>::RemoveFromEnd()
        {
			/*
            T item = _data[--_length].Value;
            return item;
			*/			
			return RemoveIndex(_length - 1);
        }

        template <typename T>
        void CArray<T>::Remove(T item)
        {
            for (u32 i = 0; i < _length; i++)
            {
                if (_data[i].Value == item)
                {
                    RemoveIndex(i);
                }
            }
        }

        template <typename T>
        T CArray<T>::RemoveIndex(u32 index)
        {
            LOG_ASSERT(index >= 0 && index < _length);			
			T value = _data[index].Value;
			
			// Dispose of value at index.
			//_data[index].Value.~T();
			//_data[index].Used = true;

            // Shift everything (without copying) down by one index.
			//u32 len = (_length - (index + 1)) * sizeof(CArrayEntry<T>);
			//u8* dest = (u8*)(_data + index);
			//u8* src  = (u8*)(_data + (index + 1));
			//memmove(dest, src, len);			
			for (u32 i = index; i < _length - 1; i++)
			{
				_data[i] = _data[i + 1];
			//	u8* dst = (u8*)(&_data[i].Value);
			//	u8* src = (u8*)(&_data[i + 1].Value);
			//	u32 len = sizeof(T);
			//	memmove(dst, src, len);
			}

            // Reduce index.
            _length--;
			
			// Free up the array entry.
			_data[_length].~CArrayEntry<T>();
			_data[_length].Used = false;

			return value;
        }

        template <typename T>
        void CArray<T>::ShiftForward()
        {
            for (u32 i = _length - 1; i > 0; i--)
            {
                _data[i] = _data[i - 1];
                if (i <= 1)
                {
                    break;
                }
            }
            _data[0].Used = false;
        }

        template <typename T>
        void CArray<T>::ShiftBackward()
        {
            for (u32 i = 0; i < _length - 1; i++)
            {
                _data[i] = _data[i + 1];
            }
            _data[_length - 1].Used = false;
        }

        template <typename T>
        s32 CArray<T>::IndexOf(T item)
        {
             for (u32 i = 0; i < _length; i++)
            {
                if (_data[i].Value == item)
                {
                    return i;
                }
            }
			 return -1;
        }

        template <typename T>
        void CArray<T>::Clear()
        {
			for (u32 i = 0; i < _length; i++)
			{
				if (_data[i].Used == true)
				{
					_data[i].~CArrayEntry<T>();
					_data[i].Used = false;
				}
			}
//            memset(_data, 0, _allocated * sizeof(CArrayEntry<T>));
            _length = 0;
        }

        template <typename T>
        void CArray<T>::Insert(T item, u32 index)
        {
            LOG_ASSERT(index >= 0 && index <= _length);
            Allocate(_length + 1, true);

            for (u32 i = _length - 1; i >= index; i--)
            {
                if (i > index)
                {
                    _data[i] = _data[i - 1];
                }
                if (i <= index)
                {
                    break;
                }
            }

            _data[index].Value = item;
			_data[index].Used = true;
        }

        template <typename T>
        void CArray<T>::InsertBefore(T item, T at)
        {
            for (u32 i = 0; i < _length; i++)
            {
                if (_data[i] == at)
                {
                    Insert(item, i);
                    return;
                }
            }
        }

        template <typename T>
        void CArray<T>::InsertAfter(T item, T at)
        {
            for (u32 i = 0; i < _length; i++)
            {
                if (_data[i] == at)
                {
                    Insert(item, i + 1);
                    return;
                }
            }
        }

        template <typename T>
        u32 CArray<T>::Count(T item)
        {
            u32 count = 0;
            for (u32 i = 0; i < _length; i++)
            {
                if (_data[i].Value == item)
                {
                    count++;
                }
            }
            return count;
        }

        template <typename T>
        bool CArray<T>::Contains(T item)
        {
            for (u32 i = 0; i < _length; i++)
            {
                if (_data[i].Value == item)
                {
                    return true;
                }
            }
            return false;
        }

    }

}
