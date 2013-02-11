///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"
#include "CScriptSymbol.h"
#include "CScriptGenerator.h"
#include "CScriptLexer.h"

namespace Engine
{
    namespace Scripting
    {
		namespace Instructions
		{
			class CScriptInstruction;
		}
		namespace AST
		{
			class CScriptASTNode;
		}
		namespace Symbols
		{

			// A symbolic representation of a jump target.
			class CScriptJumpTargetSymbol : public CScriptSymbol
			{
				private:

				public:
					//Instructions::CScriptInstruction*	Instruction;
					u32									Index;
					
					CScriptJumpTargetSymbol				(Engine::Scripting::AST::CScriptASTNode* node);
					CScriptJumpTargetSymbol				(CScriptToken token);
					~CScriptJumpTargetSymbol			();

					virtual ScriptSymbolTypes	GetType	();

					void Bind							(CScriptGenerator* gen, AST::CScriptASTNode* node);
			};

		}
	}
}