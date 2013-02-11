///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptFunctionSymbol.h"
#include "CScriptCompileContext.h"
#include "CScriptASTNode.h"
#include "CScriptLexer.h"

using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::AST;

CScriptFunctionSymbol::CScriptFunctionSymbol(Engine::Scripting::AST::CScriptASTNode* node)
{
	_astNode	= node;
	EntryPoint	= 0;
	State		= NULL;
	_token		= _astNode->GetToken();
	Index		= 0;
	Type		= AST::SCRIPT_FUNCTION_NORMAL;
}

CScriptFunctionSymbol::CScriptFunctionSymbol(Engine::Scripting::CScriptToken token)
{
	_token = token;
}


CScriptFunctionSymbol::~CScriptFunctionSymbol()
{

}

ScriptSymbolTypes CScriptFunctionSymbol::GetType()
{
	return SCRIPT_SYMBOL_TYPE_FUNCTION;
}