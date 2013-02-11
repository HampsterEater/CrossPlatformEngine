///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CArray.h"
#include "CScriptLexer.h"
#include "CScriptASTNode.h"

namespace Engine
{
    namespace Scripting
    {
		struct CScriptToken;

		namespace AST
		{

			// Types of variables that can be declared.
			enum ScriptFunctionType 
			{
				SCRIPT_FUNCTION_NORMAL,
				SCRIPT_FUNCTION_GENERATOR,
				SCRIPT_FUNCTION_EVENT,
				SCRIPT_FUNCTION_OPERATOR
			};

			// Node containing a function statement.
			class CScriptFunctionASTNode : public CScriptASTNode
			{
				protected:
					ScriptFunctionType _type;

				public:
					bool Assigned;
					u32 ParameterCount;
					u32 LocalCount;

					CScriptFunctionASTNode								(Engine::Scripting::CScriptToken token, CScriptASTNode* parent, ScriptFunctionType type);
					~CScriptFunctionASTNode								();
					
					virtual	ScriptFunctionType			 GetType		();
					virtual Engine::Containers::CString	 GetName		();

					ScriptFunctionType					 GetFunctionType();
					
					virtual void						 GenerateSymbols		(CScriptGenerator* gen);
					virtual u32							 GenerateInstructions	(CScriptGenerator* gen);

			};

		}
	}
}