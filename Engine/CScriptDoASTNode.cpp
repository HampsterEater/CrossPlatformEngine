///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptDoASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptDoASTNode::CScriptDoASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptDoASTNode::~CScriptDoASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptDoASTNode::GetName()
{
	return "<do>";
}

void CScriptDoASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	_breakJumpTarget	= GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this); 
	AddSymbol(_breakJumpTarget);

	_continueJumpTarget = GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this);
	AddSymbol(_continueJumpTarget);

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptDoASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// While

	// continue:
	//		<block>
	//		output_reg = <expression>
	//		cmp output_reg, zero_reg
	//		jne continue
	// break:

	// Forever
	
	// continue:
	//		<block>
	//		jmp continue
	// break:

	// start:
	_continueJumpTarget->Bind(gen, this);
	
	// <block>
	_children[0]->GenerateInstructions(gen);

	// While (x)
	if (_children.Size() > 1)
	{
		// output_reg = <expression>
		u32 output_reg = _children[1]->GenerateInstructions(gen);

		// cmp output_reg, zero_reg
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_CMP, 
						  CreateRegisterOperand(output_reg),
						  CreateRegisterOperand(SCRIPT_CONST_REGISTER_ZERO));			
		// jne start
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JNE, 
						  CreateJumpTargetOperand(_continueJumpTarget));
	}
	// Forever
	else
	{		
		// jmp start
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JMP, 
						  CreateJumpTargetOperand(_continueJumpTarget));
	}

	// break:
	_breakJumpTarget->Bind(gen, this);

	return 0;
}