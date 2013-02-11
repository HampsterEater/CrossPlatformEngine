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

#include "CScriptJumpTargetSymbol.h"

namespace Engine
{
    namespace Scripting
    {
		struct CScriptToken;

		namespace AST
		{

			// Node containing a for loop declaration.
			class CScriptForASTNode : public CScriptASTNode
			{
				private:
					Symbols::CScriptJumpTargetSymbol* _startJumpTarget;

				public:
					CScriptForASTNode								(Engine::Scripting::CScriptToken token, CScriptASTNode* parent);
					~CScriptForASTNode								();
					
					virtual Engine::Containers::CString	 GetName	();
					
					virtual void						 GenerateSymbols		(CScriptGenerator* gen);
					virtual u32							 GenerateInstructions	(CScriptGenerator* gen);

			};

		}
	}
}