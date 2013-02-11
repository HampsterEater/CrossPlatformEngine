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
			class CScriptContextObject;

			// Iterator for context object (which basically calls the
			// generator continuously until a return is encountered).
			class CScriptContextIteratorObject : public CScriptIteratorObject
			{
			protected:
				bool				_isFinished;
				CScriptValue		_nextValue;
				CScriptCallContext	_context;

			public:
				CScriptContextIteratorObject						(CScriptExecutionContext* context, CScriptObject* obj);

				virtual bool			IsFinished					(CScriptExecutionContext* context);
				virtual CScriptValue	NextValue					(CScriptExecutionContext* context);

				virtual void			CalcNext					(CScriptExecutionContext* context);

				void					CallbackComplete			();
				void					CallbackNext				(CScriptCallContext& context, CScriptValue& value);

			};

			// Context objects are used to store the current state of the current
			// state of a call stack entry. This is primarily used to keep track
			// of generator properties between calls.
			//
			// This is primarily setup as an object rather than a value-type to allow
			// for garbage collection of locals/parameters when done.
			class CScriptContextObject : public CScriptObject
			{
			private:
				CScriptCallContext _context;

			public:

				CScriptContextObject								(CScriptExecutionContext* context, CScriptCallContext& callContext);

				// Metadata.
				virtual Engine::Containers::CString		GetName		();
				virtual void							Finalize	(CScriptExecutionContext* context);
				
				// Iterators.
				virtual CScriptIteratorObject*		CreateIterator	(CScriptExecutionContext* context);

				// Casting.
				virtual bool						CoerceToString	(CScriptExecutionContext* context, Engine::Containers::CString& result);

				friend class CScriptContextIteratorObject;
			};
		}
	}
}