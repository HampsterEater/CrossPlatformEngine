///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptFunctionASTNode.h"
#include "CScriptFunctionSymbol.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"
#include "CScriptExpressionASTNode.h"
#include "CScriptCompileContext.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptFunctionASTNode::CScriptFunctionASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent, ScriptFunctionType type)
{
	_type = type;
	_token = token;
	Assigned = false;
	ParameterCount = 0;
	LocalCount = 0;

	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptFunctionASTNode::~CScriptFunctionASTNode()
{
	_children.Clear();
}

ScriptFunctionType CScriptFunctionASTNode::GetType()
{
	return _type;
}

Engine::Containers::CString	CScriptFunctionASTNode::GetName()
{
	return _token.Literal;
}

ScriptFunctionType CScriptFunctionASTNode::GetFunctionType()
{
	return _type;
}

void CScriptFunctionASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	CScriptASTNode* blockNode = _children[ParameterCount];
	Symbols::CScriptFunctionSymbol* symbol = NULL;
	Symbols::CScriptSymbol* node = FindSymbol(_token.Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_FUNCTION);
	if (node != NULL)
	{
		gen->Error(this, S("Encountered identifier redefinition '%s'").Format(_token.Literal.c_str()));
		return;
	}
	else
	{
		symbol = GetScriptAllocator()->NewObj<Symbols::CScriptFunctionSymbol>(this);
		symbol->ParameterCount = ParameterCount;
		symbol->State		   = GetStateSymbol();
		symbol->LocalCount	   = 0;
		symbol->Type		   = _type;

		symbol->Index = gen->GetContext()->_functionTableSize;
		gen->GetContext()->_functionTableSize++;

		AddSymbolToParent(symbol);
	}

	// Generate children.
	if (Assigned == false)
		CScriptASTNode::GenerateSymbols(gen);
	else
		blockNode->GenerateSymbols(gen);

	symbol->LocalCount = LocalCount;
}

u32 CScriptFunctionASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	CScriptASTNode* blockNode = _children[ParameterCount];
	Symbols::CScriptSymbol* symbol = FindSymbol(_token.Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_FUNCTION);

	// Assignment?
	if (Assigned == true)
	{
		// Make sure this assignment goes in the global scope.
		u32 output_reg = blockNode->GenerateInstructions(gen);
		
		// ssym sym, reg0
		StoreRegister(gen, symbol, output_reg);
	
		//CreateInstruction(gen, Instructions::SCRIPT_OPCODE_SSYM,
		//				  CreateSymbolOperand(symbol),
		//				  CreateRegisterOperand(output_reg));
	}
	
	// Or block?
	else 
	{		
		// Generate children.
		CScriptASTNode::GenerateInstructions(gen);

		// Return instruction.
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_RET);
	}

	return 0;
}