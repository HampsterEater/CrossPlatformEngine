///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptStateASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"
#include "CScriptStateSymbol.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptStateASTNode::CScriptStateASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	StateSymbol = NULL;
	IsDefault = false;
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptStateASTNode::~CScriptStateASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptStateASTNode::GetName()
{
	return "<state>";
}

void CScriptStateASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	Symbols::CScriptSymbol* node = FindSymbol(_token.Literal, true);
	if (node != NULL)
	{
		gen->Error(this, S("Encountered identifier redefinition '%s'").Format(_token.Literal.c_str()));
		return;
	}
	else
	{
		Symbols::CScriptStateSymbol* symbol = GetScriptAllocator()->NewObj<Symbols::CScriptStateSymbol>(this);
		symbol->IsDefault = IsDefault;

		StateSymbol = symbol;

		AddSymbolToParent(symbol);
	}

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptStateASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	return 0;
}