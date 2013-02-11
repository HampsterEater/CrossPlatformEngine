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
#include "CScriptFunctionASTNode.h"

namespace Engine
{
    namespace Scripting
    {
		struct CScriptToken;

		namespace AST
		{

			// Types of variables that can be declared.
			enum ScriptVariableType 
			{
				SCRIPT_VARIABLE_NORMAL,
				SCRIPT_VARIABLE_CONSTANT,
				SCRIPT_VARIABLE_PARAMETER
			};

			// Node containing a var of statements.
			class CScriptVariableASTNode : public CScriptASTNode
			{
				protected:
					ScriptVariableType _type;

				public:
					CScriptVariableASTNode								(Engine::Scripting::CScriptToken token, CScriptASTNode* parent, ScriptVariableType type);
					~CScriptVariableASTNode								();
					
					virtual	ScriptVariableType			 GetType		();
					virtual Engine::Containers::CString	 GetName		();
					
					virtual void						 GenerateSymbols		(CScriptGenerator* gen);
					virtual u32							 GenerateInstructions	(CScriptGenerator* gen);
					
					// Finds the parent function this node is within, or null
					// if in the global scope.
					AST::CScriptFunctionASTNode*				FindParentFunction();

			};

		}
	}
}