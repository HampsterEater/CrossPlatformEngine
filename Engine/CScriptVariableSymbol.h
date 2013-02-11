///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"
#include "CScriptSymbol.h"
#include "CScriptLexer.h"

namespace Engine
{
    namespace Scripting
    {
		namespace AST
		{
			class CScriptASTNode;
		}
		namespace Symbols
		{

			// A symbolic representation of a variable.
			class CScriptVariableSymbol : public CScriptSymbol
			{
				private:

				public:
					u32	Index;
					bool IsGlobal;

					CScriptVariableSymbol					(Engine::Scripting::AST::CScriptASTNode* node);
					CScriptVariableSymbol					(CScriptToken token);
					~CScriptVariableSymbol					();

					virtual ScriptSymbolTypes	GetType	();

			};

		}
	}
}