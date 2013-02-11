///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"
#include "CScriptSymbol.h"
#include "CscriptStateSymbol.h"
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

			// A symbolic representation of a function.
			class CScriptStateSymbol : public CScriptSymbol
			{
				private:

				public:
					bool IsDefault;

					CScriptStateSymbol						(Engine::Scripting::AST::CScriptASTNode* node);
					CScriptStateSymbol						(CScriptToken token);
					~CScriptStateSymbol						();

					virtual ScriptSymbolTypes	GetType		();

			};

		}
	}
}