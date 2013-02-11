///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptContinueASTNode.h"
#include "CScriptGenerator.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptContinueASTNode::CScriptContinueASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
	Loop = NULL;
}

CScriptContinueASTNode::~CScriptContinueASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptContinueASTNode::GetName()
{
	return "<continue>";
}

void CScriptContinueASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptContinueASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// Generate instructions.
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JMP, 
					  CreateJumpTargetOperand(Loop->GetContinueJumpTarget()));

	// Generate children.
	CScriptASTNode::GenerateInstructions(gen);

	return 0;
}