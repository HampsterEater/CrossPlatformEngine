///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptCompileContext.h"
#include "CScriptLexer.h"
#include "CScriptManager.h"
#include "CScriptASTNode.h"
#include "CStream.h"
#include "CScriptStateSymbol.h"
#include "CScriptFunctionSymbol.h"
#include "CScriptVariableSymbol.h"
#include "CScriptStringSymbol.h"

#include "CFileStream.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::Symbols;

CScriptCompileContext::CScriptCompileContext(const Engine::Containers::CString& raw, const Engine::Containers::CString& file)
{
	_rawSource		= raw;
	_initialFile	= file;
	_astTree		= NULL;

	_isClass		= false;
	_className		= "";
	_classBaseName	= "";

	_globalVariableCount = 0;
	_functionTableSize = 0;

	_saveable		= false;
}

CScriptCompileContext::~CScriptCompileContext()
{
	DisposeAll();
}

void CScriptCompileContext::DisposeAll()
{
	// Dispose of the AST tree.
	if (_astTree != NULL)
		Engine::Scripting::GetScriptAllocator()->FreeObj(&_astTree);

	// Destroy instructions.
	for (u32 i = 0; i < _instructions.Size(); i++)
	{
		if (_instructions[i] != NULL)
			Engine::Scripting::GetScriptAllocator()->FreeObj(&_instructions[i]);
	}
	_instructions.Clear();

	// Destroy symbols.
	for (u32 i = 0; i < _symbols.Size(); i++)
	{
		if (_symbols[i] != NULL)
			Engine::Scripting::GetScriptAllocator()->FreeObj(&_symbols[i]);
	}
	_symbols.Clear();

	// Clear symbol list (disposal will be done by AST nodes that own them).
	_symbols.Clear();
}

Engine::Containers::CArray<Instructions::CScriptInstruction*>& CScriptCompileContext::GetInstructions()
{
	return _instructions;
}

AST::CScriptASTNode* CScriptCompileContext::GetASTRoot()
{
	return _astTree;
}

void CScriptCompileContext::PushError(const CScriptError& error)
{
	_errorList.AddToEnd(error);
}

void CScriptCompileContext::PushToken(const CScriptToken& token)
{
	_tokenList.AddToEnd(token);
}

u32	CScriptCompileContext::GetErrorCount(ScriptErrorLevel level)
{
	u32 count = 0;
	for (u32 i = 0; i < _errorList.Size(); i++)
	{
		CScriptError error = _errorList[i];
		if ((level & error.Level) == error.Level)
		{
			count++;
		}
	}
	return count;
}

CScriptError& CScriptCompileContext::GetError(u32 index)
{
	return _errorList[index];
}

Engine::Containers::CString CScriptCompileContext::FormatError(const CScriptError& error)
{
	u32							lineIndex = 1;
	Engine::Containers::CString line	  = "";

	// Get the line this error is on.
	for (u32 offset = 0; offset < _rawSource.Length(); offset++)
	{
		u8 chr = _rawSource[offset];
		if (chr == '\n')
		{
			if (lineIndex == error.Line)
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

	// Calculate the formatted message.
	// Should be in this format;
	//
	// File.cpp(0:0): Warning: Unable to find function 'x'
	// _ctest = func(123, x());
	//					  ^	
	Engine::Containers::CString msg = "";
	msg += S(error.File) + "(" + error.Line + ":" + error.Column + "): ";
	switch (error.Level)
	{
		case SCRIPT_ERROR_FATAL:		msg += "Error: ";	break;
		case SCRIPT_ERROR_WARNING:		msg += "Warning: ";	break;
		case SCRIPT_ERROR_INFO:			msg += "Info: ";	break;
	}
	msg += error.Description + "\n";
	msg += line + "\n";

	if (error.Column > 1)
		msg += Engine::Containers::CString(' ', error.Column - 1);

	msg += "^\n";

	return msg;
}

bool CScriptCompileContext::Load(Engine::FileSystem::Streams::CStream* stream)
{
	// Dispose our current resources.
	DisposeAll();

	// Read header.
	if (stream->ReadU32() != SCRIPT_FILE_SIGNATURE)
		return false;
	if (stream->ReadU8() != SCRIPT_FILE_VERSION)
		return false;

	u32 instructionCount	= stream->ReadU32();
	u32 symbolCount			= stream->ReadU32();
	_isClass				= stream->ReadU8() != 0;
	_className				= stream->ReadString();
	_classBaseName			= stream->ReadString();
	_initialFile			= stream->ReadString();
	_globalVariableCount	= stream->ReadU32();
	_functionTableSize		= stream->ReadU32();
	
	// Read symbols.
	for (u32 i = 0; i < symbolCount; i++)
	{
		Symbols::ScriptSymbolTypes type = (Symbols::ScriptSymbolTypes)stream->ReadU8();

		CScriptToken token;
		token.Literal	= stream->ReadString();
		token.Line		= stream->ReadU16();
		token.Column	= stream->ReadU16();

		switch (type)
		{
			case SCRIPT_SYMBOL_TYPE_FUNCTION:
				{
					Symbols::CScriptFunctionSymbol* func = GetScriptAllocator()->NewObj<CScriptFunctionSymbol>(token);
					func->EntryPoint = stream->ReadU32();
					func->ParameterCount = stream->ReadU32();
					func->LocalCount = stream->ReadU32();
					func->Index = stream->ReadU16();
					
					s32 idx = stream->ReadS32();
					if (idx < 0)
						func->State = NULL;
					else
						func->State = static_cast<Symbols::CScriptStateSymbol*>(_symbols[idx]);
					
					func->Type = static_cast<AST::ScriptFunctionType>(stream->ReadU8());

					_symbols.AddToEnd(func);
				}
				break;

			case SCRIPT_SYMBOL_TYPE_STATE:
				{
					Symbols::CScriptStateSymbol* state = GetScriptAllocator()->NewObj<CScriptStateSymbol>(token);
					state->IsDefault = stream->ReadU8() != 0;					
					_symbols.AddToEnd(state);
				}
				break;

			case SCRIPT_SYMBOL_TYPE_VARIABLE:
				{
					Symbols::CScriptVariableSymbol* var = GetScriptAllocator()->NewObj<CScriptVariableSymbol>(token);
					var->Index = stream->ReadU32();
					var->IsGlobal = stream->ReadU8() != 0;
					_symbols.AddToEnd(var);
				}
				break;

			case SCRIPT_SYMBOL_TYPE_STRING:
				{
					Symbols::CScriptStringSymbol* str = GetScriptAllocator()->NewObj<CScriptStringSymbol>(token);
					_symbols.AddToEnd(str);
				}
				break;
		}
	}

	// Read instructions.
	for (u32 i = 0; i < instructionCount; i++)
	{
		Instructions::CScriptInstruction* instr = GetScriptAllocator()->NewObj<Instructions::CScriptInstruction>();
		_instructions.AddToEnd(instr);

		instr->Opcode		= (Instructions::ScriptInstructionOpCodes)stream->ReadU8();
		instr->OperandCount = stream->ReadU8();
		instr->Token.Line	= stream->ReadU16();
		instr->Token.Column	= stream->ReadU16();
	
		for (u32 op = 0; op < instr->OperandCount; op++)
		{
			instr->Operands[op].Type = (Instructions::ScriptOperandType)stream->ReadU8();
			switch (instr->Operands[op].Type)
			{
				case Instructions::SCRIPT_OPERAND_LITERAL_INT:
					instr->Operands[op].IntLiteral = stream->ReadU32();
					break;

				case Instructions::SCRIPT_OPERAND_LITERAL_FLOAT:
					instr->Operands[op].FloatLiteral = stream->ReadF32();
					break;
				
				case Instructions::SCRIPT_OPERAND_REGISTER:
					instr->Operands[op].RegisterIndex = stream->ReadU8();
					break;

				case Instructions::SCRIPT_OPERAND_INSTRUCTION:
					instr->Operands[op].InstructionIndex = stream->ReadU32();
					break;

				case Instructions::SCRIPT_OPERAND_STACK_INDEX:
					instr->Operands[op].StackIndex = stream->ReadS32();
					break;

				case Instructions::SCRIPT_OPERAND_SYMBOL:
					{
						u32 idx = stream->ReadU32();
						instr->Operands[op].Symbol = _symbols[idx];
						break;
					}

				case Instructions::SCRIPT_OPERAND_JUMP_TARGET:
					instr->Operands[op].InstructionIndex = stream->ReadU32();
					break;
			}

		}
	}
	
	_saveable = true;
	return true;
}

void CScriptCompileContext::Save(Engine::FileSystem::Streams::CStream* stream)
{
	LOG_ASSERT(_saveable == true);

	// Strip all jump targets from symbol table.
	while (true)
	{
		u32 counter = 0;

		for (u32 i = 0; i < _symbols.Size(); i++)
		{
			if (_symbols[i]->GetType() == SCRIPT_SYMBOL_TYPE_JUMPTARGET)
			{
				Engine::Scripting::GetScriptAllocator()->FreeObj(&_symbols[i]);
				_symbols.RemoveIndex(i);
				counter++;
				break;
			}
		}

		if (counter <= 0)
			break;
	}

	// Write header.
	stream->WriteU32	(SCRIPT_FILE_SIGNATURE);
	stream->WriteU8		(SCRIPT_FILE_VERSION);
	stream->WriteU32	(_instructions.Size());
	stream->WriteU32	(_symbols.Size());
	stream->WriteU8		(_isClass);
	stream->WriteString	(_className);
	stream->WriteString	(_classBaseName);
	stream->WriteString	(_initialFile);
	stream->WriteU32	(_globalVariableCount);
	stream->WriteU32	(_functionTableSize);
	
	// Write symbols.
	for (u32 i = 0; i < _symbols.Size(); i++)
	{
		Symbols::CScriptSymbol* sym = _symbols[i];
		stream->WriteU8		(sym->GetType());
		stream->WriteString	(sym->GetToken().Literal);
		stream->WriteU16	(sym->GetToken().Line);
		stream->WriteU16	(sym->GetToken().Column);

		switch (sym->GetType())
		{
			case SCRIPT_SYMBOL_TYPE_FUNCTION:
				{
					Symbols::CScriptFunctionSymbol* func = dynamic_cast<CScriptFunctionSymbol*>(sym);
					stream->WriteU32(func->EntryPoint);
					stream->WriteU32(func->ParameterCount);
					stream->WriteU32(func->LocalCount);
					stream->WriteU16(func->Index);
					if (func->State == NULL)
						stream->WriteS32(-1);
					else
						stream->WriteS32(_symbols.IndexOf(func->State));

					stream->WriteU8(func->Type);
				}
				break;

			case SCRIPT_SYMBOL_TYPE_STATE:
				stream->WriteU8		(dynamic_cast<CScriptStateSymbol*>(sym)->IsDefault);
				break;

			case SCRIPT_SYMBOL_TYPE_VARIABLE:
				{
					Symbols::CScriptVariableSymbol* var = dynamic_cast<CScriptVariableSymbol*>(sym);	
					stream->WriteU32(var->Index);
					stream->WriteU8(var->IsGlobal);
				}
				break;

			case SCRIPT_SYMBOL_TYPE_JUMPTARGET:
			case SCRIPT_SYMBOL_TYPE_STRING:
				break;
		}
	}

	// Write instructions.
	for (u32 i = 0; i < _instructions.Size(); i++)
	{
		Instructions::CScriptInstruction* instr = _instructions[i];
		stream->WriteU8		(instr->Opcode);
		stream->WriteU8		(instr->OperandCount);
		stream->WriteU16	(instr->Token.Line);
		stream->WriteU16	(instr->Token.Column);
		
		for (u32 j = 0; j < instr->OperandCount; j++)
		{
			Instructions::CScriptOperand op = instr->Operands[j];
			stream->WriteU8(op.Type);

			switch (op.Type)
			{
				case Instructions::SCRIPT_OPERAND_LITERAL_INT:
					stream->WriteU32(op.IntLiteral);
					break;

				case Instructions::SCRIPT_OPERAND_LITERAL_FLOAT:
					stream->WriteF32(op.FloatLiteral);
					break;
				
				case Instructions::SCRIPT_OPERAND_REGISTER:
					stream->WriteU8(op.RegisterIndex);
					break;

				case Instructions::SCRIPT_OPERAND_INSTRUCTION:
					stream->WriteU32(op.InstructionIndex);
					break;

				case Instructions::SCRIPT_OPERAND_STACK_INDEX:
					stream->WriteS32(op.StackIndex);
					break;
					
				case Instructions::SCRIPT_OPERAND_JUMP_TARGET:	
					stream->WriteU32(reinterpret_cast<Symbols::CScriptJumpTargetSymbol*>(op.Symbol)->Index);
					break;

				case Instructions::SCRIPT_OPERAND_SYMBOL:
					stream->WriteU32(_symbols.IndexOf(op.Symbol));
					break;
			}
		}
	}
}

void CScriptCompileContext::ReloadRawSource()
{
	if (Engine::Platform::PathIsFile(_initialFile))
	{
		Engine::FileSystem::Streams::CFileStream f(_initialFile, Engine::Platform::FILE_ACCESS_MODE_READ, Engine::Platform::FILE_OPEN_MODE_OPEN_EXISTING);
		if (f.Open())
		{
			_rawSource = f.ReadToEnd();
			f.Close();
		}
	}
}
