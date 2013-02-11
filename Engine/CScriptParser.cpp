///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptParser.h"
#include "CScriptLexer.h"
#include "CScriptCompileContext.h"
#include "CScriptManager.h"

#include "CScriptASTNode.h"
#include "CScriptBlockASTNode.h"
#include "CScriptVariableASTNode.h"
#include "CScriptFunctionASTNode.h"
#include "CScriptOperatorASTNode.h"
#include "CScriptExpressionASTNode.h"
#include "CScriptListASTNode.h"
#include "CScriptDictASTNode.h"
#include "CScriptLiteralASTNode.h"
#include "CScriptIdentifierASTNode.h"
#include "CScriptForASTNode.h"
#include "CScriptForeachASTNode.h"
#include "CScriptWhileASTNode.h"
#include "CScriptDoASTNode.h"
#include "CScriptReturnASTNode.h"
#include "CScriptYieldASTNode.h"
#include "CScriptSwitchASTNode.h"
#include "CScriptSwitchCaseASTNode.h"
#include "CScriptSwitchDefaultASTNode.h"
#include "CScriptIfASTNode.h"
#include "CScriptBreakASTNode.h"
#include "CScriptContinueASTNode.h"
#include "CScriptClassASTNode.h"
#include "CScriptUsingASTNode.h"
#include "CScriptStateASTNode.h"
#include "CScriptGotoStateASTNode.h"
#include "CScriptIntrinsicASTNode.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptParser::CScriptParser()
{
}

CScriptParser::~CScriptParser()
{
	_scopeList.Clear();
}

bool CScriptParser::EndOfTokens(u32 offset)
{
	return _tokenIndex + offset >= _context->_tokenList.Size();
}

CScriptToken CScriptParser::NextToken()
{
	if (EndOfTokens())
		return CScriptToken(TOKEN_OP_EOF, "<eof>", _context->_lastLineIndex, _context->_lastLineColumn);

	CScriptToken c = _context->_tokenList[_tokenIndex];
	_tokenIndex++;

	return c;
}

CScriptToken CScriptParser::ExpectToken(TokenID id)
{
	CScriptToken t = NextToken();
	if (t.ID != id)
	{
		Error(S("Encountered unexpected token '%s' (0x%x).").Format(t.Literal, (u8)t.ID));
	}
	return t;
}

CScriptToken CScriptParser::CurrentToken()
{
	return _context->_tokenList[_tokenIndex - 1];
}

CScriptToken CScriptParser::PreviousToken()
{
	if (_tokenIndex < 2)
		return CScriptToken(TOKEN_OP_SOF, "<sof>", _context->_lastLineIndex, _context->_lastLineColumn);
	return _context->_tokenList[_tokenIndex - 2];
}

void CScriptParser::RewindStream(u32 offset)
{
	_tokenIndex -= offset;
}

CScriptToken CScriptParser::LookAheadToken(u32 offset)
{
	if (EndOfTokens(offset))
		return CScriptToken(TOKEN_OP_EOF, "<eof>", _context->_lastLineIndex, _context->_lastLineColumn);
	
	return _context->_tokenList[_tokenIndex + offset];
}

void CScriptParser::Error(const Engine::Containers::CString& str)
{
	_context->PushError(CScriptError(SCRIPT_ERROR_FATAL, 
									 str, 
									 _context->_initialFile, CurrentToken().Line, CurrentToken().Column));
	throw str; // Abort the fuck out of parsing.
}

void CScriptParser::Warning(const Engine::Containers::CString& str)
{
	_context->PushError(CScriptError(SCRIPT_ERROR_WARNING, 
									 str, 
									 _context->_initialFile, CurrentToken().Line, CurrentToken().Column));
}

void CScriptParser::Info(const Engine::Containers::CString& str)
{
	_context->PushError(CScriptError(SCRIPT_ERROR_INFO, 
									 str, 
									 _context->_initialFile, CurrentToken().Line, CurrentToken().Column));
}

void CScriptParser::PushScope(AST::CScriptASTNode* node)
{
	_scopeList.AddToEnd(_scope);
	_scope = node;
}

void CScriptParser::PopScope()
{
	_scope = _scopeList.RemoveFromEnd();
}

void CScriptParser::PushBreakLoop(AST::CScriptASTNode* node)
{
	_breakLoopList.AddToEnd(node);
}

void CScriptParser::PopBreakLoop()
{
	_breakLoopList.RemoveFromEnd();
}

void CScriptParser::PushContinueLoop(AST::CScriptASTNode* node)
{
	_continueLoopList.AddToEnd(node);
}

void CScriptParser::PopContinueLoop()
{
	_continueLoopList.RemoveFromEnd();
}

bool CScriptParser::IsInGenerator()
{
	CScriptASTNode* node = _scope;
	while (node != NULL)
	{
		CScriptFunctionASTNode* sym = dynamic_cast<CScriptFunctionASTNode*>(node);
		if (sym != NULL)
		{
			if (sym->GetFunctionType() == SCRIPT_FUNCTION_GENERATOR)
			{
				return true;
			}
		}
		node = node->GetParent();
	}

	return false;
}

bool CScriptParser::IsInFunction()
{
	CScriptASTNode* node = _scope;
	while (node != NULL)
	{
		CScriptFunctionASTNode* sym = dynamic_cast<CScriptFunctionASTNode*>(node);
		if (sym != NULL)
		{
			return true;
		}
		node = node->GetParent();
	}

	return false;
}

bool CScriptParser::IsInState()
{
	CScriptASTNode* node = _scope;
	while (node != NULL)
	{
		CScriptStateASTNode* sym = dynamic_cast<CScriptStateASTNode*>(node);
		if (sym != NULL)
		{
			return true;
		}
		node = node->GetParent();
	}

	return false;
}

bool CScriptParser::IsInGlobalScope()
{
	return !IsInState() && !IsInFunction();
}

bool CScriptParser::Analyze(CScriptCompileContext* context)
{
	_context = context;
	_tokenIndex = 0;

	// Create the root AST node with a token for the start of the file.
	_scope = GetScriptAllocator()->NewObj<CScriptASTNode>(PreviousToken(), (CScriptASTNode*)NULL);
	_globalScope = _scope;

	while (!EndOfTokens() && context->GetErrorCount(SCRIPT_ERROR_FATAL) <= 0)
	{
		try
		{
			ParseStatement();
		}
		catch (...)
		{
			// Catch any exceptions.
			break;
		}
	}

	// Store the AST tree for later.
	context->_astTree = _scope;

	return (context->GetErrorCount(SCRIPT_ERROR_FATAL) <= 0);
}

// Parses a top-level statement and returns an AST node representing
// it. Top-level statements are things like functions / expressions / etc.
void CScriptParser::ParseStatement()
{
	// End of tokens?
	if (EndOfTokens())
	{
		return;
	}

	// Empty statement? If so then we may as well
	// just skip right on over it.
	if (LookAheadToken().ID == TOKEN_OP_SEMICOLON)
	{
		NextToken();
		return;
	}

	// Something juicy?
	CScriptToken token = NextToken();
	switch (token.ID)
	{
		// Blocks
		case TOKEN_OP_OPEN_BRACE:
			ParseBlock();
			return;

		// Variables
		case TOKEN_KEYWORD_VAR:
		case TOKEN_KEYWORD_CONST:
			ParseVar();
			ExpectToken(TOKEN_OP_SEMICOLON);
			return;

		// Functions
		case TOKEN_KEYWORD_FUNCTION:
		case TOKEN_KEYWORD_GENERATOR:
		case TOKEN_KEYWORD_EVENT:
		case TOKEN_KEYWORD_OPERATOR:
			ParseFunction();
			return;
			
		case TOKEN_KEYWORD_RETURN:
			ParseReturn();
			return;
		case TOKEN_KEYWORD_YIELD:
			ParseYield();
			return;

		// Loops.
		case TOKEN_KEYWORD_FOR:
			ParseFor();
			return;
		case TOKEN_KEYWORD_WHILE:
			ParseWhile();
			return;
		case TOKEN_KEYWORD_DO:
			ParseDo();
			return;
		case TOKEN_KEYWORD_FOREACH:
			ParseForeach();
			return;

		// Branching.
		case TOKEN_KEYWORD_IF:
			ParseIf();
			return;
		case TOKEN_KEYWORD_SWITCH:
			ParseSwitch();
			return;
		case TOKEN_KEYWORD_BREAK:
			ParseBreak();
			return;
		case TOKEN_KEYWORD_CONTINUE:
			ParseContinue();
			return;

		// Class stuff.
		case TOKEN_KEYWORD_USING:
			ParseUsing();
			return;
		case TOKEN_KEYWORD_CLASS:
			ParseClass();
			return;
		
		// State stuff.
		case TOKEN_KEYWORD_STATE:
			ParseState();
			return;
		case TOKEN_KEYWORD_GOTOSTATE:
			ParseGotoState();
			return;

		// Dafuck did we just find?
		default:
		{
			RewindStream(1);
			ParseExpression();
			ExpectToken(TOKEN_OP_SEMICOLON);
		}
	}
}

// Parses a block of statements surrounded in { }'s
void CScriptParser::ParseBlock()
{
	// Create a new node.
	CScriptBlockASTNode* node = GetScriptAllocator()->NewObj<CScriptBlockASTNode>(CurrentToken(), _scope); 
	PushScope(node);

	// Empty block or full?
	if (LookAheadToken().ID != TOKEN_OP_CLOSE_BRACE)
	{

		// Keep parsing till we get to the end.
		while (LookAheadToken().ID != TOKEN_OP_CLOSE_BRACE)
		{
			// End of token!
			if (EndOfTokens())
			{
				Error("Encountered unexpected end of file, expecting end of block.");
				return;
			}

			// Parse sub statement.
			ParseStatement();
		}

	}

	// Gobble up the close brace.
	NextToken();
	PopScope();
}

// Parses a variable block.
//	var a;
//	var a, b;
//	var a = 324234, b = 2342;
void CScriptParser::ParseVar()
{
	// What type of variable is this?
	AST::ScriptVariableType type = AST::SCRIPT_VARIABLE_NORMAL;
	if (CurrentToken().ID == TOKEN_KEYWORD_CONST)
	{
		type = AST::SCRIPT_VARIABLE_CONSTANT;
	}

	// Can only be defined in functions or the global scope.
	if (!IsInGlobalScope() && !IsInFunction())
	{		
		Error("Unexpected variable declaration, variables can only be declared within the global scope or a functions scope.");
		return;
	}

	// Read in all the variables, deliminated by commas.
	while (true)
	{
		CScriptToken		    identifierToken	   = ExpectToken(TOKEN_LITERAL_IDENTIFIER);
		CScriptVariableASTNode* variable		   = GetScriptAllocator()->NewObj<CScriptVariableASTNode>(identifierToken, _scope, type);

		// Read in an assignment?
		if (LookAheadToken().ID == TOKEN_OP_ASSIGN)
		{
			ExpectToken(TOKEN_OP_ASSIGN);
		
			PushScope(variable);
			ParseExpression();
			PopScope();
		}

		// Finished declaring?
		if (LookAheadToken().ID == TOKEN_OP_COMMA)
			NextToken();
		else
			break;
	}
}

// Parse a function declaration.
//
//	function FuncA(a, b, c = 123)
//	{
//	}
//  function FuncB(a, b, c) = native(x);
//	generator GeneratorA(a, b, c)
//	{
//		yield a;
//		yield b;
//		yield c;
//		return;
//	}
//  function FuncB(a, b, c) export("ABC")
//	{
//
//	}
//  event FuncB(z, y, x)
//	{
//
//	}
//
void CScriptParser::ParseFunction()
{
	// What type of function is this?
	AST::ScriptFunctionType type = AST::SCRIPT_FUNCTION_NORMAL;
	if (CurrentToken().ID == TOKEN_KEYWORD_GENERATOR)
	{
		type = AST::SCRIPT_FUNCTION_GENERATOR;
	}
	else if (CurrentToken().ID == TOKEN_KEYWORD_EVENT)
	{
		type = AST::SCRIPT_FUNCTION_EVENT;
	}
	else if (CurrentToken().ID == TOKEN_KEYWORD_OPERATOR)
	{
		type = AST::SCRIPT_FUNCTION_OPERATOR;
	}

	// Can only be defined in functions or the global scope.
	if ((!IsInGlobalScope() && !IsInState()))
	{		
		Error("Unexpected function declaration, functions can only be declared within the global scope or a states scope.");
		return;
	}
	if (IsInFunction())
	{
		Error("Unexpected function declaration, functions can not be nested.");
		return;
	}
	if (IsInState() == true && type != AST::SCRIPT_FUNCTION_EVENT)
	{
		Error("Unexpected function declaration, only events can be defined inside states.");
		return;
	}

	// Read the identifier.
	CScriptToken identifierToken;
	if (type == AST::SCRIPT_FUNCTION_OPERATOR)
	{
		identifierToken = NextToken();

		if (identifierToken.ID != TOKEN_OP_ASSIGN_BITWISE_SHR &&
			identifierToken.ID != TOKEN_OP_ASSIGN_BITWISE_SHL &&
			identifierToken.ID != TOKEN_OP_ASSIGN_BITWISE_NOT &&
			identifierToken.ID != TOKEN_OP_ASSIGN_BITWISE_XOR &&
			identifierToken.ID != TOKEN_OP_ASSIGN_BITWISE_OR &&
			identifierToken.ID != TOKEN_OP_ASSIGN_BITWISE_AND &&
			identifierToken.ID != TOKEN_OP_ASSIGN_MOD &&
			identifierToken.ID != TOKEN_OP_ASSIGN_DIV &&
			identifierToken.ID != TOKEN_OP_ASSIGN_MUL &&
			identifierToken.ID != TOKEN_OP_ASSIGN_SUB &&
			identifierToken.ID != TOKEN_OP_ASSIGN_ADD &&
			identifierToken.ID != TOKEN_OP_ASSIGN &&	
			identifierToken.ID != TOKEN_OP_INCREMENT &&			
			identifierToken.ID != TOKEN_OP_DECREMENT &&
			identifierToken.ID != TOKEN_OP_BITWISE_OR &&
			identifierToken.ID != TOKEN_OP_BITWISE_AND &&
			identifierToken.ID != TOKEN_OP_BITWISE_XOR &&
			identifierToken.ID != TOKEN_OP_BITWISE_NOT &&
			identifierToken.ID != TOKEN_OP_BITWISE_SHL &&
			identifierToken.ID != TOKEN_OP_BITWISE_SHR &&
			identifierToken.ID != TOKEN_OP_MUL &&
			identifierToken.ID != TOKEN_OP_DIV &&
			identifierToken.ID != TOKEN_OP_ADD &&
			identifierToken.ID != TOKEN_OP_SUB &&
			identifierToken.ID != TOKEN_OP_MOD &&		
			identifierToken.ID != TOKEN_OP_LESS &&
			identifierToken.ID != TOKEN_OP_GREATER &&
			identifierToken.ID != TOKEN_OP_LESS_EQUAL &&
			identifierToken.ID != TOKEN_OP_GREATER_EQUAL &&
			identifierToken.ID != TOKEN_OP_NOT_EQUAL &&
			identifierToken.ID != TOKEN_OP_EQUAL)
		{
			Error(S("Unexpected token '%s' (0x%x), expecting operator to override.").Format(identifierToken.Literal.c_str(), identifierToken.ID));
		}
	}
	else
	{
		identifierToken = ExpectToken(TOKEN_LITERAL_IDENTIFIER);
	}
	
	// Create the node.
	CScriptFunctionASTNode* node = GetScriptAllocator()->NewObj<CScriptFunctionASTNode>(identifierToken, _scope, type);

	// Read in the parameters.
	ExpectToken(TOKEN_OP_OPEN_PARENT);
	while (LookAheadToken().ID != TOKEN_OP_CLOSE_PARENT)
	{
		// Read in the identifier.
		CScriptToken paramIdentifierToken = ExpectToken(TOKEN_LITERAL_IDENTIFIER);

		// Read in the parameter.
		CScriptVariableASTNode* param = GetScriptAllocator()->NewObj<CScriptVariableASTNode>(paramIdentifierToken, node, SCRIPT_VARIABLE_PARAMETER);
		node->ParameterCount++;

		// Another parameter.
		if (LookAheadToken().ID == TOKEN_OP_CLOSE_PARENT)
			break;
		else
			ExpectToken(TOKEN_OP_COMMA);
	}
	ExpectToken(TOKEN_OP_CLOSE_PARENT);

	// Are we being set to something?
	if (LookAheadToken().ID == TOKEN_OP_ASSIGN)
	{
		node->Assigned = true;

		ExpectToken(TOKEN_OP_ASSIGN);
		
		PushScope(node);
		// Keep this expression in the global scope.
		ParseExpression();
		PopScope();

		ExpectToken(TOKEN_OP_SEMICOLON);
	}

	// Or do we have a block of code?
	else
	{
		PushScope(node);
		ParseStatement();
		PopScope();
	}
}

// Parses a for block.
//
//	for (var a, b, c = 0; x < 100; i++)
//	{
//	}
//
//	for (c = 0; x < 100; i++)
//	{
//	}
//
void CScriptParser::ParseFor()
{
	// Can only be defined in functions.
	if (!IsInGlobalScope() && !IsInFunction())
	{		
		Error("Unexpected for declaration, for loops can only be declared within the global scope or a functions scope.");
		return;
	}

	CScriptForASTNode* expr = GetScriptAllocator()->NewObj<CScriptForASTNode>(CurrentToken(), _scope);
	PushScope(expr);
		
	// Read in opening parenthesis.
	ExpectToken(TOKEN_OP_OPEN_PARENT);

	// Read in initialization block.
	if (LookAheadToken().ID == TOKEN_KEYWORD_VAR)
	{
		NextToken();
		ParseVar();
		ExpectToken(TOKEN_OP_SEMICOLON);
	}
	else
	{
		ParseExpression();
		ExpectToken(TOKEN_OP_SEMICOLON);
	}

	// Read in the comparison.
	ParseExpression();
	ExpectToken(TOKEN_OP_SEMICOLON);

	// Read in the increment.
	ParseExpression();

	// Read in the end of the for block.
	ExpectToken(TOKEN_OP_CLOSE_PARENT);

	// Read in the block.
	PushBreakLoop(expr);
	PushContinueLoop(expr);
	ParseStatement();
	PopContinueLoop();
	PopBreakLoop();

	// Pop the scope!
	PopScope();
}

// Parses a while block.
//
//	while (expr)
//	{
//	}
//
void CScriptParser::ParseWhile()
{
	// Can only be defined in functions.
	if (!IsInGlobalScope() && !IsInFunction())
	{		
		Error("Unexpected while declaration, while loops can only be declared within the global scope or a functions scope.");
		return;
	}

	CScriptWhileASTNode* expr = GetScriptAllocator()->NewObj<CScriptWhileASTNode>(CurrentToken(), _scope);
	PushScope(expr);
		
	// Read in opening parenthesis.
	ExpectToken(TOKEN_OP_OPEN_PARENT);

	// Read in the comparison.
	ParseExpression();

	// Read in the end of the for block.
	ExpectToken(TOKEN_OP_CLOSE_PARENT);

	// Read in the block.
	PushBreakLoop(expr);
	PushContinueLoop(expr);
	ParseStatement();
	PopContinueLoop();
	PopBreakLoop();

	// Pop the scope!
	PopScope();
}

// Parses a do block.
//
//	do
//	{
//	}
//	while (x)
//
//	do
//	{
//	}
//	forever
//
void CScriptParser::ParseDo()
{
	// Can only be defined in functions.
	if (!IsInGlobalScope() && !IsInFunction())
	{		
		Error("Unexpected do declaration, do loops can only be declared within the global scope or a functions scope.");
		return;
	}

	CScriptDoASTNode* expr = GetScriptAllocator()->NewObj<CScriptDoASTNode>(CurrentToken(), _scope);
	PushScope(expr);
		
	// Read in the block.
	PushBreakLoop(expr);
	PushContinueLoop(expr);
	ParseStatement();
	PopContinueLoop();
	PopBreakLoop();

	// Are we a while or forever do loop?
	if (LookAheadToken().ID == TOKEN_KEYWORD_WHILE)
	{		
		// Read while keyword.
		ExpectToken(TOKEN_KEYWORD_WHILE);

		// Read in opening parenthesis.
		ExpectToken(TOKEN_OP_OPEN_PARENT);

		// Read in the comparison.
		ParseExpression();

		// Read in the end of the for block.
		ExpectToken(TOKEN_OP_CLOSE_PARENT);
	}
	else
	{
		ExpectToken(TOKEN_KEYWORD_FOREVER);
	}

	// Pop the scope!
	PopScope();
}

// Parses a foreach block.
//
//	foreach (var x in y)
//	{
//	}
//
//	foreach (x in y)
//	{
//	}
//
void CScriptParser::ParseForeach()
{
	// Can only be defined in functions.
	if (!IsInGlobalScope() && !IsInFunction())
	{		
		Error("Unexpected foreach declaration, foreach loops can only be declared within the global scope or a functions scope.");
		return;
	}

	CScriptForeachASTNode* expr = GetScriptAllocator()->NewObj<CScriptForeachASTNode>(CurrentToken(), _scope);
	expr->NewVariable = false;
	PushScope(expr);
		
	// Read in opening parenthesis.
	ExpectToken(TOKEN_OP_OPEN_PARENT);

	// Read in initialization block.
	if (LookAheadToken().ID == TOKEN_KEYWORD_VAR)
	{
		ExpectToken(TOKEN_KEYWORD_VAR);
		expr->NewVariable = true;
	}
	CScriptToken tok = ExpectToken(TOKEN_LITERAL_IDENTIFIER);
	GetScriptAllocator()->NewObj<CScriptIdentifierASTNode>(tok, expr);

	// Read the in keyword.
	ExpectToken(TOKEN_KEYWORD_IN);

	// Read in the expression.
	ParseExpression();

	// Read in the end of the for block.
	ExpectToken(TOKEN_OP_CLOSE_PARENT);

	// Read in the block.
	PushBreakLoop(expr);
	PushContinueLoop(expr);
	ParseStatement();
	PopContinueLoop();
	PopBreakLoop();

	// Pop the scope!
	PopScope();
}

// Parses a return expression.
//
//	return <expr>;
void CScriptParser::ParseReturn()
{
	// Can only be defined in functions.
	if (!IsInFunction())
	{		
		Error("Unexpected return statement, return statements are only valid in a functions scope.");
		return;
	}

	CScriptReturnASTNode* expr = GetScriptAllocator()->NewObj<CScriptReturnASTNode>(CurrentToken(), _scope);

	if (LookAheadToken().ID != TOKEN_OP_SEMICOLON)
	{
		// Can only be defined outside generators.
		if (IsInGenerator())
		{		
			Error("Unexpected return value, values can only be returned from generators using the yield keyword.");
			return;
		}

		PushScope(expr);
		ParseExpression();
		PopScope();
	}

	ExpectToken(TOKEN_OP_SEMICOLON);
}

// Parses a yield expression.
//
//	yield <expr>;
void CScriptParser::ParseYield()
{
	// Can only be defined in generators.
	if (!IsInGenerator())
	{		
		Error("Unexpected yield statement, yield statements are only valid in a generators scope.");
		return;
	}

	CScriptYieldASTNode* expr = GetScriptAllocator()->NewObj<CScriptYieldASTNode>(CurrentToken(), _scope);
	
	PushScope(expr);
	ParseExpression();
	PopScope();

	ExpectToken(TOKEN_OP_SEMICOLON);
}

// Parses a script expression.
//
//	switch (x + y)
//	{
//		case 0:
//		{
//			return x;
//		}
//		case 1, 2, 3:
//		{
//			break;
//		}
//		default:
//		{
//		}
//	}
//
void CScriptParser::ParseSwitch()
{
	// Can only be defined in functions.
	if (!IsInGlobalScope() && !IsInFunction())
	{		
		Error("Unexpected switch declaration, switch loops can only be declared within the global scope or a functions scope.");
		return;
	}

	CScriptSwitchASTNode* expr = GetScriptAllocator()->NewObj<CScriptSwitchASTNode>(CurrentToken(), _scope);
	PushScope(expr);
		
	// Read in opening parenthesis.
	ExpectToken(TOKEN_OP_OPEN_PARENT);

	// Read in the comparison.
	ParseExpression();

	// Read in the end of the for block.
	ExpectToken(TOKEN_OP_CLOSE_PARENT);

	// Read in the opening brace.
	ExpectToken(TOKEN_OP_OPEN_BRACE);
	
	PushBreakLoop(expr);

	// Case/Default blocks!
	bool foundDefault = false;
	while (LookAheadToken().ID != TOKEN_OP_CLOSE_BRACE)
	{
		CScriptToken lat = LookAheadToken();
		if (lat.ID == TOKEN_KEYWORD_CASE)
		{
			if (foundDefault == true)
			{
				Error("The default block in a switch statement must must the last block.");
			}

			CScriptSwitchCaseASTNode* node = GetScriptAllocator()->NewObj<CScriptSwitchCaseASTNode>(CurrentToken(), expr);
			PushScope(node);

			NextToken();
			
			while (LookAheadToken().ID != TOKEN_OP_COLON)
			{
				ParseExpression();
				if (LookAheadToken().ID != TOKEN_OP_COLON)
					ExpectToken(TOKEN_OP_COMMA);
			}

			ExpectToken(TOKEN_OP_COLON);
			ParseStatement();

			PopScope();
		}
		else if (lat.ID == TOKEN_KEYWORD_DEFAULT)
		{			
			if (foundDefault == true)
			{	
				Error("Switch statements can only contain one default block.");
			}

			CScriptSwitchDefaultASTNode* node = GetScriptAllocator()->NewObj<CScriptSwitchDefaultASTNode>(CurrentToken(), expr);
			PushScope(node);

			NextToken();
			ExpectToken(TOKEN_OP_COLON);
			ParseStatement();

			PopScope();

			foundDefault = true;
		}
		else
		{
			Error(S("Encountered unexpected token '%s' (0x%x), switch statements can only contain case and default blocks.").Format(lat.Line, lat.ID));
		}
	}
	
	PopBreakLoop();

	// Close brace.
	ExpectToken(TOKEN_OP_CLOSE_BRACE);

	// Pop the scope!
	PopScope();
}

// Parses an if expression.
//
//	if (x + y)
//	{
//	}
//
//	if (x + y)
//	{
//	}
//	else
//	{
//	}
//
//	if (x + y)
//	{
//	}
//	else if (x + y)
//	{
//	}
//
void CScriptParser::ParseIf()
{
	// Can only be defined in functions.
	if (!IsInGlobalScope() && !IsInFunction())
	{		
		Error("Unexpected if declaration, if blocks can only be declared within the global scope or a functions scope.");
		return;
	}

	CScriptIfASTNode* expr = GetScriptAllocator()->NewObj<CScriptIfASTNode>(CurrentToken(), _scope);
	PushScope(expr);
		
	// Read in opening parenthesis.
	ExpectToken(TOKEN_OP_OPEN_PARENT);

	// Read in the comparison.
	ParseExpression();

	// Read in the end of the for block.
	ExpectToken(TOKEN_OP_CLOSE_PARENT);

	// Read the statement.
	ParseStatement();

	// Else block.
	if (LookAheadToken().ID == TOKEN_KEYWORD_ELSE)
	{
		NextToken();
		ParseStatement();
	}

	// Pop the scope!
	PopScope();
}

// Parses a continue expression.
//
//	continue <expr>;
void CScriptParser::ParseContinue()
{
	CScriptContinueASTNode* expr = GetScriptAllocator()->NewObj<CScriptContinueASTNode>(CurrentToken(), _scope);

	// Read in optional level.
	u32 level = 1;
	if (LookAheadToken().ID != TOKEN_OP_SEMICOLON)
	{
		level = ExpectToken(TOKEN_LITERAL_INT).Literal.ToInt();
	}

	if (level <= 0 || level > _continueLoopList.Size())
	{
		if (level == 1)
			Error("Continue statements are required to be contained inside loops.");
		else
			Error(S("Continue level (%i, only %i levels available) not available.").Format(level, _continueLoopList.Size()));			
		return;
	}

	// Update loop label.
	expr->Loop = _continueLoopList[_continueLoopList.Size() - level];

	ExpectToken(TOKEN_OP_SEMICOLON);
}

// Parses a break expression.
//
//	break <expr>;
void CScriptParser::ParseBreak()
{
	CScriptBreakASTNode* expr = GetScriptAllocator()->NewObj<CScriptBreakASTNode>(CurrentToken(), _scope);

	// Read in optional level.
	u32 level = 1;
	if (LookAheadToken().ID != TOKEN_OP_SEMICOLON)
	{
		level = ExpectToken(TOKEN_LITERAL_INT).Literal.ToInt();
	}

	if (level <= 0 || level > _breakLoopList.Size())
	{
		if (level == 1)
			Error("Break statements are required to be contained inside loop or switch statements.");
		else
			Error(S("Break level (%i, only %i levels available) not available.").Format(level, _breakLoopList.Size()));			
		return;
	}

	// Update loop label.
	expr->Loop = _breakLoopList[_breakLoopList.Size() - level];

	ExpectToken(TOKEN_OP_SEMICOLON);
}

// Parsed a definition of another package.
//
//	using X.Y.Z;
//	using X.Y.Z.*;
//	
//		NOPE, changed my mind, now namespacing.
//
//	using X;
void CScriptParser::ParseUsing()
{
	// Can only be defined in functions.
	if (!IsInGlobalScope())
	{		
		Error("Unexpected using declaration, using can only be used within the global scope.");
		return;
	}

	CScriptUsingASTNode* expr = GetScriptAllocator()->NewObj<CScriptUsingASTNode>(CurrentToken(), _scope);
	u32 offset = 0;
	
	CScriptToken tok = ExpectToken(TOKEN_LITERAL_IDENTIFIER);
	GetScriptAllocator()->NewObj<CScriptIdentifierASTNode>(tok, expr);

	/*
	while (true)
	{
		if (LookAheadToken().ID == TOKEN_OP_MUL)
		{
			CScriptToken tok = ExpectToken(TOKEN_OP_MUL);
			
			if (offset == 0)
			{
				Error(S("Encountered unexpected token '%s' (0x%i), expected class identifier.").Format(tok.Literal.c_str(), tok.ID));
				return;
			}

			GetScriptAllocator()->NewObj<CScriptIdentifierASTNode>(tok, expr);
		}
		else
		{
			CScriptToken tok = ExpectToken(TOKEN_LITERAL_IDENTIFIER);
			GetScriptAllocator()->NewObj<CScriptIdentifierASTNode>(tok, expr);
		}

		if (LookAheadToken().ID != TOKEN_OP_SEMICOLON)
		{
			ExpectToken(TOKEN_OP_PERIOD);
		}
		else
		{
			break;
		}

		offset++;
	}*/

	ExpectToken(TOKEN_OP_SEMICOLON);
}

// Parsed a definition of a class.
//
//	class X extends Y;
void CScriptParser::ParseClass()
{
	// Can only be defined in functions.
	if (!IsInGlobalScope())
	{		
		Error("Unexpected class declaration, class declarations can only be used within the global scope.");
		return;
	}

	CScriptToken tok = ExpectToken(TOKEN_LITERAL_IDENTIFIER);
	CScriptClassASTNode* expr = GetScriptAllocator()->NewObj<CScriptClassASTNode>(tok, _scope);
	
	if (LookAheadToken().ID == TOKEN_KEYWORD_EXTENDS)
	{
		NextToken();
		
		CScriptToken tok = ExpectToken(TOKEN_LITERAL_IDENTIFIER);
		GetScriptAllocator()->NewObj<CScriptIdentifierASTNode>(tok, expr);

		/*while (true)
		{
			CScriptToken tok = ExpectToken(TOKEN_LITERAL_IDENTIFIER);
			GetScriptAllocator()->NewObj<CScriptIdentifierASTNode>(tok, expr);

			if (LookAheadToken().ID != TOKEN_OP_SEMICOLON)
			{
				ExpectToken(TOKEN_OP_PERIOD);
			}
			else
			{
				break;
			}
		}*/
	}

	ExpectToken(TOKEN_OP_SEMICOLON);
}

// Parsed a state definition.
//
//	state X
//	{
//	}
//	
//	state X As Default 
//	{
//	}
//
void CScriptParser::ParseState()
{
	// Can only be defined in functions.
	if (!IsInGlobalScope())
	{		
		Error("Unexpected state declaration, states can onnly be used within the global scope.");
		return;
	}

	CScriptToken tok = ExpectToken(TOKEN_LITERAL_IDENTIFIER);
	CScriptStateASTNode* expr = GetScriptAllocator()->NewObj<CScriptStateASTNode>(tok, _scope);

	// Is default?
	if (LookAheadToken().ID == TOKEN_KEYWORD_AS &&
		LookAheadToken(1).ID == TOKEN_KEYWORD_DEFAULT)
	{
		NextToken();
		NextToken();
		expr->IsDefault = true;
	}

	PushScope(expr);
	ExpectToken(TOKEN_OP_OPEN_BRACE);
	ParseBlock();
	PopScope();
}

// Parsed a gotostate definition.
//
//	gotostate X;
//
void CScriptParser::ParseGotoState()
{
	// Can only be defined in functions.
//	if (!IsInFunction())
//	{		
//		Error("Unexpected gotostate declaration, gotostate can only be used within a functions scope.");
//		return;
//	}

	bool hasParent = false;
	if (LookAheadToken().ID == TOKEN_OP_OPEN_PARENT)
	{
		NextToken();
		hasParent = true;
	}

	CScriptToken tok = ExpectToken(TOKEN_LITERAL_IDENTIFIER);
	CScriptGotoStateASTNode* expr = GetScriptAllocator()->NewObj<CScriptGotoStateASTNode>(tok, _scope);

	if (hasParent == true)
		ExpectToken(TOKEN_OP_CLOSE_PARENT);

	ExpectToken(TOKEN_OP_SEMICOLON);
}

// Parses a top level expression. This is essentially 
// an alias for calling ParseExprLevel0()
void CScriptParser::ParseExpression()
{
	AST::CScriptASTNode* node = ParseExprLevel0();
	if (node != NULL)
	{
		CScriptExpressionASTNode* expr = GetScriptAllocator()->NewObj<CScriptExpressionASTNode>(CurrentToken(), _scope);
		expr->AddChild(node);
		//expr->GetChildren().AddToEnd(node);
	}
	else
	{
		Error("Expecting an expression, but encountered none.");
	}
}

// Parses precendence level 1, assignment operators.
// x as y
AST::CScriptASTNode* CScriptParser::ParseExprLevel0()
{
	// Parse l-value
	AST::CScriptASTNode* lvalue = ParseExprLevel1();

	// Do we have an operator next?
	CScriptToken opToken = LookAheadToken();
	if (opToken.ID != TOKEN_KEYWORD_IS &&
		opToken.ID != TOKEN_KEYWORD_AS)
		return lvalue;
	NextToken();

	// Create operator.
	CScriptOperatorASTNode* op = GetScriptAllocator()->NewObj<CScriptOperatorASTNode>(opToken, (AST::CScriptASTNode*)NULL);

	// Parse r-value.
	CScriptToken tok = LookAheadToken();
	if (tok.ID != TOKEN_KEYWORD_STRING && 
		tok.ID != TOKEN_KEYWORD_FLOAT && 
		tok.ID != TOKEN_KEYWORD_INT && 
		tok.ID != TOKEN_KEYWORD_LIST && 
		tok.ID != TOKEN_KEYWORD_DICT && 
		tok.ID != TOKEN_LITERAL_IDENTIFIER)
		ExpectToken(TOKEN_LITERAL_IDENTIFIER);
	else
		NextToken();
	
	AST::CScriptASTNode* rvalue = GetScriptAllocator()->NewObj<CScriptIdentifierASTNode>(tok, (AST::CScriptASTNode*)NULL);

	// Add children.
	op->AddChild(lvalue);
	op->AddChild(rvalue);

	return op;
}

// Parses precendence level 1, assignment operators.
// += / -= / *= / etc 
AST::CScriptASTNode* CScriptParser::ParseExprLevel1()
{
	// Parse l-value
	AST::CScriptASTNode* lvalue = ParseExprLevel2();

	// Do we have an operator next?
	CScriptToken opToken = LookAheadToken();
	if (opToken.ID != TOKEN_OP_ASSIGN_BITWISE_SHR &&
		opToken.ID != TOKEN_OP_ASSIGN_BITWISE_SHL &&
		opToken.ID != TOKEN_OP_ASSIGN_BITWISE_NOT &&
		opToken.ID != TOKEN_OP_ASSIGN_BITWISE_XOR &&
		opToken.ID != TOKEN_OP_ASSIGN_BITWISE_OR &&
		opToken.ID != TOKEN_OP_ASSIGN_BITWISE_AND &&
		opToken.ID != TOKEN_OP_ASSIGN_MOD &&
		opToken.ID != TOKEN_OP_ASSIGN_DIV &&
		opToken.ID != TOKEN_OP_ASSIGN_MUL &&
		opToken.ID != TOKEN_OP_ASSIGN_SUB &&
		opToken.ID != TOKEN_OP_ASSIGN_ADD &&
		opToken.ID != TOKEN_OP_ASSIGN)
	{
		return lvalue;
	}
	NextToken();

	// Create operator.
	CScriptOperatorASTNode* op = GetScriptAllocator()->NewObj<CScriptOperatorASTNode>(opToken, (AST::CScriptASTNode*)NULL);

	// Parse r-value.
	AST::CScriptASTNode* rvalue = ParseExprLevel2();

	op->AddChild(lvalue);
	op->AddChild(rvalue);
	//op->GetChildren().AddToEnd(lvalue);
	//op->GetChildren().AddToEnd(rvalue);

	return op;
}

// Parses precendence level 2, ternary operators.
// (expr) ? (expr) : (expr)
AST::CScriptASTNode* CScriptParser::ParseExprLevel2()
{
	// Parse l-value
	AST::CScriptASTNode* lvalue = ParseExprLevel3();

	// Do we have an operator next?
	CScriptToken opToken = LookAheadToken();
	if (opToken.ID != TOKEN_OP_QUESTION)
	{
		return lvalue;
	}
	NextToken();

	// Create operator.
	CScriptOperatorASTNode* op = GetScriptAllocator()->NewObj<CScriptOperatorASTNode>(opToken, (AST::CScriptASTNode*)NULL);

	// Parse r-value.
	AST::CScriptASTNode* rvalue = ParseExprLevel3();

	// Read in the colon.
	ExpectToken(TOKEN_OP_COLON);

	// Parse second r-value.
	AST::CScriptASTNode* rrvalue = ParseExprLevel3();

	op->AddChild(lvalue);
	op->AddChild(rvalue);
	op->AddChild(rrvalue);
			
	//op->GetChildren().AddToEnd(lvalue);
	//op->GetChildren().AddToEnd(rvalue);
	//op->GetChildren().AddToEnd(rrvalue);

	return op;
}

// Parses precendence level 3, logical operators.
// && / || 
AST::CScriptASTNode* CScriptParser::ParseExprLevel3()
{
	// Parse l-value
	AST::CScriptASTNode*	lvalue = ParseExprLevel4();

	while (true)
	{
		// Do we have an operator next?
		CScriptToken opToken = LookAheadToken();
		if (opToken.ID != TOKEN_OP_LOGICAL_AND &&
			opToken.ID != TOKEN_OP_LOGICAL_OR)
		{
			return lvalue;
		}
		NextToken();

		// Create operator.
		CScriptOperatorASTNode* op = GetScriptAllocator()->NewObj<CScriptOperatorASTNode>(opToken, (AST::CScriptASTNode*)NULL);

		// Parse r-value.
		AST::CScriptASTNode* rvalue = ParseExprLevel4();

		op->AddChild(lvalue);
		op->AddChild(rvalue);
		//op->GetChildren().AddToEnd(lvalue);
		//op->GetChildren().AddToEnd(rvalue);

		lvalue = op;
	}

	return lvalue;
}

// Parses precendence level 4, bitwise operators.
// & ^ |
AST::CScriptASTNode* CScriptParser::ParseExprLevel4()
{
	// Parse l-value
	AST::CScriptASTNode*	lvalue = ParseExprLevel5();

	while (true)
	{
		// Do we have an operator next?
		CScriptToken opToken = LookAheadToken();
		if (opToken.ID != TOKEN_OP_BITWISE_AND &&
			opToken.ID != TOKEN_OP_BITWISE_OR &&
			opToken.ID != TOKEN_OP_BITWISE_XOR &&
			opToken.ID != TOKEN_OP_BITWISE_SHL &&
			opToken.ID != TOKEN_OP_BITWISE_SHR)
		{
			return lvalue;
		}
		NextToken();

		// Create operator.
		CScriptOperatorASTNode* op = GetScriptAllocator()->NewObj<CScriptOperatorASTNode>(opToken, (AST::CScriptASTNode*)NULL);

		// Parse r-value.
		AST::CScriptASTNode* rvalue = ParseExprLevel5();

		op->AddChild(lvalue);
		op->AddChild(rvalue);
		//op->GetChildren().AddToEnd(lvalue);
		//op->GetChildren().AddToEnd(rvalue);

		lvalue = op;
	}

	return lvalue;
}

// Parses precendence level 5, conditional operators.
// == / != / <= / >= / > / <
AST::CScriptASTNode* CScriptParser::ParseExprLevel5()
{
	// Parse l-value
	AST::CScriptASTNode*	lvalue = ParseExprLevel6();

	while (true)
	{
		// Do we have an operator next?
		CScriptToken opToken = LookAheadToken();
		if (opToken.ID != TOKEN_OP_LESS &&
			opToken.ID != TOKEN_OP_GREATER &&
			opToken.ID != TOKEN_OP_LESS_EQUAL &&
			opToken.ID != TOKEN_OP_GREATER_EQUAL &&
			opToken.ID != TOKEN_OP_NOT_EQUAL &&
			opToken.ID != TOKEN_OP_EQUAL)
		{
			return lvalue;
		}
		NextToken();

		// Create operator.
		CScriptOperatorASTNode* op = GetScriptAllocator()->NewObj<CScriptOperatorASTNode>(opToken, (AST::CScriptASTNode*)NULL);

		// Parse r-value.
		AST::CScriptASTNode* rvalue = ParseExprLevel6();

		op->AddChild(lvalue);
		op->AddChild(rvalue);
			
		//op->GetChildren().AddToEnd(lvalue);
		//op->GetChildren().AddToEnd(rvalue);

		lvalue = op;
	}

	return lvalue;
}

// Parses precendence level 6, addition/subtraction operators.
AST::CScriptASTNode* CScriptParser::ParseExprLevel6()
{
	// Parse l-value
	AST::CScriptASTNode* lvalue = ParseExprLevel7();

	while (true)
	{
		// Do we have an operator next?
		CScriptToken opToken = LookAheadToken();
		if (opToken.ID != TOKEN_OP_ADD &&
			opToken.ID != TOKEN_OP_SUB)
		{
			return lvalue;
		}
		NextToken();

		// Create operator.
		CScriptOperatorASTNode* op = GetScriptAllocator()->NewObj<CScriptOperatorASTNode>(opToken, (AST::CScriptASTNode*)NULL);

		// Parse r-value.
		AST::CScriptASTNode* rvalue = ParseExprLevel7();

		op->AddChild(lvalue);
		op->AddChild(rvalue);
		//op->GetChildren().AddToEnd(lvalue);
		//op->GetChildren().AddToEnd(rvalue);

		lvalue = op;
	}

	return lvalue;
}

// Parses precendence level 7, multiply/divide/modulus operators.
AST::CScriptASTNode* CScriptParser::ParseExprLevel7()
{
	// Parse l-value
	AST::CScriptASTNode* lvalue = ParseExprLevel8();

	while (true)
	{
		// Do we have an operator next?
		CScriptToken opToken = LookAheadToken();
		if (opToken.ID != TOKEN_OP_MUL &&
			opToken.ID != TOKEN_OP_DIV &&
			opToken.ID != TOKEN_OP_MOD)
		{
			return lvalue;
		}
		NextToken();

		// Create operator.
		CScriptOperatorASTNode* op = GetScriptAllocator()->NewObj<CScriptOperatorASTNode>(opToken, (AST::CScriptASTNode*)NULL);

		// Parse r-value.
		AST::CScriptASTNode* rvalue = ParseExprLevel8();

		op->AddChild(lvalue);
		op->AddChild(rvalue);
		//op->GetChildren().AddToEnd(lvalue);
		//op->GetChildren().AddToEnd(rvalue);

		lvalue = op;
	}

	return lvalue;
}

// Parses precendence level 8, prefix inc/dec, unary plus/minus
AST::CScriptASTNode* CScriptParser::ParseExprLevel8()
{
	bool hasUnary = false;

	CScriptToken opToken = LookAheadToken();
	if (opToken.ID == TOKEN_OP_INCREMENT ||
		opToken.ID == TOKEN_OP_DECREMENT ||
		opToken.ID == TOKEN_OP_ADD ||
		opToken.ID == TOKEN_OP_SUB ||
		opToken.ID == TOKEN_OP_BITWISE_NOT ||
		opToken.ID == TOKEN_OP_LOGICAL_NOT)
	{
		NextToken();
		hasUnary = true;
	}
	
	// Parse l-value
	AST::CScriptASTNode* lvalue = ParseExprLevel9();

	// Perform unary operation.
	if (hasUnary == true)
	{
		CScriptOperatorASTNode* op = GetScriptAllocator()->NewObj<CScriptOperatorASTNode>(opToken, (AST::CScriptASTNode*)NULL);
		op->AddChild(lvalue);
		//op->GetChildren().AddToEnd(lvalue);

		lvalue = op;
	}

	return lvalue;
}

// Parses precendence level 9, postfix inc/dec, function call, subscript.
//
//	++
//	--
//	x.y
//	(args, go, here)
//	[index, index2, index3]  -  Subscript or list creation.
//  {index, index2, index3}  -  Dictionary creation.
AST::CScriptASTNode* CScriptParser::ParseExprLevel9()
{
	// Is it a list/dict?
	bool isList = false;
	bool isDict = false;
	CScriptToken t = LookAheadToken();
	if (LookAheadToken().ID == TOKEN_OP_OPEN_BRACE)
		isDict = true;
	if (LookAheadToken().ID == TOKEN_OP_OPEN_BRACKET)
		isList = true;

	// Parse l-value
	AST::CScriptASTNode* lvalue = NULL;
	if (isDict == false && isList == false)
		lvalue = ParseExprLevel10();

	while (true)
	{
		// Do we have an operator next?
		CScriptToken opToken = LookAheadToken();
		if (opToken.ID != TOKEN_OP_INCREMENT &&
			opToken.ID != TOKEN_OP_DECREMENT &&
			opToken.ID != TOKEN_OP_PERIOD &&
			opToken.ID != TOKEN_OP_OPEN_BRACKET &&
			opToken.ID != TOKEN_OP_OPEN_BRACE &&
			opToken.ID != TOKEN_OP_OPEN_PARENT)
		{
			return lvalue;
		}
		NextToken();

		// Create operator.
		CScriptASTNode* op = NULL;
		if (isList == true)
			op = GetScriptAllocator()->NewObj<CScriptListASTNode>(opToken, (AST::CScriptASTNode*)NULL);
		else if (isDict == true)
			op = GetScriptAllocator()->NewObj<CScriptDictASTNode>(opToken, (AST::CScriptASTNode*)NULL);
		else
		{
			op = GetScriptAllocator()->NewObj<CScriptOperatorASTNode>(opToken, (AST::CScriptASTNode*)NULL);
			((CScriptOperatorASTNode*)op)->PostFix = true;
		}

		if (lvalue != NULL)
		{
			op->AddChild(lvalue);
		}

		// Read sub-script / list.
		if (opToken.ID == TOKEN_OP_OPEN_BRACKET)
		{				
			while (LookAheadToken().ID != TOKEN_OP_CLOSE_BRACKET)
			{
				PushScope(op);
				ParseExpression();				
				PopScope();

				if (LookAheadToken().ID == TOKEN_OP_COMMA)
					NextToken();
				else
					break;
			}
			
			ExpectToken(TOKEN_OP_CLOSE_BRACKET);
		}

		// Read dictionary.
		else if (opToken.ID == TOKEN_OP_OPEN_BRACE)
		{				
			while (LookAheadToken().ID != TOKEN_OP_CLOSE_BRACE)
			{
				// Read key.
				CScriptToken key = ExpectToken(TOKEN_LITERAL_STRING);	
				CScriptASTNode* keyOp = GetScriptAllocator()->NewObj<CScriptLiteralASTNode>(key, (AST::CScriptASTNode*)NULL);
			//	op->GetChildren().AddToEnd(keyOp);
				op->AddChild(keyOp);


				// Read colon.
				ExpectToken(TOKEN_OP_COLON);

				// Read value.
				PushScope(op);
				ParseExpression();				
				PopScope();

				if (LookAheadToken().ID == TOKEN_OP_COMMA)
					NextToken();
				else
					break;
			}
			
			ExpectToken(TOKEN_OP_CLOSE_BRACE);
		}

		// Read function parameters.
		else if (opToken.ID == TOKEN_OP_OPEN_PARENT)
		{
			// Parse all the arguments of a function call.
			while (LookAheadToken().ID != TOKEN_OP_CLOSE_PARENT)
			{
				PushScope(op);
				ParseExpression();				
				PopScope();

				if (LookAheadToken().ID == TOKEN_OP_COMMA)
					NextToken();
				else
					break;
			}
			
			ExpectToken(TOKEN_OP_CLOSE_PARENT);
		}

		// Read in method.
		else if (opToken.ID == TOKEN_OP_PERIOD)
		{
			// Expect an identifier.
			ExpectToken(TOKEN_LITERAL_IDENTIFIER);

			// Create string symbol representing this identifier.
			CScriptToken tok = CurrentToken();
			tok.ID = TOKEN_LITERAL_STRING;

			AST::CScriptLiteralASTNode* rvalue = GetScriptAllocator()->NewObj<CScriptLiteralASTNode>(tok, (AST::CScriptASTNode*)NULL);

			// Add symbol 
			op->AddChild(rvalue);

			//AST::CScriptASTNode* rvalue = ParseExprLevel10();
			//op->AddChild(rvalue);
			//op->GetChildren().AddToEnd(rvalue);
		}

		lvalue = op;

		// Boot out if list/dict.
		if (isList == true || isDict == true)
			break;
	}

	return lvalue;
}

// Parses precendence level 10, literal values, sub expressions.
AST::CScriptASTNode* CScriptParser::ParseExprLevel10()
{
	CScriptToken token = NextToken();

	switch (token.ID)
	{
		case TOKEN_LITERAL_FLOAT:
		case TOKEN_LITERAL_INT:
		case TOKEN_LITERAL_STRING:
		case TOKEN_LITERAL_NULL:
			return GetScriptAllocator()->NewObj<CScriptLiteralASTNode>(token, (AST::CScriptASTNode*)NULL);

		case TOKEN_LITERAL_IDENTIFIER:
			return GetScriptAllocator()->NewObj<CScriptIdentifierASTNode>(token, (AST::CScriptASTNode*)NULL);

		case TOKEN_OP_OPEN_PARENT:
		{
			AST::CScriptASTNode* node = ParseExprLevel0();
			ExpectToken(TOKEN_OP_CLOSE_PARENT);
			return node;
		}

		// Native import intrinsic.
		case TOKEN_KEYWORD_NATIVE:
			{				
				AST::CScriptASTNode* op = GetScriptAllocator()->NewObj<CScriptIntrinsicASTNode>(token, (AST::CScriptASTNode*)NULL);

				ExpectToken(TOKEN_OP_OPEN_PARENT);
				AST::CScriptASTNode* node = ParseExprLevel0();
				ExpectToken(TOKEN_OP_CLOSE_PARENT);

				op->AddChild(node);
				
				return op;
			}

		default:
		{
			Error(S("Expecting expression factor, but encountered '%s' (0x%x).").Format(token.Literal.c_str(), token.ID));
			return NULL;
		}
	}

	return NULL;
}

