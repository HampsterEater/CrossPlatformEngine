///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptYieldASTNode.h"
#include "CScriptGenerator.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptYieldASTNode::CScriptYieldASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptYieldASTNode::~CScriptYieldASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptYieldASTNode::GetName()
{
	return "<yield>";
}

void CScriptYieldASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptYieldASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	u32 output_reg = _children[0]->GenerateInstructions(gen);
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_YIELD, CreateRegisterOperand(output_reg));	

	return 0;
}
