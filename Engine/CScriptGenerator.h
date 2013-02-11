///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CScriptInstruction.h"

namespace Engine
{
    namespace Scripting
    {
		namespace AST
		{
			class CScriptASTNode;
		}

		class CScriptCompileContext;

		// The script generator is responsible for taking an AST representation of source
		// code and turning it into symbols and byte code.
		class CScriptGenerator
		{
			private:
				CScriptCompileContext* _context;
				bool				   _registersAllocated[SCRIPT_MAX_GEN_PURPOSE_REGISTER];
				
			public:
				CScriptGenerator				();
				~CScriptGenerator				();

				CScriptCompileContext*	GetContext();

				void	Error					(AST::CScriptASTNode* node, const Engine::Containers::CString& str);
				void	Warning					(AST::CScriptASTNode* node, const Engine::Containers::CString& str);
				void	Info					(AST::CScriptASTNode* node, const Engine::Containers::CString& str);

				bool	Analyze					(CScriptCompileContext* context);

				void	GenerateSymbolList		(AST::CScriptASTNode* node);
				void	GenerateNonGlobalScope	(AST::CScriptASTNode* root);
				
				u32		AllocateRegister		(AST::CScriptASTNode* node);
				u32		AllocateRegister		(AST::CScriptASTNode* node, u32 idx);
				void	DeallocateRegister		(AST::CScriptASTNode* node, u32 idx);

				void	Disassemble				();

		};

	}
}