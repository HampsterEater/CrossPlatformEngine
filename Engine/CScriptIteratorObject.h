///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CString.h"
#include "CArray.h"

#include "CScriptObject.h"

#include "CScriptVirtualMachine.h"

using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Instructions;

namespace Engine
{
    namespace Scripting
    {
		namespace Objects
		{

			// An iterator object is used to iterate over a specific object stored within 
			// a given script.
			class CScriptIteratorObject : public CScriptObject
			{
			protected:
				CScriptObject* _obj;
				u32			   _index;

			public:
				
				CScriptIteratorObject									();
				CScriptIteratorObject									(CScriptExecutionContext* context, CScriptObject* obj);

				// Metadata.
				virtual Engine::Containers::CString		GetName			();
				virtual void							Finalize		(CScriptExecutionContext* context);
				
				// Casting.
				virtual bool							CoerceToString	(CScriptExecutionContext* context, Engine::Containers::CString& result);

				// Iteration method.
				virtual bool							IsFinished		(CScriptExecutionContext* context) = 0;
				virtual CScriptValue					NextValue		(CScriptExecutionContext* context) = 0;

			};

		}
	}
}