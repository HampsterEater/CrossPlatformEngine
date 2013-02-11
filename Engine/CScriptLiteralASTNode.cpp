///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptLiteralASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"
#include "CScriptStringSymbol.h"
#include "CScriptCompileContext.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptLiteralASTNode::CScriptLiteralASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptLiteralASTNode::~CScriptLiteralASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptLiteralASTNode::GetName()
{
	return _token.Literal;
}

void CScriptLiteralASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	if (_token.ID == TOKEN_LITERAL_STRING)
	{
		Symbols::CScriptSymbol* node = gen->GetContext()->GetASTRoot()->FindSymbol(_token.Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_STRING, 0);
		if (node == NULL)
		{
			Symbols::CScriptStringSymbol* symbol = GetScriptAllocator()->NewObj<Symbols::CScriptStringSymbol>(this);
			gen->GetContext()->GetASTRoot()->AddSymbol(symbol);
		}
	}

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptLiteralASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	u32 output = gen->AllocateRegister(this);

	switch (_token.ID)
	{
		case TOKEN_LITERAL_INT:	
			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_LDI,
							  CreateRegisterOperand(output),
							  CreateIntOperand(_token.Literal.ToInt()));
			break;

		case TOKEN_LITERAL_FLOAT:
			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_LDF,
							  CreateRegisterOperand(output),
							  CreateFloatOperand(_token.Literal.ToFloat()));
			break;

		case TOKEN_LITERAL_NULL:
			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_LDN,
							  CreateRegisterOperand(output));
			break;

		case TOKEN_LITERAL_STRING:
		{
			Symbols::CScriptSymbol* node = gen->GetContext()->GetASTRoot()->FindSymbol(_token.Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_STRING, 0);			
			LoadRegister(gen, node, output);
			break;
		}

		default:
		{
			LOG_ASSERT(false);
			break;
		}
	}

	gen->DeallocateRegister(this, output);
	return output;
}