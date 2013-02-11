///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptIdentifierASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptSymbol.h"
#include "CScriptFunctionSymbol.h"
#include "CScriptVariableSymbol.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;
using namespace Engine::Scripting::Symbols;

CScriptIdentifierASTNode::CScriptIdentifierASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptIdentifierASTNode::~CScriptIdentifierASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptIdentifierASTNode::GetName()
{
	return _token.Literal;
}

void CScriptIdentifierASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptIdentifierASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	CScriptSymbol* symbol = dynamic_cast<CScriptSymbol*>(FindSymbol(_token.Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_FUNCTION|Symbols::SCRIPT_SYMBOL_TYPE_VARIABLE));
	if (symbol == NULL)
	{
		gen->Error(this, "Attempt to access undefined identifier '"+_token.Literal+"'.");
		return 0;
	}
	
	u32 output = gen->AllocateRegister(this);

	// lsym reg0, sym
	LoadRegister(gen, symbol, output);

	gen->DeallocateRegister(this, output);
	return output;
}

