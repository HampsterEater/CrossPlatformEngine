///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CString.h"
#include "CArray.h"

#include "CScriptObject.h"
#include "CScriptIteratorObject.h"

#include "CScriptVirtualMachine.h"

using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Instructions;

namespace Engine
{
    namespace Scripting
    {
		namespace Objects
		{
			class CScriptDictObject;

			// Iterator for list.
			class CScriptDictIteratorObject : public CScriptIteratorObject
			{
			public:
				CScriptDictIteratorObject							(CScriptExecutionContext* context, CScriptObject* obj);

				virtual bool			IsFinished					(CScriptExecutionContext* context);
				virtual CScriptValue	NextValue					(CScriptExecutionContext* context);
			};

			// Dict object! Stores the current state of a list.
			class CScriptDictObject : public CScriptObject
			{
			private:
				Engine::Containers::CArray<CScriptValue> _keys;
				Engine::Containers::CArray<CScriptValue> _values;

			public:

				CScriptDictObject									(CScriptExecutionContext* context);

				// Metadata.
				virtual Engine::Containers::CString			GetName		();

				virtual void Finalize									(CScriptExecutionContext* context);

				// Array related stuff.
				Engine::Containers::CArray<CScriptValue>&	GetKeys		();
				Engine::Containers::CArray<CScriptValue>&	GetValues	();
				s32											GetIndex	(CScriptExecutionContext* context, const CScriptValue& key);
				void										AddItem		(CScriptExecutionContext* context, const CScriptValue& key, const CScriptValue& value);
				
				// Iterators.
				virtual CScriptIteratorObject*		CreateIterator	(CScriptExecutionContext* context);

				// Casting.
				virtual bool						CoerceToInt		(CScriptExecutionContext* context, s32& result);
				virtual bool						CoerceToFloat	(CScriptExecutionContext* context, f32& result);
				virtual bool						CoerceToString	(CScriptExecutionContext* context, Engine::Containers::CString& result);
				virtual bool						CoerceToBool	(CScriptExecutionContext* context, bool& result);

				// Subscript operations.
				virtual bool GetIndex								(CScriptExecutionContext* context, CScriptValue& dest, const CScriptValue& index);
				virtual bool SetIndex								(CScriptExecutionContext* context, const CScriptValue& dest, const CScriptValue& index, const CScriptValue& value);

				// Indirection operations.
				virtual bool GetAttribute							(CScriptExecutionContext* context, CScriptValue& dest, const Engine::Containers::CString& name);
				virtual bool SetAttribute							(CScriptExecutionContext* context, const Engine::Containers::CString& name, const CScriptValue& value);

			};
		}
	}
}