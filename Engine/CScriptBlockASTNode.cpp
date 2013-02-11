///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptBlockASTNode.h"
#include "CScriptGenerator.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptBlockASTNode::CScriptBlockASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptBlockASTNode::~CScriptBlockASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptBlockASTNode::GetName()
{
	return "<block>";
}

void CScriptBlockASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptBlockASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// Generate instructions.

	// Generate children.
	CScriptASTNode::GenerateInstructions(gen);

	return 0;
}