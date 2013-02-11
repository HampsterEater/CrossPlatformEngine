///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"
#include "CArray.h"

#include "CScriptLexer.h"

namespace Engine
{
    namespace Scripting
    {
		namespace AST
		{
			class CScriptASTNode;
		}

		struct CScriptToken;
		class CScriptCompileContext;

		// The script parser! Checks the syntax of the script and turns it into
		// an AST.
		class CScriptParser
		{
			private:
				CScriptCompileContext*								_context;

				s32													_tokenIndex;

				AST::CScriptASTNode*								_scope;
				AST::CScriptASTNode*								_globalScope;
				Engine::Containers::CArray<AST::CScriptASTNode*>	_scopeList;
				
				Engine::Containers::CArray<AST::CScriptASTNode*>	_continueLoopList;
				Engine::Containers::CArray<AST::CScriptASTNode*>	_breakLoopList;
		
			public:
				CScriptParser							();
				~CScriptParser							();

				bool					EndOfTokens		(u32 offset=0);
				CScriptToken			NextToken		();
				CScriptToken			CurrentToken	();
				CScriptToken			PreviousToken	();
				void					RewindStream	(u32 offset=1);
				CScriptToken			ExpectToken		(TokenID id);
				CScriptToken			LookAheadToken	(u32 offset=0);

				void					Error			(const Engine::Containers::CString& str);
				void					Warning			(const Engine::Containers::CString& str);
				void					Info			(const Engine::Containers::CString& str);

				bool					Analyze			(CScriptCompileContext* context);

				void					PushScope		(AST::CScriptASTNode* node);
				void					PopScope		();
				
				void					PushBreakLoop	(AST::CScriptASTNode* node);
				void					PopBreakLoop	();

				void					PushContinueLoop(AST::CScriptASTNode* node);
				void					PopContinueLoop	();

				bool					IsInGenerator	();
				bool					IsInFunction	();
				bool					IsInState		();
				bool					IsInGlobalScope	();

				void					ParseStatement	();				
				void					ParseBlock		();				
				void					ParseVar		();				
				void					ParseFunction	();
				void					ParseFor		();			
				void					ParseWhile		();	
				void					ParseDo			();				
				void					ParseForeach	();				
				void					ParseReturn		();				
				void					ParseYield		();
				void					ParseSwitch		();
				void					ParseIf			();
				void					ParseBreak		();
				void					ParseContinue	();
				void					ParseUsing		();
				void					ParseClass		();
				void					ParseState		();
				void					ParseGotoState	();
				//void					ParseEnum		();
				//void					ParseProperty	();
				//void					ParseStruct		();
				//void					ParseTry		();
				//void					ParseMetaData	();
				// Is
				// As

				void					ParseExpression	();
				AST::CScriptASTNode*	ParseExprLevel0	(); // As operators
				AST::CScriptASTNode*	ParseExprLevel1	(); // Assignment operators
				AST::CScriptASTNode*	ParseExprLevel2	(); // Ternary operators
				AST::CScriptASTNode*	ParseExprLevel3	(); // Logical operators
				AST::CScriptASTNode*	ParseExprLevel4	(); // Bitwise operators
				AST::CScriptASTNode*	ParseExprLevel5	(); // Conditional operators
				AST::CScriptASTNode*	ParseExprLevel6	(); // Addition/Subtract
				AST::CScriptASTNode*	ParseExprLevel7	(); // Multiply/Divide/Modulus
				AST::CScriptASTNode*	ParseExprLevel8	(); // Prefix inc/dec, unary plus/minus
				AST::CScriptASTNode*	ParseExprLevel9	(); // Postix inc/dec, function call, subscript
				AST::CScriptASTNode*	ParseExprLevel10(); // Literal values.
		};

	}
}