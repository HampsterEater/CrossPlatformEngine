///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CString.h"
#include "CArray.h"

namespace Engine
{
    namespace Scripting
    {
		class CScriptValue;
		class CScriptExecutionContext;
		class CScriptVirtualMachine;
		class CScriptCallContext;

		namespace Objects
		{
			class CScriptIteratorObject;

			// This is the base class that all script objects need to extend from, it 
			// provides the functionality required to perform different operators 
			// and such on the objects.
			class CScriptObject
			{
			protected:
				// Reference counting.
				s32				_refCount;
				u32				_allocCallDepth;

				// GC Linked list stuff.
				CScriptObject*	_next;
				CScriptObject*	_prev;
				u32				_generation;
				bool			_finalized;

			public:

				CScriptObject										();

				// Reference counting.
				void IncRef											();
				void DecRef											();
				s32  GetRefs										();

				virtual void Finalize								(CScriptExecutionContext* context);
				
				// Metadata.
				virtual Engine::Containers::CString	GetName			()=0;

				// Casting.
				virtual bool						CoerceToInt		(CScriptExecutionContext* context, s32& result);
				virtual bool						CoerceToFloat	(CScriptExecutionContext* context, f32& result);
				virtual bool						CoerceToString	(CScriptExecutionContext* context, Engine::Containers::CString& result);
				virtual bool						CoerceToBool	(CScriptExecutionContext* context, bool& result);

				virtual bool						CastTo			(CScriptExecutionContext* context, CScriptValue& dest, const Engine::Containers::CString& type);
				virtual bool						DerivedFrom		(CScriptExecutionContext* context, const Engine::Containers::CString& type);

				// Iterators.
				virtual CScriptIteratorObject*		CreateIterator	(CScriptExecutionContext* context);

				// General instructions.
				virtual bool Assign									(CScriptExecutionContext* context, CScriptValue& dest);

				virtual bool Add									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool Sub									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool Mul									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool Div									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool Inc									(CScriptExecutionContext* context, CScriptValue& lvalue);
				virtual bool Dec									(CScriptExecutionContext* context, CScriptValue& lvalue);
				virtual bool Neg									(CScriptExecutionContext* context, CScriptValue& lvalue);
				virtual bool Abs									(CScriptExecutionContext* context, CScriptValue& lvalue);
				virtual bool Mod									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);

				virtual bool BWOr									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool BWXor									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool BWAnd									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool BWShl									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool BWShr									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool BWNot									(CScriptExecutionContext* context, CScriptValue& lvalue);

				// Comparison.
				virtual bool Cmp									(CScriptExecutionContext* context, const CScriptValue& dest, s32& result);

				// Subscript operations.
				virtual bool GetIndex								(CScriptExecutionContext* context, CScriptValue& dest, const CScriptValue& index);
				virtual bool SetIndex								(CScriptExecutionContext* context, const CScriptValue& dest, const CScriptValue& index, const CScriptValue& value);

				// Invokation operations.
				virtual bool Invoke									(CScriptExecutionContext* context, u32 parameterCount);

				// Indirection operations.
				virtual bool GetAttribute							(CScriptExecutionContext* context, CScriptValue& dest, const Engine::Containers::CString& name);
				virtual bool SetAttribute							(CScriptExecutionContext* context, const Engine::Containers::CString& name, const CScriptValue& value);
				
				friend class CScriptVirtualMachine;
				friend class CScriptExecutionContext;
				friend class CScriptCallContext;
			};

		}
	}
}