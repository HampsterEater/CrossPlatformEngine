///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"
#include "CScriptLexer.h"

namespace Engine
{
    namespace Scripting
    {
		struct CScriptToken;

		class CVirtualMachine;

		namespace AST
		{
			class CScriptASTNode;
		}

		namespace Symbols
		{

			// Different types of symbols.
			// Has to be a bitmask because it is used for searching for symbols of multiple types.
			enum ScriptSymbolTypes
			{
				SCRIPT_SYMBOL_TYPE_FUNCTION			= 1,
				SCRIPT_SYMBOL_TYPE_VARIABLE			= 2,
				SCRIPT_SYMBOL_TYPE_JUMPTARGET		= 4,
				SCRIPT_SYMBOL_TYPE_STRING			= 8,
				SCRIPT_SYMBOL_TYPE_STATE			= 16
			};

			// A symbol represents a symbolic reference inside a script. eg. variables, functions, etc.
			class CScriptSymbol
			{
				protected:
					Engine::Scripting::AST::CScriptASTNode*		_astNode;
					CScriptToken _token;

				public:
					CScriptSymbol												();
					~CScriptSymbol												();

					Engine::Scripting::AST::CScriptASTNode*		GetASTNode		();
					CScriptToken&								GetToken		();
					virtual const Engine::Containers::CString	GetIdentifier	();
					virtual ScriptSymbolTypes					GetType			()=0;
			};

		}
	}
}