///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptSwitchCaseASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptSwitchCaseASTNode::CScriptSwitchCaseASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptSwitchCaseASTNode::~CScriptSwitchCaseASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptSwitchCaseASTNode::GetName()
{
	return "<case>";
}

void CScriptSwitchCaseASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	_continueJumpTarget	= GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this); 
	AddSymbol(_continueJumpTarget);

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptSwitchCaseASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	_children[_children.Size() - 1]->GenerateInstructions(gen);
	return 0;
}