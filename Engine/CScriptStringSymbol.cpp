///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptStringSymbol.h"
#include "CScriptCompileContext.h"
#include "CScriptASTNode.h"
#include "CScriptLexer.h"

using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::AST;

CScriptStringSymbol::CScriptStringSymbol(Engine::Scripting::AST::CScriptASTNode* node)
{
	_astNode = node;
	_token = _astNode->GetToken();
}

CScriptStringSymbol::CScriptStringSymbol(Engine::Scripting::CScriptToken token)
{
	_token = token;
}

CScriptStringSymbol::~CScriptStringSymbol()
{

}

ScriptSymbolTypes CScriptStringSymbol::GetType()
{
	return SCRIPT_SYMBOL_TYPE_STRING;
}