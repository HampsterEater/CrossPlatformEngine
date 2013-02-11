///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptGenerator.h"
#include "CScriptCompileContext.h"
#include "CScriptASTNode.h"
#include "CScriptSymbol.h"
#include "CScriptJumpTargetSymbol.h"
#include "CScriptFunctionASTNode.h"
#include "CScriptStateASTNode.h"
#include "CScriptFunctionSymbol.h"
#include "CScriptManager.h"
#include "CArray.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;
using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Instructions;

CScriptGenerator::CScriptGenerator()
{
	for (u32 i = SCRIPT_MIN_GEN_PURPOSE_REGISTER; i <= SCRIPT_MAX_GEN_PURPOSE_REGISTER; i++)
	{
		_registersAllocated[i] = false;
	}
}

CScriptGenerator::~CScriptGenerator()
{
}

CScriptCompileContext* CScriptGenerator::GetContext()
{
	return _context;
}

bool CScriptGenerator::Analyze(CScriptCompileContext* context)
{
	_context = context;

	try
	{
		// Walk through the AST and generate symbols.
		_context->_astTree->GenerateSymbols(this);

		// Generate AST for global scope (this needs to be at the start of the instruction list, as
		// variable declarations and such can be intermixed between functions, which would normally
		// cause the instructions to be intermixed as well -> which means global scope won't execute
		// correctly :(). Basically thing of the global scope as a seperate function that just happens
		// to have a bunch of seperate functions declared inside it.
		for (u32 i = 0; i < _context->_astTree->_children.Size(); i++)
		{
			CScriptASTNode* child = _context->_astTree->_children[i];
			if ((dynamic_cast<CScriptFunctionASTNode*>(child)	== NULL || dynamic_cast<CScriptFunctionASTNode*>(child)->Assigned == true) &&
				 dynamic_cast<CScriptStateASTNode*>(child)		== NULL)
			{
				child->GenerateInstructions(this);
			}
		}

		// Add a return code to the global scope.
		Instructions::CScriptInstruction* instr = GetScriptAllocator()->NewObj<Instructions::CScriptInstruction>();
		instr->Opcode		= Instructions::SCRIPT_OPCODE_RET;
		instr->OperandCount = 0;
		_context->_instructions.AddToEnd(instr);

		// Generate AST for non-global scope.
		GenerateNonGlobalScope(_context->_astTree);

		// Create our final symbol list.
		GenerateSymbolList(_context->_astTree);

		// Patch all references to instruction jump targets
		// to actual instruction indexes.
		//PatchJumpTargets();
	}
	catch (...)
	{

	}
	
	Disassemble();

	_context->_saveable = (context->GetErrorCount(SCRIPT_ERROR_FATAL) <= 0);

	return (context->GetErrorCount(SCRIPT_ERROR_FATAL) <= 0);
}

void CScriptGenerator::GenerateNonGlobalScope(CScriptASTNode* root)
{
	for (u32 i = 0; i < root->_children.Size(); i++)
	{
		CScriptASTNode* child = root->_children[i];
		if ((dynamic_cast<CScriptFunctionASTNode*>(child)	!= NULL && dynamic_cast<CScriptFunctionASTNode*>(child)->Assigned == false) ||
				dynamic_cast<CScriptStateASTNode*>(child)		!= NULL)
		{
			CScriptFunctionASTNode* node = dynamic_cast<CScriptFunctionASTNode*>(child);
			if (node != NULL)
			{
				CScriptFunctionSymbol* funcSym = dynamic_cast<CScriptFunctionSymbol*>(node->FindSymbol(node->GetToken().Literal, true));
				funcSym->EntryPoint = _context->_instructions.Size();
			}

			child->GenerateInstructions(this);
		}

		GenerateNonGlobalScope(child);
	}
}


u32	CScriptGenerator::AllocateRegister(CScriptASTNode* node)
{
	for (u32 i = SCRIPT_MIN_GEN_PURPOSE_REGISTER; i <= SCRIPT_MAX_GEN_PURPOSE_REGISTER; i++)
	{
		if (_registersAllocated[i] == false)
		{
			_registersAllocated[i] = true;
			return i;
		}
	}
	Error(node, "Failed to allocated register.");
	return 0;
}

u32	CScriptGenerator::AllocateRegister(CScriptASTNode* node, u32 idx)
{
	LOG_ASSERT(_registersAllocated[idx] == false);	
	_registersAllocated[idx] = true;
	return idx;
}

void CScriptGenerator::DeallocateRegister(CScriptASTNode* node, u32 idx)
{
	LOG_ASSERT(_registersAllocated[idx] == true);
	_registersAllocated[idx] = false;
}

void CScriptGenerator::Disassemble()
{
	Engine::Containers::CArray<CScriptSymbol*> symbols = _context->_symbols;

	printf("\nGlobal:\n");
	for (u32 i = 0; i < _context->_instructions.Size() + 1; i++)
	{
		// Any jump targets point here?
		for (u32 j = 0; j < symbols.Size(); j++)
		{
			CScriptFunctionSymbol* func = dynamic_cast<CScriptFunctionSymbol*>(symbols[j]);
			if (func != NULL)
			{
				if (func->EntryPoint == i && func->EntryPoint != 0)
					printf("\n%s:\n", func->GetIdentifier().c_str());
			}

			CScriptJumpTargetSymbol* jumpTarget = dynamic_cast<CScriptJumpTargetSymbol*>(symbols[j]);
			if (jumpTarget != NULL)
			{
				if (jumpTarget->Index == i)
					printf("jmp_%i:\n", j);
			}
		}

		if (i < _context->_instructions.Size())
		{
			CScriptInstruction* instruction = _context->_instructions[i];
			Engine::Containers::CString str = S("\t") + S(ScriptInstructionOpCodes_String[instruction->Opcode]).PadEnd(13, ' ');

			//str += "\t\t";

			for (u32 i = 0; i < instruction->OperandCount; i++)
			{
				CScriptOperand op = instruction->Operands[i];

				switch (op.Type)
				{
					case SCRIPT_OPERAND_LITERAL_INT:		str += S(op.IntLiteral); break;
					case SCRIPT_OPERAND_LITERAL_FLOAT:		str += S(op.FloatLiteral); break;
					case SCRIPT_OPERAND_REGISTER:			str += S("reg[") + op.RegisterIndex + "]"; break;
					case SCRIPT_OPERAND_INSTRUCTION:		str += S("instr[") + op.InstructionIndex + "]"; break;
					case SCRIPT_OPERAND_STACK_INDEX:		str += S("stack[") + op.StackIndex + "]"; break;
					case SCRIPT_OPERAND_SYMBOL:				str += S("sym[") + op.Symbol->GetIdentifier() + "]"; break;
					case SCRIPT_OPERAND_JUMP_TARGET:		str += S("jmp_") + _context->_symbols.IndexOf(op.Symbol); break;
					default:								str += "INVALID"; break;
				}

				if (i < instruction->OperandCount - 1)
					str += (", ");
			}

			printf("%s\n", str.c_str());
		}
	}
}

void CScriptGenerator::GenerateSymbolList(AST::CScriptASTNode* node)
{
	// Append this nodes symbols to the list.
	Engine::Containers::CArray<CScriptSymbol*> symbols = node->GetSymbols();
	for (u32 i = 0; i < symbols.Size(); i++)
	{
		_context->_symbols.AddToEnd(symbols[i]);
	}

	// Generate the symbols for all sub nodes.
	Engine::Containers::CArray<Engine::Scripting::AST::CScriptASTNode*> nodes = node->GetChildren();
	for (u32 i = 0; i < nodes.Size(); i++)
	{
		GenerateSymbolList(nodes[i]);
	}
}

void CScriptGenerator::Error(CScriptASTNode* node, const Engine::Containers::CString& str)
{
	_context->PushError(CScriptError(SCRIPT_ERROR_FATAL, 
									 str, 
									 _context->_initialFile, node->GetToken().Line, node->GetToken().Column));
	throw str; // Abort the fuck out of parsing.
}

void CScriptGenerator::Warning(CScriptASTNode* node, const Engine::Containers::CString& str)
{
	_context->PushError(CScriptError(SCRIPT_ERROR_WARNING, 
									 str, 
									 _context->_initialFile, node->GetToken().Line, node->GetToken().Column));
}

void CScriptGenerator::Info(CScriptASTNode* node, const Engine::Containers::CString& str)
{
	_context->PushError(CScriptError(SCRIPT_ERROR_INFO, 
									 str, 
									 _context->_initialFile, node->GetToken().Line, node->GetToken().Column));
}
