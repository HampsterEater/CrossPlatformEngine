///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptManager.h"
#include "CString.h"
#include "CScriptSymbol.h"
#include "CScriptCompileContext.h"
#include "CScriptFunctionSymbol.h"
#include "CScriptVariableSymbol.h"
#include "CScriptStringSymbol.h"
#include "CScriptStateSymbol.h"
#include "CScriptStateASTNode.h"
#include "CScriptInstruction.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;
using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Instructions;

CScriptASTNode::CScriptASTNode()
{
	_parent					= NULL;
	_continueJumpTarget		= NULL;
	_breakJumpTarget		= NULL;
}

CScriptASTNode::CScriptASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	_token = token;
	if (parent != NULL)
	{
		parent->_children.AddToEnd(this);
	}
	_parent = parent;
}

CScriptASTNode::~CScriptASTNode()
{
	// Dispose of children.
	for (u32 i = 0; i < _children.Size(); i++)
	{
		Engine::Scripting::GetScriptAllocator()->FreeObj(&_children[i]);
	}
	_children.Clear();
}

void CScriptASTNode::PrettyPrint(u32 tabs)
{
	printf("%s\n", (Engine::Containers::CString('\t', tabs) + GetName()).c_str());
	for (u32 i = 0; i < _children.Size(); i++)
	{
		_children[i]->PrettyPrint(tabs + 1);
	}
}

void CScriptASTNode::SetParent(CScriptASTNode* node)
{
	_parent = node;
}

CScriptASTNode* CScriptASTNode::GetParent()
{
	return _parent;
}

Engine::Containers::CArray<CScriptASTNode*>& CScriptASTNode::GetChildren()
{
	return _children;
}

Engine::Scripting::CScriptToken CScriptASTNode::GetToken()
{
	return _token;
}

Engine::Containers::CString	CScriptASTNode::GetName()
{
	return "<node>";
}

Symbols::CScriptStateSymbol* CScriptASTNode::GetStateSymbol()
{
	//for (u32 i = 0; i < _children.Size(); i++)
	//{
	//	CScriptStateASTNode* node = dynamic_cast<CScriptStateASTNode*>(_children[i]);
	//	if (node != NULL)
	//	{
	//		return node->StateSymbol;
	//	}
	//}

	CScriptStateASTNode* node = dynamic_cast<CScriptStateASTNode*>(this);
	if (node != NULL)
		return node->StateSymbol;

	if (_parent != NULL)
		return _parent->GetStateSymbol();
	else		
		return NULL;
}

void CScriptASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols for children.
	for (u32 i = 0; i < _children.Size(); i++)
		_children[i]->GenerateSymbols(gen);
}

u32 CScriptASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	// Generate symbols for children.
	for (u32 i = 0; i < _children.Size(); i++)
		_children[i]->GenerateInstructions(gen);

	return 0;
}

Engine::Containers::CArray<Symbols::CScriptSymbol*>& CScriptASTNode::GetSymbols()
{
	return _symbols;
}

Symbols::CScriptSymbol*	CScriptASTNode::FindSymbol(Engine::Containers::CString& identifier, bool recursive, u32 type, u32 exceptType)
{
	Engine::Containers::CString ourIdentifier = identifier.ToLower();

	// Mask of "type" bits from "except type" bits lol, having both makes no sense.
	type = type & ~exceptType;

	for (u32 i = 0; i < _symbols.Size(); i++)
	{
		Engine::Containers::CString symbolIdentifier = _symbols[i]->GetIdentifier();

		// Do we need to check the type.
		if (type != 0 && (type & _symbols[i]->GetType()) == 0)
			continue;

		if (exceptType != 0 && (exceptType & _symbols[i]->GetType()) != 0)
			continue;

		if (symbolIdentifier.ToLower() == ourIdentifier)
			return _symbols[i];
	}

	if (recursive == true && _parent != NULL)
		return _parent->FindSymbol(identifier, recursive, type);
	else
		return NULL;
}

void CScriptASTNode::AddChild(CScriptASTNode* node)
{
	node->_parent = this;
	_children.AddToEnd(node);
}

void CScriptASTNode::AddSymbolToParent(Symbols::CScriptSymbol* symbol)
{
	_parent->_symbols.AddToEnd(symbol);
}

void CScriptASTNode::AddSymbol(Symbols::CScriptSymbol* symbol)
{
	_symbols.AddToEnd(symbol);
}

void CScriptASTNode::AddInstruction(CScriptGenerator* gen, Instructions::CScriptInstruction* instr)
{
	instr->Token = _token;
	gen->GetContext()->GetInstructions().AddToEnd(instr);
}

Instructions::CScriptInstruction* CScriptASTNode::CreateInstruction(CScriptGenerator* gen, ScriptInstructionOpCodes opcode)
{
	Instructions::CScriptInstruction* instr = GetScriptAllocator()->NewObj<Instructions::CScriptInstruction>();
	instr->Opcode		= opcode;
	instr->OperandCount = 0;	
	instr->Token = _token;
	
	gen->GetContext()->GetInstructions().AddToEnd(instr);

	return instr;
}

Instructions::CScriptInstruction* CScriptASTNode::CreateInstruction(CScriptGenerator* gen, ScriptInstructionOpCodes opcode, CScriptOperand op1)
{
	Instructions::CScriptInstruction* instr = GetScriptAllocator()->NewObj<Instructions::CScriptInstruction>();
	instr->Opcode		= opcode;
	instr->OperandCount = 1;
	instr->Operands[0]  = op1;	
	instr->Token = _token;
	
	gen->GetContext()->GetInstructions().AddToEnd(instr);

	return instr;
}

Instructions::CScriptInstruction* CScriptASTNode::CreateInstruction(CScriptGenerator* gen, ScriptInstructionOpCodes opcode, CScriptOperand op1, CScriptOperand op2)
{
	Instructions::CScriptInstruction* instr = GetScriptAllocator()->NewObj<Instructions::CScriptInstruction>();
	instr->Opcode		= opcode;
	instr->OperandCount = 2;
	instr->Operands[0]  = op1;
	instr->Operands[1]  = op2;	
	instr->Token = _token;

	gen->GetContext()->GetInstructions().AddToEnd(instr);

	return instr;
}

Instructions::CScriptInstruction* CScriptASTNode::CreateInstruction(CScriptGenerator* gen, ScriptInstructionOpCodes opcode, CScriptOperand op1, CScriptOperand op2, CScriptOperand op3)
{
	Instructions::CScriptInstruction* instr = GetScriptAllocator()->NewObj<Instructions::CScriptInstruction>();
	instr->Opcode		= opcode;
	instr->OperandCount = 3;
	instr->Operands[0]  = op1;
	instr->Operands[1]  = op2;
	instr->Operands[2]  = op3;	
	instr->Token = _token;
	
	gen->GetContext()->GetInstructions().AddToEnd(instr);

	return instr;
}

Instructions::CScriptOperand CScriptASTNode::CreateIntOperand(u32 value)
{
	Instructions::CScriptOperand op;
	op.Type = SCRIPT_OPERAND_LITERAL_INT;
	op.IntLiteral = value;

	return op;
}

Instructions::CScriptOperand CScriptASTNode::CreateFloatOperand(f32 value)
{
	Instructions::CScriptOperand op;
	op.Type = SCRIPT_OPERAND_LITERAL_FLOAT;
	op.FloatLiteral = value;

	return op;
}

Instructions::CScriptOperand CScriptASTNode::CreateRegisterOperand(u32 registerIndex)
{
	Instructions::CScriptOperand op;
	op.Type = SCRIPT_OPERAND_REGISTER;
	op.RegisterIndex = registerIndex;

	return op;
}

Instructions::CScriptOperand CScriptASTNode::CreateSymbolOperand(Symbols::CScriptSymbol* symbol)
{
	LOG_ASSERT(symbol != NULL);

	Instructions::CScriptOperand op;
	op.Type = SCRIPT_OPERAND_SYMBOL;
	op.Symbol = symbol;

	return op;
}

Instructions::CScriptOperand CScriptASTNode::CreateStackIndexOperand(s32 stackIndex)
{
	Instructions::CScriptOperand op;
	op.Type = SCRIPT_OPERAND_STACK_INDEX;
	op.StackIndex = stackIndex;

	return op;
}

Instructions::CScriptOperand CScriptASTNode::CreateInstructionIndexOperand(s32 stackIndex)
{
	Instructions::CScriptOperand op;
	op.Type = SCRIPT_OPERAND_INSTRUCTION;
	op.InstructionIndex = stackIndex;

	return op;
}

Instructions::CScriptOperand CScriptASTNode::CreateJumpTargetOperand(Symbols::CScriptJumpTargetSymbol* symbol)
{
	LOG_ASSERT(symbol != NULL);

	Instructions::CScriptOperand op;
	op.Type = SCRIPT_OPERAND_JUMP_TARGET;
	op.Symbol = symbol;

	return op;
}

Symbols::CScriptJumpTargetSymbol* CScriptASTNode::GetBreakJumpTarget()
{
	return _breakJumpTarget;
}

Symbols::CScriptJumpTargetSymbol* CScriptASTNode::GetContinueJumpTarget()
{
	return _continueJumpTarget;
}

// This function is responsible for generating the code that
// assigns the value in rvalueRegister to the tree defined in lvalue.
//
// Basically what this does is run generate the tree up to the last operator
// as normal 'getvalue'-code
//
// If then performs the setvalue operation on the final operator.
//
// This means that supported syntaxs for r-values are basically;
//
//		identifier		 = 0;
//		(sumexpr)[0]	 = 0;
//		(sumexpr).member = 0;
//
// Not supported (for obvious reasons);
//		
//		(sumexpr)++		= 0;
//		(sumexpr)()		= 1;
//
void CScriptASTNode::PerformAssignment(CScriptGenerator* gen, CScriptASTNode* lvalue, u32 rvalueRegister)
{
	switch (lvalue->GetToken().ID)
	{
		// (sumexpr).member = 0;
		case TOKEN_OP_PERIOD:
			{
				// Parse the l-value.
				u32 lvalue_reg = lvalue->GetChildren()[0]->GenerateInstructions(gen);	
				gen->AllocateRegister(this, lvalue_reg);

				// Load the r-value symbol.
				Symbols::CScriptSymbol* rvaluesym = lvalue->GetChildren()[1]->FindSymbol(lvalue->GetChildren()[1]->GetToken().Literal, true);

				// Get the symbol indirection value.
				CreateInstruction(gen, Instructions::SCRIPT_OPCODE_INDRS, CreateRegisterOperand(lvalue_reg), CreateSymbolOperand(rvaluesym), CreateRegisterOperand(rvalueRegister));

				// Deallocate register.
				gen->DeallocateRegister(this, lvalue_reg);
			}
			break;

		// (sumexpr)[0] = 0;
		case TOKEN_OP_OPEN_BRACKET:
			{
				// Parse the l-value.
				u32 lvalue_reg = lvalue->GetChildren()[0]->GenerateInstructions(gen);	
				gen->AllocateRegister(this, lvalue_reg);

				// Get index value.
				u32 index_reg = lvalue->GetChildren()[1]->GenerateInstructions(gen);	
					
				// Get the subscript value.
				CreateInstruction(gen, Instructions::SCRIPT_OPCODE_IDXS, CreateRegisterOperand(lvalue_reg), CreateRegisterOperand(index_reg), CreateRegisterOperand(rvalueRegister));

				// Deallocate register.
				gen->DeallocateRegister(this, lvalue_reg);
			}
			break;

		// x = 0;
		case TOKEN_LITERAL_IDENTIFIER:
			{
				Symbols::CScriptSymbol* symbol = dynamic_cast<Symbols::CScriptSymbol*>(lvalue->FindSymbol(lvalue->GetToken().Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_VARIABLE));
				if (symbol == NULL)
				{
					gen->Error(this, "Attempt to access undefined identifier '"+lvalue->GetToken().Literal+"'.");
					return;
				}
	
				if (dynamic_cast<Symbols::CScriptVariableSymbol*>(symbol) != NULL)
				{
					// ssym sym, reg0					
					StoreRegister(gen, symbol, rvalueRegister);
					//CreateInstruction(gen, Instructions::SCRIPT_OPCODE_SSYM,
					//					CreateSymbolOperand(symbol),
					//					CreateRegisterOperand(rvalueRegister));
				}
				else
				{
					gen->Error(this, "Attempt to assign r-value to constant expression.");
				}
			}
			break;

		default:
			gen->Error(this, "Attempt to assign r-value to constant expression.");
			break;
	}
}

void CScriptASTNode::LoadRegister(CScriptGenerator* gen, Symbols::CScriptSymbol* symbol, u32 reg)
{
	switch (symbol->GetType())
	{
		case SCRIPT_SYMBOL_TYPE_FUNCTION:
			{
				Symbols::CScriptFunctionSymbol* func = dynamic_cast<CScriptFunctionSymbol*>(symbol);
				CreateInstruction(gen, SCRIPT_OPCODE_LFUNC,
									   CreateRegisterOperand(reg),
									   CreateIntOperand(func->Index));
			}
			break;

		case SCRIPT_SYMBOL_TYPE_VARIABLE:
			{
				Symbols::CScriptVariableSymbol* var = dynamic_cast<CScriptVariableSymbol*>(symbol);
				CreateInstruction(gen, var->IsGlobal == true ? SCRIPT_OPCODE_LGLOBAL : SCRIPT_OPCODE_LLOCAL,
									   CreateRegisterOperand(reg),
									   CreateIntOperand(var->Index));
			}
			break;
		
		case SCRIPT_SYMBOL_TYPE_STRING:
			{
				Symbols::CScriptStringSymbol* var = dynamic_cast<CScriptStringSymbol*>(symbol);
				CreateInstruction(gen, SCRIPT_OPCODE_LDS,
									   CreateRegisterOperand(reg),
									   CreateSymbolOperand(var));
			}
			break;

		//case SCRIPT_SYMBOL_TYPE_STRING:
		//case SCRIPT_SYMBOL_TYPE_JUMPTARGET:
		//case SCRIPT_SYMBOL_TYPE_STATE:
		
		default:
			gen->Error(this, S("Unable to load value of symbol '%s', symbol is of invalid type.").Format(symbol->GetIdentifier().c_str()));
			break;
	}
}

void CScriptASTNode::StoreRegister(CScriptGenerator* gen, Symbols::CScriptSymbol* symbol, u32 reg)
{
	switch (symbol->GetType())
	{
		case SCRIPT_SYMBOL_TYPE_FUNCTION:
			{
				Symbols::CScriptFunctionSymbol* func = dynamic_cast<CScriptFunctionSymbol*>(symbol);
				CreateInstruction(gen, SCRIPT_OPCODE_SFUNC,
									   CreateRegisterOperand(reg),
									   CreateIntOperand(func->Index));
			}
			break;

		case SCRIPT_SYMBOL_TYPE_VARIABLE:
			{
				Symbols::CScriptVariableSymbol* var = dynamic_cast<CScriptVariableSymbol*>(symbol);
				CreateInstruction(gen, var->IsGlobal == true ? SCRIPT_OPCODE_SGLOBAL : SCRIPT_OPCODE_SLOCAL,
									   CreateRegisterOperand(reg),
									   CreateIntOperand(var->Index));
			}
			break;

		//case SCRIPT_SYMBOL_TYPE_STRING:
		//case SCRIPT_SYMBOL_TYPE_JUMPTARGET:
		//case SCRIPT_SYMBOL_TYPE_STATE:
		
		default:
			gen->Error(this, S("Unable to store value of symbol '%s', symbol is of invalid type.").Format(symbol->GetIdentifier().c_str()));
			break;
	}
}
