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
		// Allocators!
		extern Engine::Memory::Allocators::CProxyAllocator* g_list_allocator;
		void InitListAllocator();
		void FreeListAllocator();
		inline Engine::Memory::Allocators::CProxyAllocator* GetListAllocator() { return Engine::Containers::g_list_allocator; }

        // Used to store information on a list item.
        template <typename T>
        class CListNode
        {
            public:
                CListNode<T>* Next;
                CListNode<T>* Prev;
                T              Value;

                CListNode()
                {
                    Next = NULL;
                    Prev = NULL;
                }

                CListNode(const CListNode<T>& n) :
                    Next(n.Next),
                    Next(n.Prev),
                    Next(n.Value)
                    {

                    }
        };

        // Our lovely ex-list class :3
        template <typename T>
        class CList
        {
            protected:
                u32                 _length;
                CListNode<T>*      _head;

                // Helper functions!
                inline void         Initialize      ();

            public:

                typedef CListNode<T> Node;
                typedef signed int (*SORT_FUNCTION)(T&, T&);

                // Some generic sort methods.
                static s32 SortAsInteger(s32& x, s32& y);
                static s32 SortAsString(CString& x, CString& b);

                // Constructors.
                ~CList                             ();
                CList                              ();
                CList                              (const CList<T>& v);

                // Properties.
                bool   Empty                       ()                      { return _length <= 0; }
                u32    Size                        ()                      { return _length; }

				inline CListNode<T>*  Start        ()                      { return (_head->Next == _head ? NULL : _head->Next); }
                inline CListNode<T>*  End          ()                      { return (_head->Prev == _head ? NULL : _head->Prev); }
                inline CListNode<T>*  Next         (CListNode<T>* n)       { return (n->Next     == _head ? NULL : n->Next); }
                inline CListNode<T>*  Prev         (CListNode<T>* n)       { return (n->Prev     == _head ? NULL : n->Prev); }

                // Operator overloads!
                T&              operator[]          (u32 index);
                void            operator=           (const CList<T> &v);

                // General manipulation functions.
                void            Clear               ();

                void            AddToFront          (T item);
                void            AddToEnd            (T item);

                void            Insert              (T item, u32 index);
                void            InsertBefore        (T item, T at);
                void            InsertAfter         (T item, T at);
                void            InsertBefore        (T item, CListNode<T>* at);
                void            InsertAfter         (T item, CListNode<T>* at);

                u32             IndexOf             (T item);

                T               RemoveFromFront     ();
                T               RemoveFromEnd       ();
                void            Remove              (T item);
                void            Remove              (CListNode<T>* node);
                void            RemoveIndex         (u32 index);

                T               PeekFront           ();
                T               PeekEnd             ();
                void            Reverse             ();
                void            Sort                (SORT_FUNCTION func, bool ascending=true);

                u32             Count               (T item);

                bool            Contains            (T item);

        };

        // Sorting functions
        template <typename T>
        s32 CList<T>::SortAsInteger(s32& x, s32& y)
        {
            return y - x;
        }

        template <typename T>
        s32 CList<T>::SortAsString(CString& x, CString& y)
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

        // List functions!
        template <typename T>
        void CList<T>::Initialize()
        {
            _length = 0;

            _head = GetListAllocator()->NewObj<CListNode<T>>();// new CListNode<T>();
            _head->Next = _head;
            _head->Prev = _head;
        }

        template <typename T>
        CList<T>::~CList()
        {
            Clear();
			GetListAllocator()->FreeObj(&_head);
        }

        template <typename T>
        CList<T>::CList()
        {
            Initialize();
        }

        template <typename T>
        CList<T>::CList(const CList<T>& v)
        {
            Initialize();

            for (CListNode<T>* node = v.Start(); node != NULL; node = v.Next(node))
            {
                AddToEnd(node->Value);
            }
        }

        template <typename T>
        T& CList<T>::operator[](u32 index)
        {
            u32 offset = 0;

            for (CListNode<T>* node = Start(); node != NULL; node = Next(node))
            {
                if (offset++ == index)
                {
                    return node->Value;
                }
            }

            LOG_ASSERT(0);
        }

        template <typename T>
        void CList<T>::operator=(const CList<T> &v)
        {
            Clear();

            for (CListNode<T>* node = v.Start(); node != NULL; node = v.Next(node))
            {
                AddToEnd(node->Value);
            }
        }

        template <typename T>
        void CList<T>::Clear()
        {
            CListNode<T>* node = _head->Next;
            while (node != _head)
            {
                CListNode<T>* next = node->Next;
                GetListAllocator()->FreeObj(&next);
                node = next;
            }

            _head->Next = _head;
            _head->Prev = _head;

            _length = 0;
        }

        template <typename T>
        void CList<T>::AddToFront(T item)
        {
            InsertAfter(item, _head);
        }

        template <typename T>
        void CList<T>::AddToEnd(T item)
        {
            InsertBefore(item, _head);
        }

        template <typename T>
        void CList<T>::Insert(T item, u32 index)
        {
            u32 offset = 0;

            CListNode<T>* node = _head;
            do
            {
                if (offset++ == index)
                {
                    InsertAfter(node);
                    return;
                }
                node = node->Next;
            }
            while (node != _head);

            InsertBefore(item, _head);
        }

        template <typename T>
        void CList<T>::InsertBefore(T item, T at)
        {
            CListNode<T>* node = _head->Next;
            while (node != _head)
            {
                if (node->Value == at)
                {
                    InsertBefore(node, node);
                    return;
                }
                node = node->Next;
            }

            LOG_ASSERT(0);
        }

        template <typename T>
        void CList<T>::InsertAfter(T item, T at)
        {
            CListNode<T>* node = _head->Next;
            while (node != _head)
            {
                if (node->Value == at)
                {
                    InsertAfter(node, node);
                    return;
                }
                node = node->Next;
            }

            LOG_ASSERT(0);
        }

        template <typename T>
        void CList<T>::InsertAfter(T item, CListNode<T>* at)
        {
            CListNode<T>* node = GetListAllocator()->NewObj<CListNode<T>>();//new CListNode<T>();
            node->Value = item;
            node->Prev  = at;
            node->Next  = at->Next;
            node->Next->Prev = node;
            at->Next    = node;

            _length++;
        }

        template <typename T>
        void CList<T>::InsertBefore(T item, CListNode<T>* at)
        {
            CListNode<T>* node = GetListAllocator()->NewObj<CListNode<T>>();// = new CListNode<T>();
            node->Value = item;
            node->Next  = at;
            node->Prev  = at->Prev;
            node->Prev->Next = node;
            at->Prev    = node;

            _length++;
        }

        template <typename T>
        T CList<T>::RemoveFromFront()
        {
            if (_length == 0)
            {
                LOG_ASSERT(0);
            }
            T v = _head->Next->Value;
            Remove(_head->Next);
            return v;
        }

        template <typename T>
        T CList<T>::RemoveFromEnd()
        {
            if (_length == 0)
            {
                LOG_ASSERT(0);
            }
            T val = _head->Prev->Value;
            Remove(_head->Prev);
            return val;
        }

        template <typename T>
        void CList<T>::Remove(CListNode<T>* node)
        {
            node->Next->Prev = node->Prev;
            node->Prev->Next = node->Next;
            GetListAllocator()->FreeObj(&node);
            _length--;
        }

        template <typename T>
        void CList<T>::Remove(T item)
        {
            for (CListNode<T>* node = Start(); node != NULL; node = Next(node))
            {
                if (node->Value == item)
                {
                    Remove(node);
                    return;
                }
            }
        }

        template <typename T>
        void CList<T>::RemoveIndex(u32 index)
        {
            u32 offset = 0;
            for (CListNode<T>* node = Start(); node != NULL; node = Next(node))
            {
                if (offset++ == index)
                {
                    Remove(node);
                    return;
                }
            }

            LOG_ASSERT(0);
        }

        template <typename T>
        u32 CList<T>::IndexOf(T item)
        {
            s32 index = 0;

            for (CListNode<T>* node = Start(); node != NULL; node = Next(node))
            {
                if (node->Value == item)
                {
                    return index;
                }
                index++;
            }

            LOG_ASSERT(0);
        }

        template <typename T>
        u32 CList<T>::Count(T item)
        {
            u32 count = 0;

            for (CListNode<T>* node = Start(); node != NULL; node = Next(node))
            {
                if (node->Value == item)
                {
                    count++;
                }
            }

            return count;
        }

        template <typename T>
        bool CList<T>::Contains(T item)
        {
            for (CListNode<T>* node = Start(); node != NULL; node = Next(node))
            {
                if (node->Value == item)
                {
                    return true;
                }
            }

            return false;
        }

        template <typename T>
        void CList<T>::Reverse()
        {
            CListNode<T>* pred = _head;
            CListNode<T>* succ =  pred->Next;

            do
            {
                CListNode<T>* link = succ->Next;
                pred->Prev = succ;
                succ->Next = pred;
                pred = succ;
                succ = link;
            } while (pred != _head);
        }

        template <typename T>
        void CList<T>::Sort(SORT_FUNCTION compareFunc, bool ascending)
        {
            s32 ccsgn = -1;
            if (ascending == true)
                ccsgn = 1;

            s32 insize = 1;
            while (true)
            {
                s32             merges  = 0;
                CListNode<T>*  tail    = _head;
                CListNode<T>*  p       = _head->Next;

                while (p != _head)
                {
                    merges += 1;

                    CListNode<T>* q = p->Next;
                    s32 qsize        = insize;
                    s32 psize        = 1;

                    while (psize < insize && q != _head)
                    {
                        psize += 1;
                        q = q->Next;
                    }

                    while (true)
                    {
                        CListNode<T>* t = NULL;
                        if (psize != 0 && qsize != 0 && q != _head)
                        {
                            s32 cc = compareFunc(p->Value, q->Value) * ccsgn;
                            if (cc <= 0)
                            {
                                t = p;
                                p = p->Next;
                                psize -= 1;
                            }
                            else
                            {
                                t = q;
                                q = q->Next;
                                qsize -= 1;
                            }
                        }
                        else if (psize != 0)
                        {
                            t = p;
                            p = p->Next;
                            psize -= 1;
                        }
                        else if (qsize != 0 and q != _head)
                        {
                            t = q;
                            q = q->Next;
                            qsize -= 1;
                        }
                        else
                        {
                            break;
                        }

                        t->Prev = tail;
                        tail->Next = t;
                        tail = t;
                    }

                    p = q;
                }

                tail->Next = _head;
                _head->Prev = tail;

                if (merges <= 1)
                    return;

                insize *= 2;
            }
        }

        template <typename T>
        T CList<T>::PeekFront()
        {
            if (_length == 0)
            {
                LOG_ASSERT(0);
            }
            return _head->Next->Value;
        }

        template <typename T>
        T CList<T>::PeekEnd()
        {
            if (_length == 0)
            {
                LOG_ASSERT(0);
            }
            return _head->Prev->Value;
        }

    }

}

