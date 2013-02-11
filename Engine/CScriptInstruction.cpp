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
	_astNode = node;
}

CScriptFunctionSymbol::~CScriptFunctionSymbol()
{

}
