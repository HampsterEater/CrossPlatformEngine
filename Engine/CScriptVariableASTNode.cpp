///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptVariableASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptVariableSymbol.h"
#include "CScriptManager.h"
#include "CScriptFunctionASTNode.h"
#include "CScriptCompileContext.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptVariableASTNode::CScriptVariableASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent, ScriptVariableType type)
{
	_type = type;
	_token = token;

	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptVariableASTNode::~CScriptVariableASTNode()
{
	_children.Clear();
}

ScriptVariableType CScriptVariableASTNode::GetType()
{
	return _type;
}

Engine::Containers::CString	CScriptVariableASTNode::GetName()
{
	return _token.Literal;
}

void CScriptVariableASTNode::GenerateSymbols(CScriptGenerator* gen)
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

	// Generate symbols.
	Symbols::CScriptSymbol* node = FindSymbol(_token.Literal, true);
	if (_type == SCRIPT_VARIABLE_PARAMETER)
	{
		// Only look in parameter list for parent function.
		node = _parent->FindSymbol(_token.Literal, false);
	}

	if (node != NULL)
	{
		gen->Error(this, S("Encountered identifier redefinition '%s'").Format(_token.Literal.c_str()));
		return;
	}
	else
	{
		Symbols::CScriptVariableSymbol* symbol = GetScriptAllocator()->NewObj<Symbols::CScriptVariableSymbol>(this);
		symbol->Index = index;
		symbol->IsGlobal = isGlobal;
		AddSymbolToParent(symbol);
	}

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptVariableASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	Symbols::CScriptSymbol* symbol = FindSymbol(_token.Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_VARIABLE);

	// Generate assignment.
	if (_children.Size() > 0)
	{
		u32 output_reg = _children[0]->GenerateInstructions(gen);
		
		// ssym sym, reg0
		StoreRegister(gen, symbol, output_reg);

		//CreateInstruction(gen, Instructions::SCRIPT_OPCODE_SSYM,
		//					CreateSymbolOperand(symbol),
		//					CreateRegisterOperand(output_reg));
	}

	return 0;
}

AST::CScriptFunctionASTNode* CScriptVariableASTNode::FindParentFunction()
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