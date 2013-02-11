///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptExpressionASTNode.h"
#include "CScriptGenerator.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptExpressionASTNode::CScriptExpressionASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptExpressionASTNode::~CScriptExpressionASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptExpressionASTNode::GetName()
{
	return "<expression>";
}

void CScriptExpressionASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptExpressionASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// Begin generating the first operator in the expression.
	return _children[0]->GenerateInstructions(gen);
}