///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptListASTNode.h"
#include "CScriptGenerator.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptListASTNode::CScriptListASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptListASTNode::~CScriptListASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptListASTNode::GetName()
{
	return "<list>";
}

void CScriptListASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptListASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	u32 output_reg = gen->AllocateRegister(this);

	// Allocate the list.
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_LISTNEW, 
							CreateRegisterOperand(output_reg));

	// Add all the values to the list.
	for (u32 i = 0; i < _children.Size(); i++)
	{
		u32 value_reg = _children[i]->GenerateInstructions(gen);

		// Push value into list.
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_LISTADD, 
							   CreateRegisterOperand(output_reg),
							   CreateRegisterOperand(value_reg));
	}

	gen->DeallocateRegister(this, output_reg);
	return output_reg;
}