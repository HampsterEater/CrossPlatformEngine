///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptClassASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptCompileContext.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptClassASTNode::CScriptClassASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptClassASTNode::~CScriptClassASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptClassASTNode::GetName()
{
	return "<class>";
}

void CScriptClassASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	CScriptCompileContext* context = gen->GetContext();

	context->_isClass   = true; 
	context->_className = _token.Literal;
	context->_classBaseName = "";

	for (u32 i = 0; i < _children.Size(); i++)
		context->_classBaseName += (i > 0 ? "." : "") + _children[i]->GetToken().Literal;
}

u32 CScriptClassASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// Nothing to do here.
	return 0;
}