///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptIfASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"


using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptIfASTNode::CScriptIfASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptIfASTNode::~CScriptIfASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptIfASTNode::GetName()
{
	return "<if>";
}

void CScriptIfASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	
	// Out jump target.
	if (_children.Size() > 2) // We only need 2 jumps if we have an else block.
	{
		_breakJumpTarget	= GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this); 
		AddSymbol(_breakJumpTarget);
	}

	// Else jump target.
	_continueJumpTarget = GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this);
	AddSymbol(_continueJumpTarget);

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptIfASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// With else block;

	//	output_reg = <expression>
	//	cmp output_reg, zero_reg
	//	jeq else
	//	<true-block>
	//	jmp out
	//else:
	//	<else-block>
	//out:

	// Without;

	//	output_reg = <expression>
	//	cmp output_reg, zero_reg
	//	jeq else
	//	<true-block>
	//else:

	// Generate expression.
	u32 output_reg = _children[0]->GenerateInstructions(gen);

	// cmp output_reg, zero_reg
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_CMP, 
					  CreateRegisterOperand(output_reg),
					  CreateRegisterOperand(SCRIPT_CONST_REGISTER_ZERO));	
	// jeq break
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JEQ, 
					  CreateJumpTargetOperand(_continueJumpTarget));

	// <block>
	_children[1]->GenerateInstructions(gen);

	// Else block.
	if (_children.Size() > 2)
	{
		// jmp out
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JMP, 
						  CreateJumpTargetOperand(_breakJumpTarget));

		// else:
		_continueJumpTarget->Bind(gen, this);
		
		// <else-block>
		_children[2]->GenerateInstructions(gen);
		
		// out:
		_breakJumpTarget->Bind(gen, this);		
	}
	else
	{		
		// else:
		_continueJumpTarget->Bind(gen, this);
	}

	return 0;
}