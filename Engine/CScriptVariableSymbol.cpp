///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptVariableSymbol.h"
#include "CScriptCompileContext.h"
#include "CScriptASTNode.h"
#include "CScriptLexer.h"

using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::AST;

CScriptVariableSymbol::CScriptVariableSymbol(Engine::Scripting::AST::CScriptASTNode* node)
{
	_astNode = node;
	_token = _astNode->GetToken();
	Index = 0;
	IsGlobal = false;
}

CScriptVariableSymbol::CScriptVariableSymbol(Engine::Scripting::CScriptToken token)
{
	_token = token;
}

CScriptVariableSymbol::~CScriptVariableSymbol()
{

}

ScriptSymbolTypes CScriptVariableSymbol::GetType()
{
	return SCRIPT_SYMBOL_TYPE_VARIABLE;
}