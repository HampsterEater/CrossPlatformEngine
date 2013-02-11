///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CArray.h"
#include "CString.h"

#include "CScriptLexer.h"
#include "CScriptInstruction.h"
#include "CScriptJumpTargetSymbol.h"
#include "CScriptGenerator.h"
#include "CScriptSymbol.h"


namespace Engine
{
    namespace Scripting
    {
		struct CScriptToken;
		class CScriptGenerator;

		namespace Symbols
		{
			class CScriptSymbol;
			class CScriptStateSymbol;
		}

		namespace AST
		{
			class CScriptFunctionASTNode;

			// This is the base class from which all of our abstract syntax tree
			// nodes derive from.
			class CScriptASTNode
			{
				protected:
					CScriptASTNode*													_parent;
					Engine::Scripting::CScriptToken									_token;
					Engine::Containers::CArray<CScriptASTNode*>						_children;
					Engine::Containers::CArray<Symbols::CScriptSymbol*>				_symbols;
				
					Symbols::CScriptJumpTargetSymbol* _continueJumpTarget;
					Symbols::CScriptJumpTargetSymbol* _breakJumpTarget;
			
				public:
					CScriptASTNode					();
					CScriptASTNode					(Engine::Scripting::CScriptToken token, CScriptASTNode* parent);
					~CScriptASTNode					();

					void										 PrettyPrint			(u32 tabs=0);
					
					void										 SetParent				(CScriptASTNode* node);
					CScriptASTNode*								 GetParent				();
					Engine::Containers::CArray<CScriptASTNode*>& GetChildren			();
					Engine::Scripting::CScriptToken				 GetToken				();
					virtual Engine::Containers::CString			 GetName				();

					Symbols::CScriptStateSymbol*			 	 GetStateSymbol			();
							
					void										 AddChild				(CScriptASTNode* node);

					// Symbol creation/modification.
					Engine::Containers::CArray<Symbols::CScriptSymbol*>&  GetSymbols				();
					Symbols::CScriptSymbol*								  FindSymbol				(Engine::Containers::CString& identifier, bool recursive = true, u32 type = 0, u32 exceptType = (u32)Symbols::SCRIPT_SYMBOL_TYPE_STRING);
					void												  AddSymbolToParent		    (Symbols::CScriptSymbol* symbol);
					void												  AddSymbol				    (Symbols::CScriptSymbol* symbol);

					// Instruction creation/modification.
					void															AddInstruction			(CScriptGenerator* gen, Instructions::CScriptInstruction* instr);

					Instructions::CScriptInstruction*								CreateInstruction				(CScriptGenerator* gen, Instructions::ScriptInstructionOpCodes opcode);
					Instructions::CScriptInstruction*								CreateInstruction				(CScriptGenerator* gen, Instructions::ScriptInstructionOpCodes opcode, Instructions::CScriptOperand op1);
					Instructions::CScriptInstruction*								CreateInstruction				(CScriptGenerator* gen, Instructions::ScriptInstructionOpCodes opcode, Instructions::CScriptOperand op1, Instructions::CScriptOperand op2);
					Instructions::CScriptInstruction*								CreateInstruction				(CScriptGenerator* gen, Instructions::ScriptInstructionOpCodes opcode, Instructions::CScriptOperand op1, Instructions::CScriptOperand op2, Instructions::CScriptOperand op3);
					Instructions::CScriptOperand									CreateIntOperand				(u32 value);
					Instructions::CScriptOperand									CreateFloatOperand				(f32 value);
					Instructions::CScriptOperand									CreateRegisterOperand			(u32 registerIndex);
					Instructions::CScriptOperand									CreateSymbolOperand				(Symbols::CScriptSymbol* symbol);
					Instructions::CScriptOperand									CreateStackIndexOperand			(s32 stackIndex);
					Instructions::CScriptOperand									CreateInstructionIndexOperand	(s32 stackIndex);
					Instructions::CScriptOperand									CreateJumpTargetOperand			(Symbols::CScriptJumpTargetSymbol* symbol);

					// Jump targets.	
					Symbols::CScriptJumpTargetSymbol*								GetBreakJumpTarget				();
					Symbols::CScriptJumpTargetSymbol*								GetContinueJumpTarget			();

					// These are the main methods used for walking the tree and generating
					// some actual useful data!
					virtual void								 GenerateSymbols		(CScriptGenerator* gen);
					virtual u32									 GenerateInstructions	(CScriptGenerator* gen);
					
					void										 PerformAssignment		(CScriptGenerator* gen, CScriptASTNode* lvalue, u32 rvalueRegister);

					void										LoadRegister			(CScriptGenerator* gen, Symbols::CScriptSymbol* symbol, u32 reg);
					void										StoreRegister			(CScriptGenerator* gen, Symbols::CScriptSymbol* symbol, u32 reg);

				friend class CScriptGenerator;
			};

		}
	}
}