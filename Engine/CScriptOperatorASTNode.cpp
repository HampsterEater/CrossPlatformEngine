///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptOperatorASTNode.h"
#include "CScriptGenerator.h"
#include "CScriptJumpTargetSymbol.h"
#include "CScriptManager.h"
#include "CScriptFunctionASTNode.h"
#include "CScriptVariableASTNode.h"
#include "CScriptFunctionSymbol.h"
#include "CScriptVariableSymbol.h"
#include "CScriptStringSymbol.h"
#include "CScriptCompileContext.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::AST;

CScriptOperatorASTNode::CScriptOperatorASTNode(Engine::Scripting::CScriptToken token, CScriptASTNode* parent)
{
	PostFix = false;
	_token = token;
	if (parent != NULL)
	{
		parent->GetChildren().AddToEnd(this);
	}
	_parent = parent;
}

CScriptOperatorASTNode::~CScriptOperatorASTNode()
{
	_children.Clear();
}

Engine::Containers::CString	CScriptOperatorASTNode::GetName()
{
	return _token.Literal;
}

void CScriptOperatorASTNode::GenerateSymbols(CScriptGenerator* gen)
{
	// Generate symbols.
	if (_children.Size() == 3 && _token.ID == TOKEN_OP_QUESTION)
	{
		_breakJumpTarget	= GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this); 
		AddSymbol(_breakJumpTarget);

		_continueJumpTarget	= GetScriptAllocator()->NewObj<Symbols::CScriptJumpTargetSymbol>(this); 
		AddSymbol(_continueJumpTarget);
	}

	// If we are doing a type check we need to make a string out of type name.
	if (_token.ID == TOKEN_KEYWORD_IS)
	{
		CScriptASTNode* child = _children[1];
		Symbols::CScriptSymbol* node = gen->GetContext()->GetASTRoot()->FindSymbol(child->GetToken().Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_STRING, 0);
		if (node == NULL)
		{
			Symbols::CScriptStringSymbol* symbol = GetScriptAllocator()->NewObj<Symbols::CScriptStringSymbol>(child);
			gen->GetContext()->GetASTRoot()->AddSymbol(symbol);
		}
	}

	// Generate children.
	CScriptASTNode::GenerateSymbols(gen);
}

u32 CScriptOperatorASTNode::GenerateInstructions(CScriptGenerator* gen)
{
	u32 output_reg = 0;

	// ---------------------------------------------------------------------------------------
	// Unary operation.
	// ---------------------------------------------------------------------------------------
	if (_children.Size() == 1 && _token.ID != TOKEN_OP_OPEN_PARENT)
	{
		output_reg = _children[0]->GenerateInstructions(gen);
		gen->AllocateRegister(this, output_reg);
	
		switch (_token.ID)
		{		
			// Unary negatate/absolute.
			case TOKEN_OP_ADD:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_ABS, CreateRegisterOperand(output_reg)); break;
			case TOKEN_OP_SUB:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_NEG, CreateRegisterOperand(output_reg)); break;
				
			// Logical/Bitwise Not.
			case TOKEN_OP_BITWISE_NOT:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWNOT, CreateRegisterOperand(output_reg)); break;
			case TOKEN_OP_LOGICAL_NOT:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_LNOT,  CreateRegisterOperand(output_reg)); break;

			// Postfix/Prefix increment/decrement.
			case TOKEN_OP_INCREMENT:			
			case TOKEN_OP_DECREMENT:
				{
					// Increment symbol.
					// Return old value.
					if (PostFix == true)
					{
						// inc output_reg
						CreateInstruction(gen, _token.ID == TOKEN_OP_INCREMENT ? Instructions::SCRIPT_OPCODE_INC : Instructions::SCRIPT_OPCODE_DEC, CreateRegisterOperand(output_reg));

						// Assign that shit.
						PerformAssignment(gen, _children[0], output_reg);

						// dec output_reg (reduce to old value)
						CreateInstruction(gen, _token.ID == TOKEN_OP_INCREMENT ? Instructions::SCRIPT_OPCODE_DEC : Instructions::SCRIPT_OPCODE_INC, CreateRegisterOperand(output_reg));
					}

					// Increment symbol.
					// Return new value.
					else
					{
						// inc output_reg
						CreateInstruction(gen, _token.ID == TOKEN_OP_INCREMENT ? Instructions::SCRIPT_OPCODE_INC : Instructions::SCRIPT_OPCODE_DEC, CreateRegisterOperand(output_reg));
						
						// Assign that shit.
						PerformAssignment(gen, _children[0], output_reg);
					}

					break;
				}
				
			default:
				{
					LOG_ASSERT(false);
				}
		}
	}
	
	// ---------------------------------------------------------------------------------------
	// Bi operation.
	// ---------------------------------------------------------------------------------------
	else if (_children.Size() == 2 || _token.ID == TOKEN_OP_OPEN_PARENT)
	{
		switch (_token.ID)
		{
			// (expr) is name
			case TOKEN_KEYWORD_IS:
				{
					// Load in expression on left.
					output_reg = _children[0]->GenerateInstructions(gen);	
					gen->AllocateRegister(this, output_reg);

					// Create symbol.
					Symbols::CScriptSymbol* sym = gen->GetContext()->GetASTRoot()->FindSymbol(_children[1]->GetToken().Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_STRING, 0);
					if (sym == NULL)
					{
						sym = GetScriptAllocator()->NewObj<Symbols::CScriptStringSymbol>(_children[1]);
						gen->GetContext()->GetASTRoot()->AddSymbol(sym);
					}

					// Check type.
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_ISTYPE, CreateRegisterOperand(output_reg), CreateSymbolOperand(sym));

					break;
				}
				
			// (expr) as name
			case TOKEN_KEYWORD_AS:
				{
					// Load in expression on left.
					output_reg = _children[0]->GenerateInstructions(gen);	
					gen->AllocateRegister(this, output_reg);

					// Create symbol.
					Symbols::CScriptSymbol* sym = gen->GetContext()->GetASTRoot()->FindSymbol(_children[1]->GetToken().Literal, true, Symbols::SCRIPT_SYMBOL_TYPE_STRING, 0);
					if (sym == NULL)
					{
						sym = GetScriptAllocator()->NewObj<Symbols::CScriptStringSymbol>(_children[1]);
						gen->GetContext()->GetASTRoot()->AddSymbol(sym);
					}

					// Check type.
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_ASTYPE, CreateRegisterOperand(output_reg), CreateSymbolOperand(sym));

					break;
				}
			// Indirection / Subscript operations.
			case TOKEN_OP_OPEN_BRACKET:
				{
					// Load in symbol from left.
					output_reg = _children[0]->GenerateInstructions(gen);	
					gen->AllocateRegister(this, output_reg);

					// Get index value.
					u32 index_reg = _children[1]->GenerateInstructions(gen);	
					
					// Get the subscript value.
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_IDX, CreateRegisterOperand(output_reg), CreateRegisterOperand(index_reg));

					break;
				}

			// Invokation,
			case TOKEN_OP_OPEN_PARENT:
				{
					// Load in the function pointer.
					output_reg = _children[0]->GenerateInstructions(gen);	
					gen->AllocateRegister(this, output_reg);

					// Push everything onto the stack.
					for (u32 i = 1; i < _children.Size(); i++)
					{
						u32 value_reg = _children[i]->GenerateInstructions(gen);	
						CreateInstruction(gen, Instructions::SCRIPT_OPCODE_PUSH, CreateRegisterOperand(value_reg));
					}

					// Invoke the function.
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_INVK, CreateRegisterOperand(output_reg), CreateIntOperand(_children.Size() - 1));

					// Move return value into output register.
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_MOV, CreateRegisterOperand(output_reg), CreateRegisterOperand(SCRIPT_CONST_REGISTER_RETURN));					

					break;
				}

			// Member access.
			case TOKEN_OP_PERIOD:
				{
					// Parse the l-value.
					output_reg = _children[0]->GenerateInstructions(gen);	
					gen->AllocateRegister(this, output_reg);

					// Parse the r-value.
					u32 rvalue_reg = _children[1]->GenerateInstructions(gen);	

					// Get the symbol indirection value.
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_INDR, CreateRegisterOperand(output_reg), CreateRegisterOperand(rvalue_reg));

					break;
				}
				
			// Assignment.
			case TOKEN_OP_ASSIGN:
				{
					// Parse the r-value.
					output_reg = _children[1]->GenerateInstructions(gen);	
					gen->AllocateRegister(this, output_reg);
					
					// Assign that shit.
					PerformAssignment(gen, _children[0], output_reg);

					break;
				}
			case TOKEN_OP_ASSIGN_BITWISE_SHR:
			case TOKEN_OP_ASSIGN_BITWISE_SHL:
			case TOKEN_OP_ASSIGN_BITWISE_NOT:
			case TOKEN_OP_ASSIGN_BITWISE_XOR:
			case TOKEN_OP_ASSIGN_BITWISE_OR:
			case TOKEN_OP_ASSIGN_BITWISE_AND:
			case TOKEN_OP_ASSIGN_MOD:
			case TOKEN_OP_ASSIGN_DIV:
			case TOKEN_OP_ASSIGN_MUL:
			case TOKEN_OP_ASSIGN_SUB:
			case TOKEN_OP_ASSIGN_ADD:
				{
					// Parse the l-value.
					output_reg = _children[0]->GenerateInstructions(gen);	
					gen->AllocateRegister(this, output_reg);

					// Parse the r-value.
					u32 reg2   = _children[1]->GenerateInstructions(gen);
					gen->AllocateRegister(this, reg2);
					
					// Add stuff together.
					switch (_token.ID)
					{
						case TOKEN_OP_ASSIGN_BITWISE_SHR:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWSHR, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
						case TOKEN_OP_ASSIGN_BITWISE_SHL:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWSHL, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;	
						case TOKEN_OP_ASSIGN_BITWISE_NOT:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWNOT, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
						case TOKEN_OP_ASSIGN_BITWISE_XOR:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWXOR, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
						case TOKEN_OP_ASSIGN_BITWISE_OR:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWOR,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
						case TOKEN_OP_ASSIGN_BITWISE_AND:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWAND, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
						case TOKEN_OP_ASSIGN_MOD:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_MOD,   CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
						case TOKEN_OP_ASSIGN_DIV:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_DIV,   CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
						case TOKEN_OP_ASSIGN_MUL:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_MUL,   CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
						case TOKEN_OP_ASSIGN_SUB:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_SUB,   CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
						case TOKEN_OP_ASSIGN_ADD:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_ADD,   CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					}

					// Assign that shit.
					PerformAssignment(gen, _children[0], output_reg);

					// Deallocate the register.
					gen->DeallocateRegister(this, reg2);

					break;
				}

			// Other operations.
			default:
			{
				output_reg = _children[0]->GenerateInstructions(gen);	
				gen->AllocateRegister(this, output_reg);

				u32 reg2   = _children[1]->GenerateInstructions(gen);
				gen->AllocateRegister(this, reg2);

				switch (_token.ID)
				{		
					// Normal arithmatic.
					case TOKEN_OP_BITWISE_OR:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWOR, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_BITWISE_AND:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWAND, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_BITWISE_XOR:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWXOR, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_BITWISE_SHL:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWSHL, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_BITWISE_SHR:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_BWSHR, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_MUL:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_MUL,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_DIV:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_DIV,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_ADD:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_ADD,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_SUB:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_SUB,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_MOD:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_MOD,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;

					// Logical operations.
					case TOKEN_OP_LOGICAL_AND:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_LAND, CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_LOGICAL_OR:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_LOR,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
			
					// Comparison operations.
					case TOKEN_OP_LESS:			CreateInstruction(gen, Instructions::SCRIPT_OPCODE_IL,   CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_GREATER:		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_IG,   CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_LESS_EQUAL:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_ILE,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_GREATER_EQUAL:CreateInstruction(gen, Instructions::SCRIPT_OPCODE_IGE,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_NOT_EQUAL:	CreateInstruction(gen, Instructions::SCRIPT_OPCODE_INE,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;
					case TOKEN_OP_EQUAL:		CreateInstruction(gen, Instructions::SCRIPT_OPCODE_IEQ,  CreateRegisterOperand(output_reg), CreateRegisterOperand(reg2)); break;

					default:
						{
							LOG_ASSERT(false);
						}
				}

				// Deallocate the register.
				gen->DeallocateRegister(this, reg2);
			}
		}
	}

	// ---------------------------------------------------------------------------------------
	// Ternary operation.
	// ---------------------------------------------------------------------------------------
	else if (_children.Size() == 3)
	{
		switch (_token.ID)
		{		
			case TOKEN_OP_QUESTION:
				{

					// jmp_1 = _breakJumpTarget
					// jmp_0 = _continueJumpTarget
					
					//		output = <expression>
					//		cmp output, 0
					//		je jmp_1
					//		reg2 = <block1>
					//		mov output, reg2			// Could be optimized out.
					//		jmp jmp_0
					// jmp_1:
					//		reg1 = <block2>
					//		mov output, reg1			// Could be optimized out.
					// jmp_0:

					// output = <expression>
					output_reg = _children[0]->GenerateInstructions(gen);	
					gen->AllocateRegister(this, output_reg);

					// cmp output, 0
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_CMP, 
									  CreateRegisterOperand(output_reg),
									  CreateRegisterOperand(SCRIPT_CONST_REGISTER_ZERO));

					// je jmp_1
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JEQ, 
									  CreateJumpTargetOperand(_breakJumpTarget));

					// reg1 = <block1>
					u32 reg1 = _children[1]->GenerateInstructions(gen);	
		
					// mov output, reg1
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_MOV, 
									  CreateRegisterOperand(output_reg),
									  CreateRegisterOperand(reg1));

					// jmp jmp_0
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_JMP, 
									  CreateJumpTargetOperand(_continueJumpTarget));

					_breakJumpTarget->Bind(gen, this);

					// reg2 = <block2>
					u32 reg2 = _children[2]->GenerateInstructions(gen);	
					
					// mov output, reg2
					CreateInstruction(gen, Instructions::SCRIPT_OPCODE_MOV, 
									  CreateRegisterOperand(output_reg),
									  CreateRegisterOperand(reg2));

					_continueJumpTarget->Bind(gen, this);

					break;
				}
			default:
				{
					LOG_ASSERT(false);
				}
		}
	}

	// Deallocate and return register!
	gen->DeallocateRegister(this, output_reg);
	return output_reg;
}
