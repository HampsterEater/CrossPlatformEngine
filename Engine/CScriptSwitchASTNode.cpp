///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptSwitchASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"
#include "CScriptSwitchCaseASTNode.h"
#include "CScriptSwitchDefaultASTNode.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptSwitchASTNode::CScriptSwitchASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptSwitchASTNode::~CScriptSwitchASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptSwitchASTNode::GetName()
{
	return "<switch>";
}

void CScriptSwitchASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	_breakJumpTarget	= GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this); 
	AddSymbol(_breakJumpTarget);

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptSwitchASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	//		output_reg = <expression>
	//
	//		case_reg = <expression>
	//		cmp case_reg, output_reg
	//		jeq case_0
	//
	//		<default-block>
	//		jmp break
	// case_0:
	//		<block>
	//		jmp break
	// break:

	// output_reg = <expression>
	u32 output_reg = _children[0]->GenerateInstructions(gen);
	gen->AllocateRegister(this, output_reg);

	// Build the case lookup table.
	for (u32 i = 0; i < _children.Size(); i++)
	{
		CScriptSwitchCaseASTNode* node = dynamic_cast<CScriptSwitchCaseASTNode*>(_children[i]);
		if (node != NULL)
		{		
			for (u32 i = 0; i < node->GetChildren().Size() - 1; i++)
			{
				// Parse case block.
				u32 case_reg = node->GetChildren()[i]->GenerateInstructions(gen);
			
				// cmp case_reg, zero_reg
				CreateInstruction(gen, Instructions::SCRIPT_OPCODE_CMP, 
								  CreateRegisterOperand(case_reg),
								  CreateRegisterOperand(output_reg));	
	
				// jeq case_x
				CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JEQ, 
				 					   CreateJumpTargetOperand(node->GetContinueJumpTarget()));
			}
		}
	}

	// Build default block if it exists.
	for (u32 i = 0; i < _children.Size(); i++)
	{
		CScriptSwitchDefaultASTNode* node = dynamic_cast<CScriptSwitchDefaultASTNode*>(_children[i]);
		if (node != NULL)
		{
			// Parse case block.
			node->GenerateInstructions(gen);
		}
	}

	// Bail motherfuckers bail!
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JMP, 
					  CreateJumpTargetOperand(_breakJumpTarget));

	// Build case blocks.
	for (u32 i = 0; i < _children.Size(); i++)
	{
		CScriptSwitchCaseASTNode* node = dynamic_cast<CScriptSwitchCaseASTNode*>(_children[i]);
		if (node != NULL)
		{
			node->GetContinueJumpTarget()->Bind(gen, this);

			// Parse case block.
			node->GenerateInstructions(gen);

			// jmp break
			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JMP, 
				 				   CreateJumpTargetOperand(_breakJumpTarget));
		}
	}

	// break:
	_breakJumpTarget->Bind(gen, this);

	// Deallocate the output register.
	gen->DeallocateRegister(this, output_reg);

	return 0;
}