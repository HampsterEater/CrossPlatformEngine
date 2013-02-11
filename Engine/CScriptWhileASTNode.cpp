///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptWhileASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptWhileASTNode::CScriptWhileASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptWhileASTNode::~CScriptWhileASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptWhileASTNode::GetName()
{
	return "<while>";
}

void CScriptWhileASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	_breakJumpTarget	= GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this); 
	AddSymbol(_breakJumpTarget);

	_continueJumpTarget = GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this);
	AddSymbol(_continueJumpTarget);

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptWhileASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// start:
	//	output_reg = <expression>
	//	cmp output_reg, zero_reg
	//	jeq break
	//		<block>
	//	jmp start
	// break:

	// start:
	_continueJumpTarget->Bind(gen, this);

	// Generate expression.
	u32 output_reg = _children[0]->GenerateInstructions(gen);

	// cmp output_reg, zero_reg
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_CMP, 
					  CreateRegisterOperand(output_reg),
					  CreateRegisterOperand(SCRIPT_CONST_REGISTER_ZERO));	
	// jeq break
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JEQ, 
					  CreateJumpTargetOperand(_breakJumpTarget));

	// <block>
	_children[1]->GenerateInstructions(gen);

	// jmp start
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JMP, 
					  CreateJumpTargetOperand(_continueJumpTarget));

	// break:
	_breakJumpTarget->Bind(gen, this);

	return 0;
}
