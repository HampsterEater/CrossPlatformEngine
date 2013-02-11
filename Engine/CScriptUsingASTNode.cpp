///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptUsingASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"
#include "CScriptVariableASTNode.h"
#include "CScriptVariableSymbol.h"
#include "CScriptCompileContext.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptUsingASTNode::CScriptUsingASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptUsingASTNode::~CScriptUsingASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptUsingASTNode::GetName()
{
	return "<using>";
}

void CScriptUsingASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	CScriptASTNode* identifierNode = _children[0];

	// Generate symbols.
	Symbols::CScriptSymbol* node = FindSymbol(identifierNode->GetToken().Literal, true);
	if (node != NULL)
	{
		gen->Error(this, S("Encountered identifier redefinition '%s'").Format(identifierNode->GetToken().Literal.c_str()));
		return;
	}
	else
	{
		Symbols::CScriptVariableSymbol* symbol = GetScriptAllocator()->NewObj<Symbols::CScriptVariableSymbol>(identifierNode);
		symbol->IsGlobal = true;
		
		symbol->Index = gen->GetContext()->_globalVariableCount;
		gen->GetContext()->_globalVariableCount++;

		AddSymbolToParent(symbol);
	}
}

u32 CScriptUsingASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	CScriptASTNode* identifierNode = _children[0];
	Symbols::CScriptSymbol* symbol = FindSymbol(identifierNode->GetToken().Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_VARIABLE);

	u32 output_reg = gen->AllocateRegister(this);

	// loadmodule output, symbol
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_LOADMODULE,
						CreateRegisterOperand(output_reg),
						CreateSymbolOperand(symbol));

	// ssym sym, reg0
	StoreRegister(gen, symbol, output_reg);
	
//	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_SSYM,
//						CreateSymbolOperand(symbol),
//						CreateRegisterOperand(output_reg));

	gen->DeallocateRegister(this, output_reg);

	return output_reg;
}