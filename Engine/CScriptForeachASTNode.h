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

			// Node containing a foreach loop declaration.
			class CScriptForeachASTNode : public CScriptASTNode
			{
				private:

				public:
					bool NewVariable;

					CScriptForeachASTNode							(Engine::Scripting::CScriptToken token, CScriptASTNode* parent);
					~CScriptForeachASTNode							();
					
					virtual Engine::Containers::CString	 GetName	();
					
					virtual void						 GenerateSymbols		(CScriptGenerator* gen);
					virtual u32							 GenerateInstructions	(CScriptGenerator* gen);
					
					// Finds the parent function this node is within, or null
					// if in the global scope.
					AST::CScriptFunctionASTNode*				FindParentFunction();
			};

		}
	}
}