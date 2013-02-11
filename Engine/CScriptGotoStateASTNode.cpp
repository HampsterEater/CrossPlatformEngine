///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptGotoStateASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"
#include "CScriptStringSymbol.h"
#include "CScriptCompileContext.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptGotoStateASTNode::CScriptGotoStateASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptGotoStateASTNode::~CScriptGotoStateASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptGotoStateASTNode::GetName()
{
	return "<gotostate>";
}

void CScriptGotoStateASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	//Symbols::CScriptSymbol* node = gen->GetContext()->GetASTRoot()->FindSymbol(_token.Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_STRING, 0);
	//if (node == NULL)
	//{
	//	Symbols::CScriptStringSymbol* symbol = GetScriptAllocator()->NewObj<Symbols::CScriptStringSymbol>(this);
	//	gen->GetContext()->GetASTRoot()->AddSymbol(symbol);
	//}

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptGotoStateASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	Symbols::CScriptSymbol* node = gen->GetContext()->GetASTRoot()->FindSymbol(_token.Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_STATE, 0);			
	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_SETSTATE, CreateSymbolOperand(node));	

	return 0;
}