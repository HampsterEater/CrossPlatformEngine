///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

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

        #define HASH_TABLE_MAX_LOAD				   0.75f
        #define HASH_TABLE_INCREMENT			   2
		#define HASH_TABLE_INITIAL_BUCKETS		   16

		// Allocators!
		extern Engine::Memory::Allocators::CProxyAllocator* g_hashtable_allocator;
		void InitHashTableAllocator();
		void FreeHashTableAllocator();
		inline Engine::Memory::Allocators::CProxyAllocator* GetHashTableAllocator() { return Engine::Containers::g_hashtable_allocator; }

		// Hash table object.
		template <typename T>
		struct CHashTableValue
		{
			T					 Value;
			u32					 Hash;
			CHashTableValue<T>*  Next;
		};

        // Our lovely ex-list class :3
        template <typename T>
        class CHashTable
        {
			private:
				
				CHashTableValue<T>**	_data;
				u32						_size;
				u32						_allocated;
				
				// Helper functions!
				inline void Initialize     (u32 buckets);
				inline void Allocate       (u32 size, bool keepOld=false);
				inline void CheckLoad	   ();

			public:
			
				// Constructors.
				~CHashTable				   ();
				CHashTable				   ();
				CHashTable				   (u32 buckets);
				CHashTable				   (const CHashTable<T>& a);

				// Properties.
				bool   Empty               (); 
				u32    Size                ();

				// Operator overloads!
				T&	   operator[]          (u32 hash);
				void   operator=           (const CHashTable<T> &arr);

				// General manipulation functions.
				void   Remove               (u32 hash);
				void   Clear                ();
				bool   Contains             (u32 hash);
				void   Insert               (u32 hash, T item);
				CHashTableValue<T>* AtIndex (u32 index);
				CHashTableValue<T>*	FromHash(u32 hash);
		};

        template <typename T>
		void CHashTable<T>::Initialize(u32 buckets)
		{
			_data = NULL;
			_allocated = 0;
			_size = 0;

			Allocate(buckets);
		}

        template <typename T>
		void CHashTable<T>::Allocate(u32 size, bool keepOld=false)
		{
            CHashTableValue<T>** oldData      = _data;
			u32					 oldAllocSize = _allocated;
            u32					 allocateSize =  size;

            if (allocateSize > _allocated || _data == NULL)
			{
                _data = (CHashTableValue<T>**)GetHashTableAllocator()->Alloc(allocateSize * sizeof(CHashTableValue<T>*));
                _allocated = allocateSize;
                memset(_data, 0, allocateSize * sizeof(CHashTableValue<T>*));
				
                if (keepOld == true && oldData != NULL)
                {
					_size = 0;

					// Reinsert the data into the new memory block.
					for (u32 i = 0; i < oldAllocSize; i++)
                    {
						CHashTableValue<T>* val = oldData[i];
						while (val != NULL)
						{
							Insert(val->Hash, val->Value);
							val = val->Next;
						}						
                    }
				}
				else
				{
					_size = 0;
                }

				// Dispose of any old data.
                if (oldData != NULL)
                {
					for (u32 i = 0; i < oldAllocSize; i++)
                    {
						CHashTableValue<T>* val = oldData[i];
						while (val != NULL)
						{
							CHashTableValue<T>* next = val->Next;

							//val->Value.~T(); -> Not needed, the destructor should do this for us.
							GetHashTableAllocator()->FreeObj(&val);

							val = next;
						}						
                    }

		            GetHashTableAllocator()->Free(&oldData);
					oldData = NULL;
                }
            }
		}
		
        template <typename T>
		void CHashTable<T>::CheckLoad()
		{
			f32 current_load = f32(_size) / f32(_allocated);
			if (current_load >= HASH_TABLE_MAX_LOAD)
			{
				u32 new_load_size = _allocated * HASH_TABLE_INCREMENT;
				while (true)
				{
					f32 new_load = f32(_size) / f32(new_load_size);
					if (new_load < HASH_TABLE_MAX_LOAD)
					{
						//printf("Growing buckets to %i (new_load:%f)\n", new_load_size, new_load);
						break;
					}
					new_load_size *= HASH_TABLE_INCREMENT;
				}
				Allocate(new_load_size, true);
			}
		}
		
        template <typename T>
		CHashTable<T>::~CHashTable()
		{
			for (u32 i = 0; i < _allocated; i++)
            {
				CHashTableValue<T>* val = _data[i];
				while (val != NULL)
				{
					CHashTableValue<T>* next = val->Next;
					
					//val->Value.~T(); -> Not needed, the destructor should do this for us.
					GetHashTableAllocator()->FreeObj(&val);

					val = next;
				}						
            }

		    GetHashTableAllocator()->Free(&_data);
		}

        template <typename T>
		CHashTable<T>::CHashTable()
		{
			Initialize(HASH_TABLE_INITIAL_BUCKETS);
		}

        template <typename T>
		CHashTable<T>::CHashTable(u32 buckets)
		{
			Initialize(buckets);
		}

        template <typename T>
		CHashTable<T>::CHashTable(const CHashTable<T>& a)
		{
			Initialize(buckets);
			Allocate(a._size, false);

			for (u32 i = 0; i < a._size; i++)
			{
				CHashTableValue<T> val = a.AtIndex(i);
				Insert(val.Hash, val.Value);
			}
		}

        template <typename T>
		bool CHashTable<T>::Empty()
		{
			return _size > 0;
		}

        template <typename T>
		u32 CHashTable<T>::Size()
		{
			return _size;
		}

        template <typename T>
		T& CHashTable<T>::operator[](u32 hash)
		{
			return FromHash(hash)->Value;
		}

        template <typename T>
		void CHashTable<T>::operator=(const CHashTable<T> &a)
		{
			Allocate(a._size, false);

			for (u32 i = 0; i < a._size; i++)
			{
				CHashTableValue<T> val = a.AtIndex(i);
				Insert(val.Hash, val.Value);
			}
		}

        template <typename T>
		void CHashTable<T>::Remove(u32 hash)
		{
			u32 bucket_index = hash % _allocated;
			
			CHashTableValue<T>* bucket = _data[bucket_index];
			CHashTableValue<T>* val = bucket;
			if (bucket != NULL)
			{
				CHashTableValue<T>* prev = val;
				while (val != NULL)
				{
					CHashTableValue<T>* next = val->Next;
					
					if (val->Hash == hash)
					{
						// Update list pointers.
						if (val == bucket)
						{
							_data[bucket_index] = next;
							prev = next;
						}
						else
						{
							// Update previous link to point to next link.
							if (prev != NULL)
							{
								prev->Next = val->Next;
							}
						}

						//val->Value.~T(); -> Not needed, the destructor should do this for us.
						GetHashTableAllocator()->FreeObj(&val);			

						_size--;
						return;
					}
					else
					{
						prev = val;
					}

					val = next;
				}
			}
		}

        template <typename T>
		void CHashTable<T>::Clear()
		{			
			for (u32 i = 0; i < _allocated; i++)
            {
				CHashTableValue<T>* val = _data[i];
				while (val != NULL)
				{
					CHashTableValue<T>* next = val->Next;

					//val->Value.~T(); -> Not needed, the destructor should do this for us.
					GetHashTableAllocator()->FreeObj<CHashTableValue<T>>(&val);

					val = next;
				}		
				_data[i] = NULL;
            }
			_size = 0;
		}
		
        template <typename T>		
		CHashTableValue<T>* CHashTable<T>::FromHash(u32 hash)
		{
			u32 bucket = hash % _allocated;

			CHashTableValue<T>* val = _data[bucket];
			while (val != NULL)
			{
				if (val->Hash == hash)
				{
					return val;
				}
					
				val = val->Next;
			}		

			return NULL;
		}

        template <typename T>
		CHashTableValue<T>* CHashTable<T>::AtIndex(u32 hash)
		{
			u32 index = 0;
			for (u32 i = 0; i < _allocated; i++)
            {
				CHashTableValue<T>* val = _data[i];
				while (val != NULL)
				{
					if (index == hash)
					{
						return val;
					}

					index++;
					val = val->Next;
				}		
            }

			return NULL;
		}

        template <typename T>
		bool CHashTable<T>::Contains(u32 hash)
		{
			return (FromHash(hash) != NULL);
		}
		
        template <typename T>
		void CHashTable<T>::Insert(u32 hash, T item)
		{
			u32 bucket_index = hash % _allocated;
			
			// Find end of current bucket chain.
			CHashTableValue<T>* bucket = _data[bucket_index];
			while (bucket != NULL)
			{
				LOG_ASSERT_MSG((bucket->Hash != hash), "Attempt to insert value into hash table with duplicate hash!");
				CHashTableValue<T>* next = bucket->Next;
				
				if (next == NULL)
				{
					break;					
				}

				bucket = next;
			}

			// Insert the new item.
			CHashTableValue<T>* val = GetHashTableAllocator()->NewObj<CHashTableValue<T>>();
			val->Hash = hash;
			val->Value = item;
			val->Next = NULL;

			if (bucket == NULL)
			{
				_data[bucket_index] = val;
			}
			else
			{
				bucket->Next = val;			
			}

			_size++;

			// Expand if we are over the load limit.
			CheckLoad();
		}

	}
}