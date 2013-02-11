///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptReturnASTNode.h"
#include "CScriptGenerator.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptReturnASTNode::CScriptReturnASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptReturnASTNode::~CScriptReturnASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptReturnASTNode::GetName()
{
	return "<return>";
}

void CScriptReturnASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptReturnASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// Generate instructions.
	if (_children.Size() > 0)
	{
		u32 output_reg = _children[0]->GenerateInstructions(gen);
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_RET, CreateRegisterOperand(output_reg));	
	}
	else
	{
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_RET);
	}

	return 0;
}