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
#include "CScriptFunctionASTNode.h"

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
			class CScriptFunctionSymbol : public CScriptSymbol
			{
				private:

				public:
					u32						EntryPoint;
					u32						ParameterCount;
					u32						LocalCount;
					CScriptStateSymbol*		State;
					u32						Index;
					AST::ScriptFunctionType	Type;

					CScriptFunctionSymbol					(Engine::Scripting::AST::CScriptASTNode* node);
					CScriptFunctionSymbol					(CScriptToken token);
					~CScriptFunctionSymbol					();

					virtual ScriptSymbolTypes	GetType		();

			};

		}
	}
}