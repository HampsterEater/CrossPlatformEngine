///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"
#include "CScriptSymbol.h"

#include "CScriptLexer.h"

namespace Engine
{
    namespace Scripting
    {
		namespace AST
		{
			class CScriptASTNode;
		}
		namespace Instructions
		{

			#define SCRIPT_MAX_OPERAND_COUNT 3

			// All the different kinds of instructions we can use!
			enum ScriptInstructionOpCodes
			{
				#define X(v) SCRIPT_OPCODE_ ## v,
				#include "opcodes.def"
				#undef X
			};
			char const* const ScriptInstructionOpCodes_String[] = {
				#define X(v) #v,
				#include "opcodes.def"
				#undef X
			};

			// The different types of operands.
			enum ScriptOperandType
			{
				SCRIPT_OPERAND_LITERAL_INT,
				SCRIPT_OPERAND_LITERAL_FLOAT,
				
				SCRIPT_OPERAND_REGISTER,
				SCRIPT_OPERAND_INSTRUCTION,
				SCRIPT_OPERAND_STACK_INDEX,
				SCRIPT_OPERAND_SYMBOL,
				SCRIPT_OPERAND_JUMP_TARGET,
			};

			// Register map.
			//		
			//		0	 :	Constant, always equal to 0.
			//		1  	 :	Constant, always equal to 1.
			//		2	 :  Return value of function.
			//		3	 :  Comparison result registor.
			//		4-31 :	General use.
			#define SCRIPT_CONST_REGISTER_ZERO		0
			#define SCRIPT_CONST_REGISTER_ONE		1
			#define SCRIPT_CONST_REGISTER_RETURN	2
			#define SCRIPT_CONST_REGISTER_CMP		3
			#define SCRIPT_MIN_GEN_PURPOSE_REGISTER	4
			#define SCRIPT_MAX_GEN_PURPOSE_REGISTER	31
			#define	SCRIPT_TOTAL_REGISTERS			32
			
			// Base class for operands assigned to an instruction.
			struct CScriptOperand
			{
				ScriptOperandType Type;
				union
				{
					// Literal operands.
					u32 IntLiteral;
					f32 FloatLiteral;

					// Register Operand.
					u32 RegisterIndex;
				
					// Instruction Operand.
					u32 InstructionIndex;
				
					// Stack-Index Operand.
					s32 StackIndex;
				
					// Symbol Operand.
					// Jump Target Operand.
					Engine::Scripting::Symbols::CScriptSymbol* Symbol;
				};
			};

			// Defines an instruction within a script that
			// can be executed.
			class CScriptInstruction
			{
				private:

				public:
					CScriptToken	Token;

					ScriptInstructionOpCodes	Opcode;
					u32							OperandCount;
					CScriptOperand				Operands[SCRIPT_MAX_OPERAND_COUNT];
			};

		}
	}
}