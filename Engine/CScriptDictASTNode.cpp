///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptDictASTNode.h"
#include "CScriptGenerator.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptDictASTNode::CScriptDictASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptDictASTNode::~CScriptDictASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptDictASTNode::GetName()
{
	return "<dict>";
}

void CScriptDictASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptDictASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	u32 output_reg = gen->AllocateRegister(this);

	// Allocate the list.
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_DICTNEW, 
							CreateRegisterOperand(output_reg));

	// Add all the values to the list.
	for (u32 i = 0; i < _children.Size(); i += 2)
	{
		// Parse the key.
		u32 key_reg = _children[i]->GenerateInstructions(gen);
		gen->AllocateRegister(this, key_reg);

		// Parse the value.
		u32 value_reg = _children[i + 1]->GenerateInstructions(gen);

		// Push value into list.
		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_DICTADD, 
							   CreateRegisterOperand(output_reg),
							   CreateRegisterOperand(key_reg),
							   CreateRegisterOperand(value_reg));

		gen->DeallocateRegister(this, key_reg);
	}

	gen->DeallocateRegister(this, output_reg);
	return output_reg;
}