///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptJumpTargetSymbol.h"
#include "CScriptCompileContext.h"
#include "CScriptASTNode.h"
#include "CScriptLexer.h"
#include "CScriptGenerator.h"

using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::AST;

CScriptJumpTargetSymbol::CScriptJumpTargetSymbol(Engine::Scripting::AST::CScriptASTNode* node)
{
	_astNode = node;
	Index = 0;
	_token = _astNode->GetToken();
}

CScriptJumpTargetSymbol::CScriptJumpTargetSymbol(Engine::Scripting::CScriptToken token)
{
	_token = token;
}

CScriptJumpTargetSymbol::~CScriptJumpTargetSymbol()
{

}

void CScriptJumpTargetSymbol::Bind(Engine::Scripting::CScriptGenerator* gen, CScriptASTNode* node)
{
	// Count instructions.
	Index = gen->GetContext()->GetInstructions().Size();

}

ScriptSymbolTypes CScriptJumpTargetSymbol::GetType()
{
	return SCRIPT_SYMBOL_TYPE_JUMPTARGET;
}