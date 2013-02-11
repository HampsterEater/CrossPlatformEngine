///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptSwitchDefaultASTNode.h"
#include "CScriptGenerator.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptSwitchDefaultASTNode::CScriptSwitchDefaultASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptSwitchDefaultASTNode::~CScriptSwitchDefaultASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptSwitchDefaultASTNode::GetName()
{
	return "<default>";
}

void CScriptSwitchDefaultASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptSwitchDefaultASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	_children[0]->GenerateInstructions(gen);
	return 0;
}