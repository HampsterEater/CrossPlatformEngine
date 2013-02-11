///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CString.h"
#include "CArray.h"

#include "CScriptObject.h"

using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Instructions;

namespace Engine
{
    namespace Scripting
    {
		namespace Objects
		{

			// String object! Stores the current state of a string.
			class CScriptStringObject : public CScriptObject
			{
			private:
				Engine::Containers::CString _string;

			public:

				CScriptStringObject									(CScriptExecutionContext* context, const Engine::Containers::CString& str);

				// Metadata.
				virtual Engine::Containers::CString	GetName			();

				// Casting.
				virtual bool						CoerceToInt		(CScriptExecutionContext* context, s32& result);
				virtual bool						CoerceToFloat	(CScriptExecutionContext* context, f32& result);
				virtual bool						CoerceToString	(CScriptExecutionContext* context, Engine::Containers::CString& result);
				virtual bool						CoerceToBool	(CScriptExecutionContext* context, bool& result);

				// General instructions.
				virtual bool Assign									(CScriptExecutionContext* context, CScriptValue& dest);

				virtual bool Add									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool Mul									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);
				virtual bool Mod									(CScriptExecutionContext* context, CScriptValue& lvalue, CScriptValue& rvalue);

				// Comparison.
				virtual bool Cmp									(CScriptExecutionContext* context, const CScriptValue& dest, s32& result);

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