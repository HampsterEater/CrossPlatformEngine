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

			// A symbolic representation of a string.
			class CScriptStringSymbol : public CScriptSymbol
			{
				private:

				public:
					CScriptStringSymbol					(Engine::Scripting::AST::CScriptASTNode* node);
					CScriptStringSymbol					(CScriptToken token);
					~CScriptStringSymbol				();

					virtual ScriptSymbolTypes	GetType	();

			};

		}
	}
}