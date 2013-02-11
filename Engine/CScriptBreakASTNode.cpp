///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptBreakASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptInstruction.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;
using namespace Engine::Scripting::Instructions;

CScriptBreakASTNode::CScriptBreakASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
	Loop = NULL;
}

CScriptBreakASTNode::~CScriptBreakASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptBreakASTNode::GetName()
{
	return "<break>";
}

void CScriptBreakASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptBreakASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// Generate instructions.
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JMP, 
					  CreateJumpTargetOperand(Loop->GetBreakJumpTarget()));

	// Generate children.
	CScriptASTNode::GenerateInstructions(gen);

	return 0;
}