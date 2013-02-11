///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptIntrinsicASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptJumpTargetSymbol.h"
#include "CScriptManager.h"
#include "CScriptFunctionASTNode.h"
#include "CScriptVariableASTNode.h"
#include "CScriptFunctionSymbol.h"
#include "CScriptVariableSymbol.h"
#include "CScriptStringSymbol.h"
#include "CScriptCompileContext.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptIntrinsicASTNode::CScriptIntrinsicASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	PostFix = false;
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptIntrinsicASTNode::~CScriptIntrinsicASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptIntrinsicASTNode::GetName()
{
	return _token.Literal;
}

void CScriptIntrinsicASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptIntrinsicASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	u32 output_reg = _children[0]->GenerateInstructions(gen);
	gen->AllocateRegister(this, output_reg);
	
	switch (_token.ID)
	{		
		// Unary negatate/absolute.
		case TOKEN_KEYWORD_NATIVE:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_GETNATIVE, CreateRegisterOperand(output_reg)); break;
				
		default:
			{
				LOG_ASSERT(false);
			}
	}

	// Deallocate and return register!
	gen->DeallocateRegister(this, output_reg);
	return output_reg;
}
