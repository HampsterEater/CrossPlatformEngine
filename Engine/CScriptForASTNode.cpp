///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptForASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptForASTNode::CScriptForASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptForASTNode::~CScriptForASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptForASTNode::GetName()
{
	return "<for>";
}

void CScriptForASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	_breakJumpTarget	= GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this); 
	AddSymbol(_breakJumpTarget);

	_continueJumpTarget = GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this);
	AddSymbol(_continueJumpTarget);

	_startJumpTarget = GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this);
	AddSymbol(_startJumpTarget);

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptForASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// <assignment>
	// start:
	//	output_reg = <expression>
	//	cmp output_reg, zero_reg
	//	jeq break
	//	<block>
	// continue:
	//	<increment>
	//	jmp start
	// break:

	// <assignment>
	_children[0]->GenerateInstructions(gen);

	// start:
	_startJumpTarget->Bind(gen, this);

		// output_reg = <expression>
		u32 output_reg = _children[1]->GenerateInstructions(gen);

		// cmp output_reg, zero_reg
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_CMP, 
						  CreateRegisterOperand(output_reg),
						  CreateRegisterOperand(SCRIPT_CONST_REGISTER_ZERO));		

		// jeq break
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JEQ, 
						  CreateJumpTargetOperand(_breakJumpTarget));

		// <block>
		_children[3]->GenerateInstructions(gen);

	// continue:
	_continueJumpTarget->Bind(gen, this);

		// <increment>
		_children[2]->GenerateInstructions(gen);

		// jmp start
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JMP, 
						  CreateJumpTargetOperand(_startJumpTarget));

	// break:
	_breakJumpTarget->Bind(gen, this);

	return 0;
}