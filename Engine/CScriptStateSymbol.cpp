///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptStateSymbol.h"
#include "CScriptCompileContext.h"
#include "CScriptASTNode.h"
#include "CScriptLexer.h"

#include "CScriptStateSymbol.h"

using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::AST;

CScriptStateSymbol::CScriptStateSymbol(Engine::Scripting::AST::CScriptASTNode* node)
{
	_astNode	= node;
	IsDefault = false;
	_token = _astNode->GetToken();
}

CScriptStateSymbol::CScriptStateSymbol(Engine::Scripting::CScriptToken token)
{
	_token = token;
}

CScriptStateSymbol::~CScriptStateSymbol()
{

}

ScriptSymbolTypes CScriptStateSymbol::GetType()
{
	return SCRIPT_SYMBOL_TYPE_STATE;
}