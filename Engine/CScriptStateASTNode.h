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

		namespace Symbols
		{
			class CScriptStateSymbol;
		}

		namespace AST
		{

			// Node containing an state block declaration.
			class CScriptStateASTNode : public CScriptASTNode
			{
				private:

				public:
					bool IsDefault;
					Symbols::CScriptStateSymbol* StateSymbol;

					CScriptStateASTNode								(Engine::Scripting::CScriptToken token, CScriptASTNode* parent);
					~CScriptStateASTNode							();
					
					virtual Engine::Containers::CString	 GetName	();
					
					virtual void						 GenerateSymbols		(CScriptGenerator* gen);
					virtual u32							 GenerateInstructions	(CScriptGenerator* gen);

			};

		}
	}
}