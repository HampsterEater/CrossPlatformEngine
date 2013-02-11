///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptForeachASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"
#include "CScriptVariableASTNode.h"
#include "CScriptVariableSymbol.h"
#include "CScriptVirtualMachine.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptForeachASTNode::CScriptForeachASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	NewVariable = false;
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptForeachASTNode::~CScriptForeachASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptForeachASTNode::GetName()
{
	return "<foreach>";
}

void CScriptForeachASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	CScriptASTNode* identifierNode = _children[0];

	if (NewVariable == true)
	{
		// Generate symbols.
		Symbols::CScriptSymbol* node = FindSymbol(identifierNode->GetToken().Literal, true);

		if (node != NULL)
		{
			gen->Error(this, S("Encountered identifier redefinition '%s'").Format(identifierNode->GetToken().Literal.c_str()));
			return;
		}
		else
		{
			// Update counter on function.
			u32 index;
			bool isGlobal;

			CScriptFunctionASTNode* functionNode = FindParentFunction();
			if (functionNode != NULL)
			{
				isGlobal = false;
				index = functionNode->LocalCount;
				functionNode->LocalCount++;
			}
			else
			{
				isGlobal = true;
				index = gen->GetContext()->_globalVariableCount;
				gen->GetContext()->_globalVariableCount++;
			}

			Symbols::CScriptVariableSymbol* symbol = GetScriptAllocator()->NewObj<Symbols::CScriptVariableSymbol>(identifierNode);
			symbol->IsGlobal = isGlobal;
			symbol->Index = index;
			AddSymbol(symbol);
		}
	}
	else
	{
		Symbols::CScriptSymbol* node = FindSymbol(identifierNode->GetToken().Literal, true);

		if (node == NULL)
		{
			gen->Error(this, S("Encountered undefined identifier '%s'").Format(identifierNode->GetToken().Literal.c_str()));
			return;
		}
	}

	// Jump targets.
	_breakJumpTarget	= GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this); 
	AddSymbol(_breakJumpTarget);

	_continueJumpTarget = GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this);
	AddSymbol(_continueJumpTarget);
}

u32 CScriptForeachASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	CScriptASTNode* identifierNode = _children[0];
	Symbols::CScriptSymbol* symbol = FindSymbol(identifierNode->GetToken().Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_VARIABLE);

	//		// Get the enumeration context.
	//		value_reg = <expression>
	//		ITERNEW context_reg, value_reg 
	//		
	// continue:	
	//
	//		// Is there another variable.
	//		ITERDONE reg, context_reg
	//		cmp reg, zero_reg
	//		jne break
	//
	//		// Get next value and assign to variable.
	//		ITERNEXT next_value_reg, context_reg
	//		ssym	variable, next_value_reg
	//
	//		// Block.
	//		<block>
	//
	//		jmp continue
	//
	// break:
	
	// Parse expression.
	u32 context_reg = _children[1]->GenerateInstructions(gen);
	gen->AllocateRegister(this, context_reg);

	// Generate context.	
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_ITERNEW, 
					  CreateRegisterOperand(context_reg),
					  CreateRegisterOperand(context_reg));	

	// continue:
	_continueJumpTarget->Bind(gen, this);

	// Is there another variable.
	u32 done_reg = gen->AllocateRegister(this);

	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_ITERDONE, 
					  CreateRegisterOperand(done_reg),
					  CreateRegisterOperand(context_reg));	

	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_CMP, 
					  CreateRegisterOperand(done_reg),
					  CreateRegisterOperand(SCRIPT_CONST_REGISTER_ZERO));	
	
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JNE, 
					  CreateJumpTargetOperand(_breakJumpTarget));

	// Get next value and assign to variable.
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_ITERNEXT, 
					  CreateRegisterOperand(done_reg),
					  CreateRegisterOperand(context_reg));	

	StoreRegister(gen, symbol, done_reg);
	//CreateInstruction(gen, Instructions::SCRIPT_OPCODE_SSYM, 
	//				  CreateSymbolOperand(symbol),
	//				  CreateRegisterOperand(done_reg));	

	gen->DeallocateRegister(this, done_reg);

	// block.
	_children[2]->GenerateInstructions(gen);

	// Back to the start of the loop.
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JMP, 
					  CreateJumpTargetOperand(_continueJumpTarget));	

	// break;
	_breakJumpTarget->Bind(gen, this);

	// Free context.
	gen->DeallocateRegister(this, context_reg);

	return 0;
}

AST::CScriptFunctionASTNode* CScriptForeachASTNode::FindParentFunction()
{
	AST::CScriptASTNode* node = this;
	while (node != NULL)
	{
		if (dynamic_cast<AST::CScriptFunctionASTNode*>(node) != NULL)
			return dynamic_cast<AST::CScriptFunctionASTNode*>(node);

		node = node->GetParent();
	}
	return NULL;
}