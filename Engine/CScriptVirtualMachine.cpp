///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptVirtualMachine.h"
#include "CScriptManager.h"
#include "CScriptVariableSymbol.h"
#include "CScriptFunctionSymbol.h"

#include "CScriptObject.h"
#include "CScriptStringObject.h"
#include "CScriptListObject.h"
#include "CScriptDictObject.h"
#include "CScriptContextObject.h"
#include "CScriptIteratorObject.h"

// Include our automatically generated scripting glue.
#include "ScriptGlue.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::Instructions;
using namespace Engine::Scripting::Objects;

// CScriptExecutionContext ---------------------------------------------------

CScriptExecutionContext::CScriptExecutionContext(CScriptCompileContext* context)
{
	_context			  = context;
	_globalScopeRun		  = false;

	_currentContext		  = NULL;
	_gcLastRun			  = 0;

	_instructionsExecuted = 0;
	_instructionTimer	  = (f32)Engine::Platform::GetMillisecs();

	_state								= NULL;
	_globalScopeSymbolHashTableCreated	= false;

	// Allocate globals table.
	_globals			  = GetScriptAllocator()->AllocArray<CScriptValue>(context->_globalVariableCount);
	for (u32 i = 0; i < context->_globalVariableCount; i++)
	{
		_globals[i].Type = SCRIPT_VALUE_TYPE_NULL;
	}

	// Initialize the function table.
	_functionTable		  = GetScriptAllocator()->AllocArray<CScriptValue>(context->_functionTableSize);
	for (u32 i = 0; i < context->_symbols.Size(); i++)
	{
		if (context->_symbols[i]->GetType() == SCRIPT_SYMBOL_TYPE_FUNCTION)
		{
			CScriptFunctionSymbol* func = reinterpret_cast<CScriptFunctionSymbol*>(context->_symbols[i]);

			_functionTable[func->Index].Type = SCRIPT_VALUE_TYPE_FUNCTION;
			_functionTable[func->Index].Symbol = context->_symbols[i];
		}
	}		

	// Copy instructions into flat array (more cache friendly).
	_instructions = GetScriptAllocator()->AllocArray<Instructions::CScriptInstruction*>(context->_instructions.Size());
	for (u32 i = 0; i < context->_instructions.Size(); i++)
	{
		_instructions[i] = context->_instructions[i];
	}
	
	// Copy symbols into flat array (more cache friendly).
	_symbols = GetScriptAllocator()->AllocArray<Symbols::CScriptSymbol*>(context->_symbols.Size());
	for (u32 i = 0; i < context->_symbols.Size(); i++)
	{
		_symbols[i] = context->_symbols[i];
		if (_symbols[i]->GetType() == SCRIPT_SYMBOL_TYPE_STATE &&
			dynamic_cast<CScriptStateSymbol*>(_symbols[i])->IsDefault == true)
		{
			_state = reinterpret_cast<CScriptStateSymbol*>(_symbols[i]);
		}
	}

	// Create the GC pool.
	for (u32 i = 0; i < SCRIPT_MAX_GC_GENERATIONS; i++)
	{
		_gcObjectPool[i] = NULL;
		_gcObjectPoolDirty[i] = false;
	}
}

CScriptExecutionContext::~CScriptExecutionContext()
{
	_context = NULL;

	// Dispose of scope hash tables.
	_globalScopeHashTable.Clear();
	for (u32 i = 0; i < _stateScopeHashTable.Size(); i++)
	{
		Engine::Containers::CHashTable<CScriptSymbol*>* subArray = _stateScopeHashTable[i];
	//	subArray->Clear();
		GetScriptAllocator()->FreeObj<Engine::Containers::CHashTable<CScriptSymbol*>>(&subArray);
	}

	// Dispose of call-stack stuff.
	_callStack.Clear();
	_parameterStack.Clear();

	if (_globals != NULL)
		GetScriptAllocator()->FreeArray(&_globals);
	
	if (_functionTable != NULL)
		GetScriptAllocator()->FreeArray(&_functionTable);
	
	if (_instructions != NULL)
		GetScriptAllocator()->FreeArray(&_instructions);
	
	if (_symbols != NULL)
		GetScriptAllocator()->FreeArray(&_symbols);

	// Dispose the GC pool.
	for (u32 i = 0; i < SCRIPT_MAX_GC_GENERATIONS; i++)
	{
		CScriptObject* obj = _gcObjectPool[i];
		while (obj != NULL)
		{
			CScriptObject* next = obj->_next;
			GetScriptAllocator()->FreeObj(&obj);
			obj = next;
		}
	}
}

// Executs the next instruction in the context, returns true if 
// we are giving up the rest of our timeslice, or false if we want
// to continue running.
bool CScriptExecutionContext::Execute()
{
	// Grab the context we are executing on.
	CScriptCallContext* context = _currentContext;
	
	// Instruction valid?
	//LOG_ASSERT(context->PC < _context->_instructions.Size());
	CScriptInstruction* instruction = _instructions[context->PC++];

	// Keep track of instructions executed.
	_instructionsExecuted++;

	// Run the GC?
	if (_instructionsExecuted - _gcLastRun == SCRIPT_VM_GC_INTERVAL)
	{
		GCExecute();
		_gcLastRun = _instructionsExecuted;
	}

	// Debug output.
//	printf("EXECUTED[%i] %s\n", context->PC - 1, Instructions::ScriptInstructionOpCodes_String[instruction->Opcode]);

	// Massive Switch Block! GOOO!
	switch (instruction->Opcode)
	{
		// --------------------------------------------------------------------------------------------
		// Load / Store
		// ------------------------------------------------ --------------------------------------------
		case SCRIPT_OPCODE_LDI:			// load reg, int
			{
				u32 dstRegister = instruction->Operands[0].RegisterIndex;
				s32 value       = instruction->Operands[1].IntLiteral;

				context->Registers[dstRegister].Type		= SCRIPT_VALUE_TYPE_INT;
				context->Registers[dstRegister].IntValue	= value;
			}
			break;

		case SCRIPT_OPCODE_LDF:			// load	reg, float
			{
				u32 dstRegister = instruction->Operands[0].RegisterIndex;
				f32 value       = instruction->Operands[1].FloatLiteral;

				context->Registers[dstRegister].Type		= SCRIPT_VALUE_TYPE_FLOAT;
				context->Registers[dstRegister].FloatValue	= value;
			}
			break;
			
		case SCRIPT_OPCODE_LDS:			// loadstring	reg, index
			{
				u32 dstRegister						= instruction->Operands[0].RegisterIndex;
				Engine::Containers::CString& value  = instruction->Operands[1].Symbol->GetToken().Literal;

				CScriptStringObject* strObj			= Engine::Scripting::GetScriptAllocator()->NewObj<CScriptStringObject>(this, value);
				GCAdd(strObj);

				context->Registers[dstRegister].Type	= SCRIPT_VALUE_TYPE_OBJECT;
				context->Registers[dstRegister].Object	= strObj;
			}
			break;
			
		case SCRIPT_OPCODE_LDN:			// loadnull	reg
			{
				u32 dstRegister = instruction->Operands[0].RegisterIndex;

				context->Registers[dstRegister].Type = SCRIPT_VALUE_TYPE_NULL;
			}
			break;

		case SCRIPT_OPCODE_LFUNC:		// loadfunc    reg, index
			{
				u32 dstRegister = instruction->Operands[0].RegisterIndex;
				u32 index       = instruction->Operands[1].IntLiteral;

				context->Registers[dstRegister] = _functionTable[index];
			}
			break;

		case SCRIPT_OPCODE_SFUNC:		// storefunc   reg, index, value
			{
				u32 dstRegister = instruction->Operands[0].RegisterIndex;
				u32 index       = instruction->Operands[1].IntLiteral;
				
				AssignTo(_functionTable[index], context->Registers[dstRegister]);
			}
			break;

		case SCRIPT_OPCODE_LLOCAL:		// loadlocal   reg, index
			{
				u32 dstRegister = instruction->Operands[0].RegisterIndex;
				u32 index       = instruction->Operands[1].IntLiteral;

				context->Registers[dstRegister] = context->Locals[index];
			}
			break;

		case SCRIPT_OPCODE_SLOCAL:		// storelocal  reg, index, value
			{
				u32 dstRegister = instruction->Operands[0].RegisterIndex;
				u32 index       = instruction->Operands[1].IntLiteral;

				AssignTo(context->Locals[index], context->Registers[dstRegister]);
			}
			break;

		case SCRIPT_OPCODE_LGLOBAL:		// loadglobal  reg, index
			{
				u32 dstRegister = instruction->Operands[0].RegisterIndex;
				u32 index       = instruction->Operands[1].IntLiteral;

				context->Registers[dstRegister] = _globals[index];
			}
			break;

		case SCRIPT_OPCODE_SGLOBAL:		// storeglobal reg, index, value
			{
				u32 dstRegister = instruction->Operands[0].RegisterIndex;
				u32 index       = instruction->Operands[1].IntLiteral;

				AssignTo(_globals[index], context->Registers[dstRegister]);
			}
			break;

		// --------------------------------------------------------------------------------------------
		// Arithmatic
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_MOV:			// mov dest, src
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				dest = src;
			}
			break;

		case SCRIPT_OPCODE_ADD:			// add dest, src
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				// Perform coercion of arguments.
				ImplicitCast(dest, src);

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue		+= src.IntValue;	break;
					case SCRIPT_VALUE_TYPE_FLOAT:	dest.FloatValue		+= src.FloatValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->Add(this, dest, src))
							break;
					default:						Error(S("Attempt to perform addition on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_SUB:			// sub dest, src
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				// Perform coercion of arguments.
				ImplicitCast(dest, src);

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue		-= src.IntValue;	break;
					case SCRIPT_VALUE_TYPE_FLOAT:	dest.FloatValue		-= src.FloatValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->Sub(this, dest, src))
							break;
					default:						Error(S("Attempt to perform subtraction on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_MUL:			// mul dest, src
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				// Perform coercion of arguments.
				ImplicitCast(dest, src);

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue		*= src.IntValue;	break;
					case SCRIPT_VALUE_TYPE_FLOAT:	dest.FloatValue		*= src.FloatValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->Mul(this, dest, src))
							break;
					default:						Error(S("Attempt to perform multiplication on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_DIV:			// div dest, src
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				// Perform coercion of arguments.
				ImplicitCast(dest, src);

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue		/= src.IntValue;	break;
					case SCRIPT_VALUE_TYPE_FLOAT:	dest.FloatValue		/= src.FloatValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->Div(this, dest, src))
							break;
					default:						Error(S("Attempt to perform division on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_INC:			// inc dest
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				
				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue		+= 1;	break;
					case SCRIPT_VALUE_TYPE_FLOAT:	dest.FloatValue		+= 1;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->Inc(this, dest))
							break;
					default:						Error(S("Attempt to perform increment on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_DEC:			// dec dest
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				
				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue		-= 1;	break;
					case SCRIPT_VALUE_TYPE_FLOAT:	dest.FloatValue		-= 1;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->Dec(this, dest))
							break;
					default:						Error(S("Attempt to perform increment on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_NEG:			// neg dest
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				
				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue		= -dest.IntValue;	break;
					case SCRIPT_VALUE_TYPE_FLOAT:	dest.FloatValue		= -dest.FloatValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->Neg(this, dest))
							break;
					default:						Error(S("Attempt to perform negatation on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_ABS:			// abs dest
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				
				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue		= dest.IntValue >= 0 ? dest.IntValue : -dest.IntValue;	break;
					case SCRIPT_VALUE_TYPE_FLOAT:	dest.FloatValue		= dest.FloatValue >= 0 ? dest.FloatValue : -dest.FloatValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->Abs(this, dest))
							break;
					default:						Error(S("Attempt to perform absolution on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_MOD:			// mod dest, src=			
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				// Perform coercion of arguments.
				//ImplicitCast(dest, src);

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue %= src.IntValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:		
						if (dest.Object->Mod(this, src, dest))
							break;
					default:						Error(S("Attempt to perform modulus on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_BWOR:		// bwor	 dest, src
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				// Perform coercion of arguments.
				//ImplicitCast(dest, src);

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue |= src.IntValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->BWOr(this, src, dest))
							break;
					default:						Error(S("Attempt to perform bitwise or on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_BWXOR:		// bwxor dest, src
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				// Perform coercion of arguments.
				//ImplicitCast(dest, src);

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue ^= src.IntValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->BWXor(this, src, dest))
							break;
					default:						Error(S("Attempt to perform bitwise xor on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_BWAND:		// bwand dest, src
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				// Perform coercion of arguments.
				//ImplicitCast(dest, src);

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue &= src.IntValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:		
						if (dest.Object->BWAnd(this, src, dest))
							break;
					default:						Error(S("Attempt to perform bitwise and on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_BWNOT:		// bwnot dest
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue = !dest.IntValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:		
						if (dest.Object->BWNot(this, dest))
							break;
					default:						Error(S("Attempt to perform bitwise not on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_BWSHL:		// bwshl dest, src
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				// Perform coercion of arguments.
				//ImplicitCast(dest, src);

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue <<= src.IntValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->BWShl(this, src, dest))
							break;
					default:						Error(S("Attempt to perform bitwise shift left on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_BWSHR:		// bwshr dest, src
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				// Perform coercion of arguments.
				//ImplicitCast(dest, src);

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		dest.IntValue >>= src.IntValue;	break;
					case SCRIPT_VALUE_TYPE_OBJECT:		
						if (dest.Object->BWShr(this, src, dest))
							break;
					default:						Error(S("Attempt to perform bitwise shift right on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		// --------------------------------------------------------------------------------------------
		// Branching
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_JMP:			// jmp	 address
			{
				context->PC = instruction->Operands[0].InstructionIndex;
				break;
			}

		case SCRIPT_OPCODE_JEQ:			// jeq	 address
			{
				if (context->Registers[SCRIPT_CONST_REGISTER_CMP].IntValue == 0)
				{
					context->PC = instruction->Operands[0].InstructionIndex;
				}
				break;
			}

		case SCRIPT_OPCODE_JL:			// jl	 address
			{
				if (context->Registers[SCRIPT_CONST_REGISTER_CMP].IntValue < 0)
				{
					context->PC = instruction->Operands[0].InstructionIndex;
				}
				break;
			}

		case SCRIPT_OPCODE_JG:			// jg	 address
			{
				if (context->Registers[SCRIPT_CONST_REGISTER_CMP].IntValue > 0)
				{
					context->PC = instruction->Operands[0].InstructionIndex;
				}
				break;
			}

		case SCRIPT_OPCODE_JLE:			// jle	 address
			{
				if (context->Registers[SCRIPT_CONST_REGISTER_CMP].IntValue <= 0)
				{
					context->PC = instruction->Operands[0].InstructionIndex;
				}
				break;
			}

		case SCRIPT_OPCODE_JGE:			// jge	 address
			{
				if (context->Registers[SCRIPT_CONST_REGISTER_CMP].IntValue >= 0)
				{
					context->PC = instruction->Operands[0].InstructionIndex;
				}
				break;
			}

		case SCRIPT_OPCODE_JNE:			// jne	 address
			{
				if (context->Registers[SCRIPT_CONST_REGISTER_CMP].IntValue != 0)
				{
					context->PC = instruction->Operands[0].InstructionIndex;
				}
				break;
			}

		case SCRIPT_OPCODE_CMP:			// cmp	 reg1, reg2
			{		
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				context->Registers[SCRIPT_CONST_REGISTER_CMP].Type	 = SCRIPT_VALUE_TYPE_INT;
				context->Registers[SCRIPT_CONST_REGISTER_CMP].IntValue = CompareValues(dest, src);
				
				break;
			}

		case SCRIPT_OPCODE_IEQ:			// ieq	 reg1, reg2
			{		
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				dest.IntValue = CompareValues(dest, src) == 0 ? 1 : 0;
				dest.Type	  = SCRIPT_VALUE_TYPE_INT;
				
				break;
			}

		case SCRIPT_OPCODE_IL:			// il	 reg1, reg2
			{		
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];
				
				dest.IntValue = CompareValues(dest, src) < 0 ? 1 : 0;
				dest.Type	  = SCRIPT_VALUE_TYPE_INT;

				break;
			}

		case SCRIPT_OPCODE_IG:			// ig	 reg1, reg2
			{		
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];
				
				dest.IntValue = CompareValues(dest, src) > 0 ? 1 : 0;
				dest.Type	  = SCRIPT_VALUE_TYPE_INT;

				break;
			}

		case SCRIPT_OPCODE_ILE:			// ile	 reg1, reg2
			{		
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];
				
				dest.IntValue = CompareValues(dest, src) <= 0 ? 1 : 0;
				dest.Type	  = SCRIPT_VALUE_TYPE_INT;

				break;
			}

		case SCRIPT_OPCODE_IGE:			// ige	 reg1, reg2
			{		
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];
				
				dest.IntValue = CompareValues(dest, src) >= 0 ? 1 : 0;
				dest.Type	  = SCRIPT_VALUE_TYPE_INT;

				break;
			}

		case SCRIPT_OPCODE_INE:			// ine	 reg1, reg2
			{		
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];
				
				dest.IntValue = CompareValues(dest, src) != 0 ? 1 : 0;
				dest.Type	  = SCRIPT_VALUE_TYPE_INT;

				break;
			}			

		// --------------------------------------------------------------------------------------------
		// Boolean Logic
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_LAND:		// land	 reg1, reg2
			{		
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				bool srcValid  = false;
				bool destValid = false;

				switch (src.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		srcValid = (src.IntValue != 0);				break;
					case SCRIPT_VALUE_TYPE_FLOAT:	srcValid = (src.FloatValue != 0);			break;
					case SCRIPT_VALUE_TYPE_NULL:	srcValid = false;							break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (src.Object->CoerceToBool(this, srcValid))
							break;
					default:						Error(S("Attempt to perform logical and on invalid data type '%s'.").Format(GetDataTypeName(src).c_str()), instruction);	break;
				}
				
				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		destValid = (dest.IntValue != 0);			break;
					case SCRIPT_VALUE_TYPE_FLOAT:	destValid = (dest.FloatValue != 0);			break;
					case SCRIPT_VALUE_TYPE_NULL:	destValid = false;							break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->CoerceToBool(this, destValid))
							break;
					default:						Error(S("Attempt to perform logical and on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}

				dest.Type	   = SCRIPT_VALUE_TYPE_INT;
				dest.IntValue = srcValid == true && destValid == true ? 1 : 0;

				break;
			}	
		
		case SCRIPT_OPCODE_LOR:			// lor	 reg1, reg2
			{		
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& src  = context->Registers[instruction->Operands[1].RegisterIndex];

				bool srcValid  = false;
				bool destValid = false;

				switch (src.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		srcValid = (src.IntValue != 0);				break;
					case SCRIPT_VALUE_TYPE_FLOAT:	srcValid = (src.FloatValue != 0);			break;
					case SCRIPT_VALUE_TYPE_NULL:	srcValid = false;							break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (src.Object->CoerceToBool(this, srcValid))
							break;
					default:						Error(S("Attempt to perform logical and on invalid data type '%s'.").Format(GetDataTypeName(src).c_str()), instruction);	break;
				}
				
				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		destValid = (dest.IntValue != 0);			break;
					case SCRIPT_VALUE_TYPE_FLOAT:	destValid = (dest.FloatValue != 0);			break;
					case SCRIPT_VALUE_TYPE_NULL:	destValid = false;							break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->CoerceToBool(this, destValid))
							break;
					default:						Error(S("Attempt to perform logical or on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}

				dest.Type	   = SCRIPT_VALUE_TYPE_INT;
				dest.IntValue = srcValid == true || destValid == true ? 1 : 0;

				break;
			}	

		case SCRIPT_OPCODE_LNOT:		// lnot	 reg1
			{		
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];

				bool destValid = false;

				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_INT:		destValid = (dest.IntValue != 0);			break;
					case SCRIPT_VALUE_TYPE_FLOAT:	destValid = (dest.FloatValue != 0);			break;
					case SCRIPT_VALUE_TYPE_NULL:	destValid = false;							break;
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->CoerceToBool(this, destValid))
							break;
					default:						Error(S("Attempt to perform logical not on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}

				dest.Type	  = SCRIPT_VALUE_TYPE_INT;
				dest.IntValue = destValid == true ? 0 : 1;

				break;
			}	

		// --------------------------------------------------------------------------------------------
		// Subscript
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_IDX:			// idx	reg1, reg2					- Get symbol at index.
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& idx  = context->Registers[instruction->Operands[1].RegisterIndex];
				
				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->GetIndex(this, dest, idx))
							break;
					default:	Error(S("Attempt to perform subscript on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		case SCRIPT_OPCODE_IDXS:		// idx	reg1, reg2, valuereg 		- Set symbol at index.
			{
				CScriptValue& dest = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& idx  = context->Registers[instruction->Operands[1].RegisterIndex];
				CScriptValue& val  = context->Registers[instruction->Operands[2].RegisterIndex];
				
				switch (dest.Type)
				{
					case SCRIPT_VALUE_TYPE_OBJECT:	
						if (dest.Object->SetIndex(this, dest, idx, val))
							break;
					default:	Error(S("Attempt to perform subscript on invalid data type '%s'.").Format(GetDataTypeName(dest).c_str()), instruction);	break;
				}
			}
			break;

		// --------------------------------------------------------------------------------------------
		// Lists
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_LISTNEW:		// listnew register
			{
				u32 dstRegister			   = instruction->Operands[0].RegisterIndex;

				CScriptListObject* listObj = Engine::Scripting::GetScriptAllocator()->NewObj<CScriptListObject>(this);
				GCAdd(listObj);

				context->Registers[dstRegister].Type	= SCRIPT_VALUE_TYPE_OBJECT;
				context->Registers[dstRegister].Object	= listObj;

				break;
			}

		case SCRIPT_OPCODE_LISTADD:		// listadd register, value_register	
			{
				CScriptValue& listRegister = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& valRegister  = context->Registers[instruction->Operands[1].RegisterIndex];

				if (listRegister.Type == SCRIPT_VALUE_TYPE_OBJECT &&
					typeid(CScriptListObject) == typeid(*listRegister.Object))
				{					
					CScriptListObject* list = dynamic_cast<CScriptListObject*>(listRegister.Object);
					list->AddItem(this, valRegister);
				}
				else
				{
					Error(S("Attempt to add item to list of invalid data type '%s'.").Format(GetDataTypeName(listRegister).c_str()), instruction);
				}

				break;
			}
			
		// --------------------------------------------------------------------------------------------
		// Dictionaries
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_DICTNEW:		// dictnew register
			{
				u32 dstRegister			   = instruction->Operands[0].RegisterIndex;

				CScriptDictObject* listObj = Engine::Scripting::GetScriptAllocator()->NewObj<CScriptDictObject>(this);
				GCAdd(listObj);

				context->Registers[dstRegister].Type	= SCRIPT_VALUE_TYPE_OBJECT;
				context->Registers[dstRegister].Object	= listObj;

				break;
			}

		case SCRIPT_OPCODE_DICTADD:		// dictadd register, key_register, value_register
			{
				CScriptValue& listRegister = context->Registers[instruction->Operands[0].RegisterIndex];
				CScriptValue& keyRegister  = context->Registers[instruction->Operands[1].RegisterIndex];
				CScriptValue& valRegister  = context->Registers[instruction->Operands[2].RegisterIndex];

				if (listRegister.Type == SCRIPT_VALUE_TYPE_OBJECT &&
					typeid(CScriptDictObject) == typeid(*listRegister.Object))
				{					
					CScriptDictObject* list = dynamic_cast<CScriptDictObject*>(listRegister.Object);
					list->AddItem(this, keyRegister, valRegister);
				}
				else
				{
					Error(S("Attempt to add item to dictionary of invalid data type '%s'.").Format(GetDataTypeName(listRegister).c_str()), instruction);
				}

				break;
			}

		// --------------------------------------------------------------------------------------------
		// Indirection
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_INDR:		// indr reg1, symbol    - This takes the object in reg1 and returns the object with the same name as symbol inside it. eg. module.x would turn into indr module, x		
			{
				CScriptValue& objRegister			   = context->Registers[instruction->Operands[0].RegisterIndex];
				Engine::Containers::CString symbolName = CoerceToString(context->Registers[instruction->Operands[1].RegisterIndex]);

				if (objRegister.Type == SCRIPT_VALUE_TYPE_OBJECT && objRegister.Object != NULL)
				{					
					if (!objRegister.Object->GetAttribute(this, objRegister, symbolName))
					{
						Error(S("Attempt to access invalid attribute '%s' of object '%s'.").Format(symbolName.c_str(), GetDataTypeName(objRegister).c_str()), instruction);
					}
				}
				else
				{
					Error(S("Attempt to access attribute of invalid data type '%s'.").Format(GetDataTypeName(objRegister).c_str()), instruction);
				}

				break;
			}
		case SCRIPT_OPCODE_INDRS:		// indrs reg1, symbol, valuereg   - Same as above, except it sets rather than gets the value.
			{
				CScriptValue& objRegister			   = context->Registers[instruction->Operands[0].RegisterIndex];
				Engine::Containers::CString symbolName = CoerceToString(context->Registers[instruction->Operands[1].RegisterIndex]);
				CScriptValue& valueRegister			   = context->Registers[instruction->Operands[2].RegisterIndex];

				if (objRegister.Type == SCRIPT_VALUE_TYPE_OBJECT && objRegister.Object != NULL)
				{					
					if (!objRegister.Object->SetAttribute(this, symbolName, valueRegister))
					{
						Error(S("Attempt to access invalid attribute '%s' of object '%s'.").Format(symbolName.c_str(), GetDataTypeName(objRegister).c_str()), instruction);
					}
				}
				else
				{
					Error(S("Attempt to access attribute of invalid data type '%s'.").Format(GetDataTypeName(objRegister).c_str()), instruction);
				}

				break;
			}
			
		// --------------------------------------------------------------------------------------------
		// Parameter Stack
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_PUSH:		// push register
			{
				CScriptValue& objRegister = context->Registers[instruction->Operands[0].RegisterIndex];				
				_parameterStack.AddToEnd(objRegister);
				break;
			}

		// --------------------------------------------------------------------------------------------
		// Invokation.
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_INVK:		// invk register, parametercount
			{
				CScriptValue& funcRegister = context->Registers[instruction->Operands[0].RegisterIndex];					
				s32			  paramCount   = instruction->Operands[1].IntLiteral;	

				// Invoke native function.
				bool success = false;
				if (funcRegister.Type == SCRIPT_VALUE_TYPE_NATIVE_FUNCTION && funcRegister.NativeFunction != NULL)
				{
					_nativeFunctionParameterCount = paramCount;
					_nativeFunctionIdentifier = funcRegister.NativeFunction->Name;

					funcRegister.NativeFunction->FunctionPtr(this);
					
					for (s32 i = 0; i < paramCount; i++)
					{
						_parameterStack.RemoveFromEnd();
					}

					success = true;
				}

				// Invoke script function.
				else if (funcRegister.Type == SCRIPT_VALUE_TYPE_FUNCTION && funcRegister.Symbol != NULL)
				{
					Symbols::CScriptFunctionSymbol* funcSymbol = dynamic_cast<CScriptFunctionSymbol*>(funcRegister.Symbol);
					success = InvokeFunction(funcSymbol, paramCount);

					//if (funcSymbol->IsGenerator == true)
					//	funcRegister = context->Registers[SCRIPT_CONST_REGISTER_RETURN];
				}

				// Invoke object.
				else if (funcRegister.Type == SCRIPT_VALUE_TYPE_OBJECT && funcRegister.Object != NULL)
				{
					success = funcRegister.Object->Invoke(this, paramCount);

					for (s32 i = 0; i < paramCount; i++)
					{
						_parameterStack.RemoveFromEnd();
					}

				}
				else
				{
					success = false;
					Error(S("Attempt to invoke invalid data type '%s'.").Format(GetDataTypeName(funcRegister).c_str()));
				}

				break;
			}
		case SCRIPT_OPCODE_RET:			// ret		OR		ret ret_val_reg
			{				
				if (instruction->OperandCount == 1)
				{
					CScriptValue retVal = context->Registers[instruction->Operands[0].RegisterIndex];

					PopCallContext();

					// Set the return value on the next lower call stack.
					if (_callStack.Size() > 1)
					{
						_callStack[_callStack.Size() - 1].Registers[SCRIPT_CONST_REGISTER_RETURN] = retVal;
					}
				}
				else
				{
					if (_currentContext->GeneratorIterator != NULL)
					{
						_currentContext->GeneratorIterator->CallbackComplete();
					}
					PopCallContext();
				}

				break;
			}
			
		// --------------------------------------------------------------------------------------------
		// Generators.
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_YIELD:		// yield valuereg
			{
				CScriptValue retVal = context->Registers[instruction->Operands[0].RegisterIndex];
				
				if (_currentContext->GeneratorIterator != NULL)
				{
					_currentContext->GeneratorIterator->CallbackNext(*_currentContext, retVal);
				}

				PopCallContext();

				break;
			}

		// --------------------------------------------------------------------------------------------
		// Natives.
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_GETNATIVE:		// native valuereg
			{
				CScriptValue&				outReg		= context->Registers[instruction->Operands[0].RegisterIndex];	
				Engine::Containers::CString symbolName	= CoerceToString(outReg);

				// Look for function first.
				CScriptNativeFunction* func = _virtualMachine->FindNativeFunction(symbolName);
				if (func != NULL)
				{
					outReg.Type				= SCRIPT_VALUE_TYPE_NATIVE_FUNCTION;
					outReg.NativeFunction	= func;
				
					break;
				}
				
				// ERRROOOORZ.
				Error(S("Undefined native symbol '%s'.").Format(symbolName.c_str()), instruction);
			}
			
		// --------------------------------------------------------------------------------------------
		// State code.
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_SETSTATE:	// setstate symbol
			{
				ChangeState(reinterpret_cast<CScriptStateSymbol*>(instruction->Operands[0].Symbol));
				break;
			}
			
		// --------------------------------------------------------------------------------------------
		// Dependencies.
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_LOADMODULE:	// loadmodule output_reg, name
			{

				break;
			}

		// --------------------------------------------------------------------------------------------
		// Iteration.
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_ITERNEW:		// ITERNEW  context_reg, value_reg			:		Creates a iterator from value that can be iterated over.
			{
				CScriptValue& outputreg = context->Registers[instruction->Operands[0].RegisterIndex];	
				CScriptValue& valuereg = context->Registers[instruction->Operands[1].RegisterIndex];	
						
				// Check we have a valid object to iterate over.
				if (valuereg.Type != SCRIPT_VALUE_TYPE_OBJECT)
				{
					UniterableObject(valuereg);
					break;
				}

				// Create iterator based on type.
				CScriptIteratorObject* iterObj = valuereg.Object->CreateIterator(this);
				if (iterObj == NULL)
				{
					UniterableObject(valuereg);
					break;
				}

				// Return the iterator.
				outputreg.Type = SCRIPT_VALUE_TYPE_OBJECT;
				outputreg.Object = iterObj;

				break;
			}
			
		case SCRIPT_OPCODE_ITERDONE:		// ITERDONE output_reg, context_reg 
			{
				CScriptValue& outputreg = context->Registers[instruction->Operands[0].RegisterIndex];	
				CScriptValue& contextreg = context->Registers[instruction->Operands[1].RegisterIndex];	

				if (contextreg.Type != SCRIPT_VALUE_TYPE_OBJECT)
				{
					UniterableObject(contextreg);
					break;
				}

				CScriptIteratorObject* iterObj = dynamic_cast<CScriptIteratorObject*>(contextreg.Object);
				if (iterObj == NULL)
				{
					UniterableObject(contextreg);
					break;
				}

				outputreg.Type		= SCRIPT_VALUE_TYPE_INT;
				outputreg.IntValue	= (iterObj->IsFinished(this) ? 1 : 0);

				break;
			}
			
		case SCRIPT_OPCODE_ITERNEXT:		// ITERNEXT output_reg, context_reg
			{
				CScriptValue& outputreg = context->Registers[instruction->Operands[0].RegisterIndex];	
				CScriptValue& contextreg = context->Registers[instruction->Operands[1].RegisterIndex];	

				if (contextreg.Type != SCRIPT_VALUE_TYPE_OBJECT)
				{
					UniterableObject(contextreg);
					break;
				}

				CScriptIteratorObject* iterObj = dynamic_cast<CScriptIteratorObject*>(contextreg.Object);
				if (iterObj == NULL)
				{
					UniterableObject(contextreg);
					break;
				}

				outputreg = iterObj->NextValue(this);

				break;
			}
			
		// --------------------------------------------------------------------------------------------
		// Symbol type.
		// --------------------------------------------------------------------------------------------
		case SCRIPT_OPCODE_ISTYPE:		// istype output_reg, symbol
			{
				CScriptValue&				outReg		= context->Registers[instruction->Operands[0].RegisterIndex];	
				Engine::Containers::CString symbolName	= instruction->Operands[1].Symbol->GetIdentifier();

				symbolName = symbolName.ToLower();

				if (symbolName == "int")
				{
					outReg.IntValue	= (outReg.Type == SCRIPT_VALUE_TYPE_INT) ? 1 : 0;
					outReg.Type		= SCRIPT_VALUE_TYPE_INT;
				}
				else if (symbolName == "float")
				{
					outReg.IntValue   = (outReg.Type == SCRIPT_VALUE_TYPE_FLOAT) ? 1 : 0;
					outReg.Type		  = SCRIPT_VALUE_TYPE_INT;
				}
				else if (symbolName == "null" || (outReg.Type == SCRIPT_VALUE_TYPE_OBJECT && outReg.Object == NULL))
				{
					outReg.IntValue   = (outReg.Type == SCRIPT_VALUE_TYPE_NULL) ? 1 : 0;
					outReg.Type		  = SCRIPT_VALUE_TYPE_INT;
				}
				else if (symbolName == "symbol")
				{
					outReg.IntValue   = (outReg.Type == SCRIPT_VALUE_TYPE_SYMBOL) ? 1 : 0;
					outReg.Type		  = SCRIPT_VALUE_TYPE_INT;
				}
				else if (symbolName == "function")
				{
					outReg.IntValue   = (outReg.Type == SCRIPT_VALUE_TYPE_FUNCTION) ? 1 : 0;
					outReg.Type		  = SCRIPT_VALUE_TYPE_INT;
				}
				else if (symbolName == "object")
				{
					outReg.IntValue   = (outReg.Type == SCRIPT_VALUE_TYPE_OBJECT) ? 1 : 0;
					outReg.Type		  = SCRIPT_VALUE_TYPE_INT;
				}
				else if (outReg.Type == SCRIPT_VALUE_TYPE_OBJECT && outReg.Object != NULL)
				{
					outReg.IntValue   = (outReg.Object->GetName().ToLower() == symbolName || outReg.Object->DerivedFrom(this, symbolName));
					outReg.Type		  = SCRIPT_VALUE_TYPE_INT;
				}
				else
				{
					outReg.IntValue   = 0;
					outReg.Type		  = SCRIPT_VALUE_TYPE_INT;
				}

				break;
			}
			
		case SCRIPT_OPCODE_ASTYPE:		// astype output_reg, symbol
			{
				CScriptValue&				outReg		= context->Registers[instruction->Operands[0].RegisterIndex];	
				Engine::Containers::CString symbolName	= instruction->Operands[1].Symbol->GetIdentifier();

				symbolName = symbolName.ToLower();

				bool success = true;

				if (symbolName == "string")
				{
					CScriptStringObject* strObj	= Engine::Scripting::GetScriptAllocator()->NewObj<CScriptStringObject>(this, CoerceToString(outReg));
					GCAdd(strObj);

					outReg.Type		= SCRIPT_VALUE_TYPE_OBJECT;
					outReg.Object	= strObj;
				}
				else if (symbolName == "int")
				{
					outReg.Type		= SCRIPT_VALUE_TYPE_INT;
					outReg.IntValue	= CoerceToInt(outReg);
				}
				else if (symbolName == "float")
				{
					outReg.Type			= SCRIPT_VALUE_TYPE_FLOAT;
					outReg.FloatValue	= CoerceToFloat(outReg);
				}
				else if (outReg.Type == SCRIPT_VALUE_TYPE_OBJECT && outReg.Object != NULL)
				{
					if (outReg.Object->GetName().ToLower() != symbolName && !outReg.Object->DerivedFrom(this, symbolName))
					{
						success = outReg.Object->CastTo(this, outReg, symbolName);
					}
				}
				else
				{
					success = false;
				}

				// Not able to cast? D:
				if (success == false)
				{
					InvalidCast(outReg, symbolName, instruction);
				}		

				break;
			}
			
		// --------------------------------------------------------------------------------------------
		//  dafaq?
		// --------------------------------------------------------------------------------------------
		default:
			{
				Error(S("Encountered invalid opcode (0x%x)\n").Format(instruction->Opcode), instruction);
				break;
			}
	}

	// Continue running!
	return false;
}

void CScriptExecutionContext::Run(f32 timeslice)
{
	f32 timesliceRemaining = timeslice;
	bool finishedRun = false;

	if (_globalScopeRun == false)
		RunGlobalScope();
	
	if (_callStack.Size() <= 0)
		return;

	// Keep executing until we are done.
	while (!finishedRun)
	{
		// Execute instruction!
		f32 timer = (f32)Engine::Platform::GetMillisecs();
		for (u32 i = 0; i < SCRIPT_VM_TIMESLICE_CHECK_INTERVAL; i++)
		{
			bool ret = Execute();
			if (ret == true || _callStack.Size() == 0)
			{
				finishedRun = true;
				break;
			}
		}
		timesliceRemaining -= ((f32)Engine::Platform::GetMillisecs() - timer);

		// Timeslice finished?
		if (timeslice != 0 && timesliceRemaining < 0)
			finishedRun = true;
	}
}

void CScriptExecutionContext::RunGlobalScope()
{
	LOG_ASSERT(_globalScopeRun == false);

	// Push a call context for the global scope.
	CScriptCallContext context;
	context.LocalCount	= 0;
	context.PC			= 0; // Global scope always starts at instruction 0.
	PushCallContext(context);

	// Keep executing until we are complete.
	while (_callStack.Size() > 0)
		Execute();

	printf("Executed %i instructions.\n", _instructionsExecuted);

	_globalScopeRun = true;
}

bool CScriptExecutionContext::InvokeFunction(Symbols::CScriptFunctionSymbol* symbol, u32 paramCount)
{
	// Check the parameter count.
	if (symbol->ParameterCount != paramCount)
	{
		Error(S("Attempt to invoke function '%s' with incorrect parameter count '%i'.\n").Format(symbol->GetIdentifier().c_str(), paramCount));
		return false;
	}

	// Check the call context.
	CScriptCallContext context;
	context.Symbol		= symbol;
	context.PC			= symbol->EntryPoint;
	context.LocalCount	= symbol->LocalCount;
	context.Locals		= GetScriptAllocator()->AllocArray<CScriptValue>(symbol->LocalCount);

	// Null out locals and store parameters.
	for (u32 i = 0; i < symbol->LocalCount; i++)
	{
		context.Locals[i].Type = SCRIPT_VALUE_TYPE_NULL;
		if (i < paramCount)
		{
			context.Locals[i] = _parameterStack[i];

			if (context.Locals[i].Type == SCRIPT_VALUE_TYPE_OBJECT && context.Locals[i].Object != NULL)
				context.Locals[i].Object->_refCount++;
		}
	}

	// Null out registers.
	for (u32 i = 0; i < SCRIPT_TOTAL_REGISTERS; i++)
	{
		context.Registers[i].Type = SCRIPT_VALUE_TYPE_NULL;
	}

	// Remove parameters.
	for (u32 i = 0; i < paramCount; i++)
	{
		_parameterStack.RemoveFromEnd();
	}

	// If this is a generator set the return value to this context
	// otherwise push the call context.
	if (symbol->Type == AST::SCRIPT_FUNCTION_GENERATOR)
	{
		CScriptContextObject* ctxObj = Engine::Scripting::GetScriptAllocator()->NewObj<CScriptContextObject>(this, context);
		GCAdd(ctxObj);

		_currentContext->Registers[SCRIPT_CONST_REGISTER_RETURN].Type   = SCRIPT_VALUE_TYPE_OBJECT;
		_currentContext->Registers[SCRIPT_CONST_REGISTER_RETURN].Object = ctxObj;
	}
	else
	{
		PushCallContext(context);
	}

	return true;
}

void CScriptExecutionContext::PushCallContext(CScriptCallContext& context)
{
	_callStack.AddToEnd(context);
	_currentContext = &_callStack[_callStack.Size() - 1];
}

void CScriptExecutionContext::PopCallContext()
{
	CScriptCallContext context = _callStack.RemoveFromEnd();

	if (context.GeneratorIterator == NULL)
		context.Dispose();

	// Set new context.
	if (_callStack.Size() <= 0)
		_currentContext = NULL;
	else	
		_currentContext = &_callStack[_callStack.Size() - 1];
}

void CScriptExecutionContext::Error(const Engine::Containers::CString& str, Instructions::CScriptInstruction* instruction)
{
	// If instruction is null, use current instruction.
	if (instruction == NULL)
	{
		if (_callStack.Size() > 0)
		{
			CScriptCallContext* context = &_callStack[_callStack.Size() - 1];
			if (context->PC - 1 > 0)
			{
				instruction =  _instructions[context->PC - 1];
			}
		}
	}

	// Write out the call stack.
	u32							lineIndex = 1;
	Engine::Containers::CString line	  = "";

	// Reload the raw source code if its available.
	if (_context->_rawSource == "")
		_context->ReloadRawSource();

	// Get the line this error is on.
	for (u32 offset = 0; offset < _context->_rawSource.Length(); offset++)
	{
		u8 chr = _context->_rawSource[offset];
		if (chr == '\n')
		{
			if (lineIndex == instruction->Token.Line)
			{
				line = line.Trim();
				break;
			}
			else
			{
				lineIndex++;
				line = "";
			}
		}
		else
		{
			line += chr;
		}
	}

	if (line == "")
	{
		line = "<no source-code available>";
	}

	// Calculate the formatted message.
	// Should be in this format;
	//
	// File.cpp(0:0): Error: Unable to find function 'x'
	// _ctest = func(123, x());
	//					  ^	
	Engine::Containers::CString msg = "";
	msg += S(_context->_initialFile) + "(" + instruction->Token.Line + ":" + instruction->Token.Column + "): Error: ";
	msg += str + "\n";
	msg += line + "\n";

	if (instruction->Token.Column > 1)
		msg += Engine::Containers::CString(' ', instruction->Token.Column - 1);

	msg += "^\n";

	// Emit the error.
	printf(msg.Replace("%", "%%").c_str());

	// Break the compiler!
	Engine::Platform::DebugBreak();
}

void CScriptExecutionContext::InvalidOp(const Engine::Containers::CString& op, const CScriptValue& value, Instructions::CScriptInstruction* instruction)
{
	Engine::Containers::CString k = GetDataTypeName(value);
	Error(S("Attempt to perform '%s' operator on unsupported data type '%s'.").Format(op.c_str(), k.c_str()), instruction);
}

void CScriptExecutionContext::InvalidIndex(const CScriptValue& obj, s32 index, Instructions::CScriptInstruction* instruction)
{
	Engine::Containers::CString v = GetDataTypeName(obj);
	Error(S("Attempt to access invalid index '%i' of object '%s'.").Format(index, v.c_str()), instruction);
}

void CScriptExecutionContext::InvalidIndex(const CScriptValue& obj, const CScriptValue& key, Instructions::CScriptInstruction* instruction)
{
	Engine::Containers::CString k = CoerceToString(key);
	Engine::Containers::CString v = GetDataTypeName(obj);
	Engine::Containers::CString str = S("Attempt to access invalid index '%s' of object '%s'.").Format(k.c_str(), v.c_str());
	Error(str, instruction);
}

void CScriptExecutionContext::ImmutableError(const CScriptValue& obj, Instructions::CScriptInstruction* instruction)
{
	Engine::Containers::CString v = GetDataTypeName(obj);
	Error(S("Attempt modify immutable object '%s'.").Format(v.c_str()), instruction);
}

void CScriptExecutionContext::ImmutableError(const Engine::Containers::CString& str, Instructions::CScriptInstruction* instruction)
{
	Error(S("Attempt modify immutable object '%s'.").Format(str.c_str()), instruction);
}

void CScriptExecutionContext::DuplicateIndex(const CScriptValue& obj, const CScriptValue& key, Instructions::CScriptInstruction* instruction)
{
	Engine::Containers::CString k = GetDataTypeName(key);
	Engine::Containers::CString v = GetDataTypeName(obj);
	Error(S("Duplicate index '%s' in object '%s'.").Format(k.c_str(), v.c_str()), instruction);
}

void CScriptExecutionContext::InvalidCast(const CScriptValue& obj, const Engine::Containers::CString& type, Instructions::CScriptInstruction* instruction)
{
	Engine::Containers::CString v = GetDataTypeName(obj);
	Error(S("Invalid cast from '%s' to '%s'.").Format(v.c_str(), type.c_str()), instruction);
}

void CScriptExecutionContext::InvalidParameterCount(u32 expectedParamCount, Instructions::CScriptInstruction* instruction)
{
	Error(S("Attempt to call function '%s' with invalid parameter count '%i', expecting '%i' parameters.").Format(_nativeFunctionIdentifier.c_str(), _nativeFunctionParameterCount, expectedParamCount), instruction);
}

void CScriptExecutionContext::UniterableObject(const CScriptValue& obj, Instructions::CScriptInstruction* instruction)
{
	Engine::Containers::CString v = GetDataTypeName(obj);
	Error(S("Attempt to iterate over uniterable object '%s'.").Format(v.c_str()), instruction);
}

void CScriptExecutionContext::AssignTo(CScriptValue& dest, CScriptValue& val, bool binary)
{
	// Assign the new value.
	if (binary == true)
	{
		dest = val;
	}
	else
	{
		// Decrease reference of old object.
		if (dest.Type == SCRIPT_VALUE_TYPE_OBJECT && dest.Object != NULL)
		{
			dest.Object->_refCount--;
			//dest.Object->DecRef();

			_gcObjectPoolDirty[dest.Object->_generation] = true;
		}

		if (val.Type == SCRIPT_VALUE_TYPE_OBJECT && val.Object != NULL)
		{
			if (!val.Object->Assign(this, dest))
				InvalidOp("=", val);
		}
		else
		{
			dest = val;
		}

		// Increase reference of new object.
		if (dest.Type == SCRIPT_VALUE_TYPE_OBJECT && dest.Object != NULL)
		{
			//dest.Object->IncRef();
			dest.Object->_refCount++;
		}
	}
}

Engine::Containers::CString CScriptExecutionContext::GetDataTypeName(const CScriptValue& value)
{
	if (value.Type == SCRIPT_VALUE_TYPE_OBJECT)
		return value.Object->GetName();
	else
		return ScriptValueType_String[value.Type];
}

s32 CScriptExecutionContext::CompareValues(CScriptValue& src, CScriptValue& dest)
{
	ImplicitCast(src, dest);

	switch (src.Type)
	{
		case SCRIPT_VALUE_TYPE_INT:			return (s32)(src.IntValue - dest.IntValue);
		case SCRIPT_VALUE_TYPE_FLOAT:		return (s32)(src.FloatValue - dest.FloatValue);
		case SCRIPT_VALUE_TYPE_FUNCTION:	return src.IntValue == dest.IntValue ? 0 : 1; 
		case SCRIPT_VALUE_TYPE_SYMBOL:		return src.Symbol == dest.Symbol ? 0 : 1;
		case SCRIPT_VALUE_TYPE_NULL:		return src.Type != dest.Type ? 1 : 0; // To check for null, we just check both values are null XD, nice and easy.
		case SCRIPT_VALUE_TYPE_OBJECT:		
			{
				s32 res = 1;
				src.Object->Cmp(this, dest, res);
				return res;
			}
		default:							return 1;	// Automatically assume not equal.
	}

	return 0;
}

// This will take in 2 types, and balance them depending on 
// their cast priority.
void CScriptExecutionContext::ImplicitCast(CScriptValue& src, CScriptValue& dest)
{
	// Values the same, no balancing neccessary.
	if (dest.Type == src.Type)
	{
		if (dest.Type == SCRIPT_VALUE_TYPE_OBJECT)
		{
			if (typeid(*dest.Object) == typeid(*src.Object))
			{
				return;
			}
		}
		else
		{
			return; // Both the same primitive type, ignore.
		}
	}

	// --------------------------------------------------------------
	// Strings take priority over everything.
	// --------------------------------------------------------------
	if ((dest.Type == SCRIPT_VALUE_TYPE_OBJECT && typeid(*dest.Object) == typeid(CScriptStringObject)) || 
		(src.Type == SCRIPT_VALUE_TYPE_OBJECT  && typeid(*src.Object) == typeid(CScriptStringObject))) 
	{
		if (dest.Type == SCRIPT_VALUE_TYPE_OBJECT && typeid(*dest.Object) == typeid(CScriptStringObject))
		{
			CScriptStringObject* strObj	= Engine::Scripting::GetScriptAllocator()->NewObj<CScriptStringObject>(this, CoerceToString(src));
			GCAdd(strObj);

			src.Object = strObj;
			src.Type = SCRIPT_VALUE_TYPE_OBJECT;
		}
		else
		{
			CScriptStringObject* strObj	= Engine::Scripting::GetScriptAllocator()->NewObj<CScriptStringObject>(this, CoerceToString(dest));
			GCAdd(strObj);

			dest.Object = strObj;
			dest.Type = SCRIPT_VALUE_TYPE_OBJECT;
		}
	}

	// --------------------------------------------------------------
	// Floats take next priority level.	
	// --------------------------------------------------------------
	if (dest.Type == SCRIPT_VALUE_TYPE_FLOAT || src.Type == SCRIPT_VALUE_TYPE_FLOAT)
	{
		if (dest.Type == SCRIPT_VALUE_TYPE_FLOAT)
		{
			src.FloatValue = CoerceToFloat(src);
			src.Type = SCRIPT_VALUE_TYPE_FLOAT;
		}
		else
		{
			dest.FloatValue = CoerceToFloat(dest);
			dest.Type = SCRIPT_VALUE_TYPE_FLOAT;
		}
	}

	// --------------------------------------------------------------
	// Ints are ignored, its assumed they will be implicitly casted
	// by the above operations. At this point we can return as we
	// assume we have done all the conversions neccessary for the op.
	// --------------------------------------------------------------
//	Error(S("Attempt to implicitly cast incompatible types, '%s' and '%s'.").Format(GetDataTypeName(src), GetDataTypeName(dest)));	
}

s32 CScriptExecutionContext::CoerceToInt(const CScriptValue& value)
{
	switch (value.Type)
	{
		case SCRIPT_VALUE_TYPE_INT:			return (s32)(value.IntValue);
		case SCRIPT_VALUE_TYPE_FLOAT:		return (s32)(value.FloatValue);
		case SCRIPT_VALUE_TYPE_OBJECT:
			{
				s32 result = 0;
				if (value.Object->CoerceToInt(this, result))
					break;
			}
		default:							InvalidOp("cast to int", value);
	}
	return 0;
}

f32 CScriptExecutionContext::CoerceToFloat(const CScriptValue& value)
{
	switch (value.Type)
	{
		case SCRIPT_VALUE_TYPE_INT:			return (f32)(value.IntValue);
		case SCRIPT_VALUE_TYPE_FLOAT:		return (f32)(value.FloatValue);
		case SCRIPT_VALUE_TYPE_OBJECT:
			{
				f32 result = 0;
				if (value.Object->CoerceToFloat(this, result))
					break;
			}
		default:							InvalidOp("cast to float", value);
	}
	return 0;
}

Engine::Containers::CString CScriptExecutionContext::CoerceToString(const CScriptValue& value)
{
	switch (value.Type)
	{
		case SCRIPT_VALUE_TYPE_INT:			return Engine::Containers::CString(value.IntValue);
		case SCRIPT_VALUE_TYPE_FLOAT:		return Engine::Containers::CString(value.FloatValue);
		case SCRIPT_VALUE_TYPE_OBJECT:
			{
				Engine::Containers::CString result = "";
				if (value.Object->CoerceToString(this, result))
					return result;
			}
		default:							InvalidOp("cast to string", value);
	}
	return "";
}

CScriptObject* CScriptExecutionContext::CoerceToObject(const CScriptValue& value)
{
	switch (value.Type)
	{
		case SCRIPT_VALUE_TYPE_OBJECT:		return value.Object;
		default:							InvalidOp("cast to object", value);
	}
	return NULL;
}

s32 CScriptExecutionContext::CoerceValueToInt(const CScriptValue& value)
{
	return CoerceToInt(value);
}

f32 CScriptExecutionContext::CoerceValueToFloat(const CScriptValue& value)
{
	return CoerceToFloat(value);
}

Engine::Containers::CString CScriptExecutionContext::CoerceValueToString(const CScriptValue& value)
{
	return CoerceToString(value);
}

CScriptObject* CScriptExecutionContext::CoerceValueToObject(const CScriptValue& value)
{
	return CoerceToObject(value);
}

s32 CScriptExecutionContext::CompareScriptValues(CScriptValue& src, CScriptValue& dest)
{
	return CompareValues(src, dest);
}

void CScriptExecutionContext::GCAdd(CScriptObject* object, u32 generation)
{
	CScriptObject* next = _gcObjectPool[generation];

	_gcObjectPool[generation] = object;
	_gcObjectPoolDirty[generation] = true;
	object->_prev = NULL;
	object->_next = next;
	object->_generation = generation;

	if (next != NULL)
		next->_prev = object;

	if (generation == 0)
		object->_allocCallDepth = _callStack.Size();
}

void CScriptExecutionContext::GCRemove(CScriptObject* object)
{
	if (object->_prev != NULL)
		object->_prev->_next = object->_next;
	if (object->_next != NULL)
		object->_next->_prev = object->_prev;

	if (object == _gcObjectPool[object->_generation])
	{
		_gcObjectPool[object->_generation] = object->_next;
	}
}

void CScriptExecutionContext::GCExecute()
{
	//f32 timer = Engine::Platform::GetMillisecs();

	// IcarusScript uses a simple generational GC.
	//
	// The main aim of the GC is to provide automatic garbage collection without
	// any recognisable delay to the script programmer. The GC in IScript will always
	// prioritize no-delays over memory efficiency. If it has a large amount of objects to collect
	// it will prefer to do it slowly over several frames rather than at once and potentially 
	// "lag" the script.
	//
	// Each generation is collected every SCRIPT_VM_GC_INTERVAL * (10 pow generation)
	// Generations are only collected if their dirty flag is set (which is set when new objects are added, 
	// or referenced counts are decremented).
	//
	// When a collection is performed any objects that are not in any registers in the callstack and
	// have zero references are disposed off.
	//
	// If an object survives a collection it moves up to the next highest generation.
	//
	// The garbage collection is incremental, only a certain amount of objects will be checked per run
	// this keeps the gc "lag" down to a minimum (pretty much required for interactive games!).
	//
	// Downsides:
	//	- Cyclic references are not dealt with at all. Avoid!
	//	- If a lot of items are allocated at the same time the memory will take a while to be collected
	//	  due to the incremental GC. Better than delaying the game logic though!
	//	- As registers can hold onto references of objects until they are overwriten, objects may
	//	  stay alive for longer than they actually need to. This shouldn't be to much of an issue
	//	  though as its likely the registers will get overwritten soon enough.
	
	// TODO: Make garbage collection incremental.

	// Which generation will we run.
	u32 generation = 0;
	for (u32 i = 1; i < SCRIPT_MAX_GC_GENERATIONS; i++)
	{
		u32 interval = SCRIPT_VM_GC_INTERVAL * Math::Pow(10, i);
		if ((_instructionsExecuted % interval) == 0)
			generation = i;
	}

	// Run the generation.
	GCCollectGeneration(generation);
	
	//f32 elapsed = Engine::Platform::GetMillisecs() - timer;
	//printf("EXECUTED GEN %i IN %f\n", generation, elapsed);

//	if (generation == 1)
//	{
//		printf("GC State:\n");
//		for (u32 i = 0; i < SCRIPT_MAX_GC_GENERATIONS; i++)
//		{
//			CScriptObject* obj = _gcObjectPool[i];
//			u32 size = 0;
//			while (obj != NULL)
//			{
//				obj = obj->_next;
//				size++;
//			}
//			printf("\t[%i] %i\n", i, size);
//		}
//	}
}

void CScriptExecutionContext::GCCollectGeneration(u32 generation)
{
	// Don't run if this generation is not dirty.
	if (_gcObjectPoolDirty[generation] == false)
	{
		return;
	}
	_gcObjectPoolDirty[generation] = false;

	CScriptObject* obj = _gcObjectPool[generation];
	u32 count = 0;
	while (obj != NULL)
	{
		bool referenced	= false;
		s32	 refs		= obj->_refCount;

		if (refs <= 0)
		{
			// Oh god wtf. Why is the reference count below zero? Something
			// is seriously seriously wrong D:
			LOG_ASSERT(refs >= 0);

			// Go through the call stack and check its not in any registers currently.
			for (u32 depth = obj->_allocCallDepth - 1; depth < _callStack.Size() && referenced == false; depth++)
			{
				CScriptCallContext& context = _callStack[depth];
				for (u32 reg = 0; reg < SCRIPT_TOTAL_REGISTERS; reg++)
				{
					if (context.Registers[reg].Type == SCRIPT_VALUE_TYPE_OBJECT &&
						context.Registers[reg].Object == obj)
					{
						referenced = true;
						break;
					}
				}
			}
		}
		else
		{
			referenced = true;
		}
		
		// Keep a reference to the next node incase we remove this one.
		CScriptObject* next = obj->_next;

		// Move the object up a generation if its still referenced.
		if (referenced == true)
		{
			if (generation < SCRIPT_MAX_GC_GENERATIONS - 1)
			{
				// Remove from old pool.
				GCRemove(obj);	
				GCAdd(obj, generation + 1);
			}
		}

		// Otherwise dispose.
		else
		{		
			// Remove from old pool.
			GCRemove(obj);

			// Finalize the object.
			obj->Finalize(this);

			// I'm FREEEEEEEEEEEEEEEEEEEEEE.
			GetScriptAllocator()->FreeObj(&obj);
		}

		// Proceed to next node.
		obj = next;

		count++;
	}

}

u32 CScriptExecutionContext::GetParameterCount()
{
	return _nativeFunctionParameterCount;
}
			
CScriptValue CScriptExecutionContext::GetParameter(u32 index)
{
	u32 firstParamIndex = _parameterStack.Size() - _nativeFunctionParameterCount;
	return _parameterStack[firstParamIndex + index];
}

s32 CScriptExecutionContext::GetIntParameter(u32 index)
{
	return CoerceToInt(GetParameter(index));
}

f32 CScriptExecutionContext::GetFloatParameter(u32 index)
{
	return CoerceToFloat(GetParameter(index));
}

Engine::Containers::CString	CScriptExecutionContext::GetStringParameter(u32 index)
{
	return CoerceToString(GetParameter(index));
}

CScriptObject* CScriptExecutionContext::GetObjectParameter(u32 index)
{
	return CoerceToObject(GetParameter(index));
}
			
CScriptValue CScriptExecutionContext::GetReturnValue()
{
	return _currentContext->Registers[SCRIPT_CONST_REGISTER_RETURN];
}

s32	CScriptExecutionContext::GetReturnIntValue()
{
	return CoerceToInt(GetReturnValue());
}

f32	CScriptExecutionContext::GetReturnFloatValue()
{
	return CoerceToFloat(GetReturnValue());
}

Engine::Containers::CString	CScriptExecutionContext::GetReturnStringValue()
{
	return CoerceToString(GetReturnValue());
}

CScriptObject* CScriptExecutionContext::GetReturnObjectValue()
{
	return CoerceToObject(GetReturnValue());
}

void CScriptExecutionContext::SetReturnValue(const CScriptValue& param)
{
	_currentContext->Registers[SCRIPT_CONST_REGISTER_RETURN] = param;
}

void CScriptExecutionContext::SetReturnIntValue(s32 param)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_INT;
	v.IntValue = param;
	SetReturnValue(v);
}

void CScriptExecutionContext::SetReturnFloatValue(f32 param)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_FLOAT;
	v.FloatValue = param;
	SetReturnValue(v);
}

void CScriptExecutionContext::SetReturnStringValue(const Engine::Containers::CString& str)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_OBJECT;
	v.Object = GetScriptAllocator()->NewObj<CScriptStringObject>(this, str);
	GCAdd(v.Object);

	SetReturnValue(v);
}

void CScriptExecutionContext::SetReturnObjectValue(CScriptObject* obj)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_OBJECT;
	v.Object = obj;
	SetReturnValue(v);
}

// For native->script calling.
void CScriptExecutionContext::PassStringParameter(const Engine::Containers::CString& param)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_OBJECT;
	v.Object = GetScriptAllocator()->NewObj<CScriptStringObject>(this, param);
	GCAdd(v.Object);

	PassParameter(v);
}

void CScriptExecutionContext::PassIntParameter(s32 param)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_INT;
	v.IntValue = param;
	PassParameter(v);
}

void CScriptExecutionContext::PassFloatParameter(f32 param)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_FLOAT;
	v.FloatValue = param;
	PassParameter(v);
}

void CScriptExecutionContext::PassObjectParameter(CScriptObject* obj)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_OBJECT;
	v.Object = obj;
	PassParameter(v);
}

void CScriptExecutionContext::PassParameter(const CScriptValue& param)
{
	_parameterStack.AddToEnd(param);
}

bool CScriptExecutionContext::CallFunction(const Engine::Containers::CString& name, u32 parameterCount)
{
	Symbols::CScriptFunctionSymbol* func = GetFunctionSymbol(name);
	if (func == NULL)
		return false;

	return CallFunction(func, parameterCount);
}

bool CScriptExecutionContext::CallFunction(Symbols::CScriptFunctionSymbol* symbol, u32 parameterCount)
{
	InvokeFunction(symbol, parameterCount);
	return true;
}

bool CScriptExecutionContext::CallEvent(const Engine::Containers::CString& name, u32 parameterCount, bool async, bool stackable)
{
	Symbols::CScriptFunctionSymbol* func = GetFunctionSymbol(name);
	if (func == NULL)
		return false;

	return CallEvent(func, parameterCount, async, stackable);
}

bool CScriptExecutionContext::CallEvent(Symbols::CScriptFunctionSymbol* symbol, u32 parameterCount, bool async, bool stackable)
{
	// Check symbol is not already on the call stack if its not stackable.
	if (stackable == false)
	{
		for (u32 i = 0; i < _callStack.Size(); i++)
			if (_callStack[i].Symbol == symbol)
				return false;
	}

	// Invoke the function.
	u32 call_stack_depth = _callStack.Size();
	if (!InvokeFunction(symbol, parameterCount))
		return false;

	// Run until this function is off the stack.
	if (async == true)
	{
		while (_callStack.Size() > call_stack_depth)
			Execute();
	}

	return true;
}

Symbols::CScriptFunctionSymbol*	CScriptExecutionContext::GetFunctionSymbol(const Engine::Containers::CString& name)
{
	u32 lowerNameHash = name.ToLower().ToHashCode();

	// If state exists, look in there for info.
	if (_state != NULL)
	{
		u32 lowerStateNameHash = _state->GetIdentifier().ToLower().ToHashCode();

		// Rebuild the states symbol hash table.
		if (!_stateScopeHashTable.Contains(lowerStateNameHash))
			RebuildStateScopeSymbolHashTable(_state);

		// Look in hash table.
		Engine::Containers::CHashTable<CScriptSymbol*>* hashTable = _stateScopeHashTable[lowerStateNameHash];
		CScriptSymbol* symbol = (*hashTable)[lowerNameHash];
		if (symbol != NULL && symbol->GetType() == SCRIPT_SYMBOL_TYPE_FUNCTION)
		{
			return reinterpret_cast<Symbols::CScriptFunctionSymbol*>(symbol);
		}
	}

	// If its not already been done then hash the symbols.
	if (!_globalScopeSymbolHashTableCreated)
		RebuildGlobalScopeSymbolHashTable();

	// Look in global scope hash table.
	CScriptSymbol* symbol = _globalScopeHashTable[lowerNameHash];
	if (symbol != NULL && symbol->GetType() == SCRIPT_SYMBOL_TYPE_FUNCTION)
	{
		return reinterpret_cast<Symbols::CScriptFunctionSymbol*>(symbol);
	}

	return NULL;

}

Symbols::CScriptVariableSymbol*	CScriptExecutionContext::GetVariableSymbol(const Engine::Containers::CString& name)
{	
	u32 lowerNameHash = name.ToLower().ToHashCode();

	// If its not already been done then hash the symbols.
	if (!_globalScopeSymbolHashTableCreated)
		RebuildGlobalScopeSymbolHashTable();

	// Look in global scope hash table.
	CScriptSymbol* symbol = _globalScopeHashTable[lowerNameHash];
	if (symbol != NULL && symbol->GetType() == SCRIPT_SYMBOL_TYPE_VARIABLE)
	{
		return reinterpret_cast<Symbols::CScriptVariableSymbol*>(symbol);
	}

	return NULL;
}

void CScriptExecutionContext::RebuildGlobalScopeSymbolHashTable()
{
	for (u32 i = 0; i < _context->_symbols.Size(); i++)
	{
		CScriptSymbol* sym = _context->_symbols[i];
		u32 lowerNameHash = sym->GetIdentifier().ToLower().ToHashCode();

		if (sym->GetType() == SCRIPT_SYMBOL_TYPE_FUNCTION)
		{
			CScriptFunctionSymbol* funcSym = reinterpret_cast<CScriptFunctionSymbol*>(sym);
			if (funcSym->State == NULL)
			{
				_globalScopeHashTable.Insert(lowerNameHash, sym);
			}
		}
		else if (sym->GetType() == SCRIPT_SYMBOL_TYPE_VARIABLE)
		{
			CScriptVariableSymbol* varSym = reinterpret_cast<CScriptVariableSymbol*>(sym);
			if (varSym->IsGlobal == true)
			{
				_globalScopeHashTable.Insert(lowerNameHash, sym);
			}
		}
	}

	_globalScopeSymbolHashTableCreated = true;
}

void CScriptExecutionContext::RebuildStateScopeSymbolHashTable(Symbols::CScriptStateSymbol* symbol)
{
	u32 lowerStateNameHash = symbol->GetIdentifier().ToLower().ToHashCode();

	// Allocate new state hash table and put it in the main list.
	Engine::Containers::CHashTable<CScriptSymbol*>* arr = GetScriptAllocator()->NewObj<Engine::Containers::CHashTable<CScriptSymbol*>>();

	// Start adding symbols to the list.
	for (u32 i = 0; i < _context->_symbols.Size(); i++)
	{
		CScriptSymbol* sym = _context->_symbols[i];
		u32 lowerNameHash = sym->GetIdentifier().ToLower().ToHashCode();

		if (sym->GetType() == SCRIPT_SYMBOL_TYPE_FUNCTION)
		{
			CScriptFunctionSymbol* funcSym = reinterpret_cast<CScriptFunctionSymbol*>(sym);
			if (funcSym->State == symbol)
			{
				arr->Insert(lowerNameHash, sym);
			}
		}
	}

	_stateScopeHashTable.Insert(lowerStateNameHash, arr);
}

void CScriptExecutionContext::ChangeState(Symbols::CScriptStateSymbol* symbol)
{
	_state = symbol;
}

/*
Engine::Containers::CString	CScriptExecutionContext::GetGlobalStringVariable(const Engine::Containers::CString& name)
{
	return CoerceToString(GetGlobalVariable(name));
}

s32 CScriptExecutionContext::GetGlobalIntVariable(const Engine::Containers::CString& name)
{
	return CoerceToInt(GetGlobalVariable(name));
}

f32	CScriptExecutionContext::GetGlobalFloatVariable(const Engine::Containers::CString& name)
{
	return CoerceToFloat(GetGlobalVariable(name));
}

CScriptObject* CScriptExecutionContext::GetGlobalObjectVariable(const Engine::Containers::CString& name)
{
	return CoerceToObject(GetGlobalVariable(name));
}

CScriptValue CScriptExecutionContext::GetGlobalVariable(const Engine::Containers::CString& name)
{

}

void CScriptExecutionContext::SetGlobalStringVariable(const Engine::Containers::CString& name, const Engine::Containers::CString& val)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_OBJECT;
	v.Object = GetScriptAllocator()->NewObj<CScriptStringObject>(this, val);
	GCAdd(v.Object);

	SetGlobalVariable(name, v);
}

void CScriptExecutionContext::SetGlobalIntVariable(const Engine::Containers::CString& name, s32 param)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_INT;
	v.IntValue = param;

	SetGlobalVariable(name, v);
}

void CScriptExecutionContext::SetGlobalFloatVariable(const Engine::Containers::CString& name, f32 param)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_FLOAT;
	v.FloatValue = param;

	SetGlobalVariable(name, v);
}

void CScriptExecutionContext::SetGlobalObjectVariable(const Engine::Containers::CString& name, CScriptObject* obj)
{
	CScriptValue v;
	v.Type = SCRIPT_VALUE_TYPE_OBJECT;
	v.Object = obj;

	SetGlobalVariable(name, v);
}

void CScriptExecutionContext::SetGlobalVariable(const Engine::Containers::CString& name, const CScriptValue& param)
{
}
*/

// CScriptCallCOntext -----------------------------------------------------

void CScriptCallContext::Dispose()
{
	// Reduce ref-count of all objects.
	if (Symbol != NULL && Locals != NULL)
	{
		for (u32 i = 0; i < Symbol->LocalCount; i++)
		{
			if (Locals[i].Type == SCRIPT_VALUE_TYPE_OBJECT && Locals[i].Object != NULL)
				Locals[i].Object->_refCount--;
		}
	}

	// Deallocate locals memory.
	if (Locals != NULL)
	{	
		GetScriptAllocator()->FreeArray(&Locals);
	}
}

// CScriptVirtualMachine -----------------------------------------------------

void CScriptVirtualMachine::LoadNativeLibrary()
{
	Engine::Scripting::Native::Glue::RegisterScriptFunctions(this);
}

CScriptVirtualMachine::CScriptVirtualMachine()
{
}

CScriptVirtualMachine::~CScriptVirtualMachine()
{
	Engine::Memory::Allocators::CAllocator* alloc = GetScriptAllocator();
	if (alloc == NULL)
		return;

	for (u32 i = 0; i < _nativeFunctions.Size(); i++)
	{
		CScriptNativeFunction* func = _nativeFunctions.AtIndex(i)->Value;
		alloc->FreeObj(&func);
	}
}

void CScriptVirtualMachine::Run(f32 timeslice)
{
	f32 timeLeft = timeslice;
	while (timeLeft >= 0)
	{
		f32 contextTimeslice = _contexts.Size() / timeLeft;
		f32 timer = (f32)Engine::Platform::GetMillisecs();

		for (u32 i = 0; i < _contexts.Size(); i++)
		{
			_contexts[i]->Run(timeslice == 0 ? 0 : contextTimeslice);
		}

		timeLeft -= (f32)(Engine::Platform::GetMillisecs() - timer);
	}
}

void CScriptVirtualMachine::AddContext(CScriptExecutionContext* context)
{
	_contexts.AddToEnd(context);
	context->_virtualMachine = this;
}

void CScriptVirtualMachine::RemoveContext(CScriptExecutionContext* context)
{
	_contexts.Remove(context);
	context->_virtualMachine = NULL;
}

const Engine::Containers::CArray<CScriptExecutionContext*>&	CScriptVirtualMachine::GetContexts()
{
	return _contexts;
}

void CScriptVirtualMachine::RegisterNativeFunction(const u8* name, ScriptNativeFunctionPrototype funcPtr)
{
	CScriptNativeFunction* func = GetScriptAllocator()->NewObj<CScriptNativeFunction>(name, funcPtr);
	_nativeFunctions.Insert(S(name).ToLower().ToHashCode(), func);
}

CScriptNativeFunction* CScriptVirtualMachine::FindNativeFunction(const Engine::Containers::CString& name)
{
	u32 hash = name.ToLower().ToHashCode();
	if (!_nativeFunctions.Contains(hash))
		return NULL;
	return _nativeFunctions[hash];
}


// CScriptNativeFunction -----------------------------------------------------

CScriptNativeFunction::CScriptNativeFunction(const u8* name, ScriptNativeFunctionPrototype funcPtr)
{
	Name = name;
	FunctionPtr = funcPtr;
}
