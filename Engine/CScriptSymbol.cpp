///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptSymbol.h"
#include "CScriptCompileContext.h"
#include "CScriptASTNode.h"
#include "CScriptLexer.h"

using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::AST;

CScriptSymbol::CScriptSymbol()
{
	_astNode = NULL;
}

CScriptSymbol::~CScriptSymbol()
{
}

CScriptASTNode* CScriptSymbol::GetASTNode()
{
	return _astNode;
}

Engine::Scripting::CScriptToken& CScriptSymbol::GetToken()
{
	return _token;
}

const Engine::Containers::CString CScriptSymbol::GetIdentifier()
{
	return _token.Literal;
}

