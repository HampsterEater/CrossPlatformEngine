///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright � 2011 Timothy Leonard
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

			// Node containing a return declaration.
			class CScriptReturnASTNode : public CScriptASTNode
			{
				private:

				public:
					CScriptReturnASTNode								(Engine::Scripting::CScriptToken token, CScriptASTNode* parent);
					~CScriptReturnASTNode								();
					
					virtual Engine::Containers::CString	 GetName		();
					
					virtual void						 GenerateSymbols		(CScriptGenerator* gen);
					virtual u32							 GenerateInstructions	(CScriptGenerator* gen);

			};

		}
	}
}