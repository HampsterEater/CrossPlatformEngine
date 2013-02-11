///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CString.h"
#include "CArray.h"

#include "CScriptLexer.h"
#include "CScriptParser.h"
#include "CScriptInstruction.h"
#include "CScriptSymbol.h"

#include "CScriptASTNode.h"
#include "CScriptClassASTNode.h"
#include "CScriptUsingASTNode.h"
#include "CScriptVariableASTNode.h"
#include "CScriptFunctionASTNode.h"
#include "CScriptForeachASTNode.h"

#include "CScriptCompileContext.h"

#include "CStream.h"

namespace Engine
{
    namespace Scripting
    {

		class CScriptParser;
		class CScriptLexer;
		struct CScriptToken;

		// Scripting error levels. 
		// These are bitflags, they can be combined when doing searchs.
		enum ScriptErrorLevel
		{
			SCRIPT_ERROR_FATAL		= 1,	// Causes compilation to terminate.
			SCRIPT_ERROR_WARNING	= 2,	// Gives the user a warning that something should not be done, but accepts it anyway.
			SCRIPT_ERROR_INFO 		= 4,	// General notice about any unimportant aspect of the code.

			SCRIPT_ERROR_ALL		= SCRIPT_ERROR_FATAL|SCRIPT_ERROR_WARNING|SCRIPT_ERROR_INFO
		};

		// Stores information on a single error that occured within a script.
		struct CScriptError
		{
			// Position
			Engine::Containers::CString File;
			u32							Line;
			u32							Column;

			// Error description
			ScriptErrorLevel			Level;
			Engine::Containers::CString	Description;

			// Constructor!
			CScriptError()
			{

			}
			CScriptError(ScriptErrorLevel level, const Engine::Containers::CString& description, const Engine::Containers::CString& file="<unknown>", u32 line=0, u32 column=0)
			{
				Level		= level;
				Description = description;
				File		= file;
				Line		= line;
				Column		= column;
			}
		};

		// Script file defines.
		#define SCRIPT_FILE_SIGNATURE				*((u32*)"ISCR")
		#define SCRIPT_FILE_VERSION					1

		// The compiler context is used to pass the script source between the different
		// parts of the scripting language compiler.
		class CScriptCompileContext
		{
			protected:
				Engine::Containers::CString										_initialFile;
				Engine::Containers::CString										_rawSource;
				Engine::Containers::CArray<CScriptError>						_errorList;
				Engine::Containers::CArray<CScriptToken>						_tokenList;
				AST::CScriptASTNode*											_astTree;

				// Final "output" state.
				Engine::Containers::CArray<Instructions::CScriptInstruction*>	_instructions;
				Engine::Containers::CArray<Symbols::CScriptSymbol*>				_symbols;
				bool															_isClass;
				Engine::Containers::CString										_className;
				Engine::Containers::CString										_classBaseName;
				u32																_globalVariableCount;
				u32																_functionTableSize;
	

				// Used with the parser for unexpected eof messages.
				u32																_lastLineIndex;
				u32																_lastLineColumn;

				bool															_saveable;

			protected:
				void PushError				(const CScriptError& error);
				void PushToken				(const CScriptToken& token);
				void DisposeAll				();

			public:
				CScriptCompileContext		(const Engine::Containers::CString& raw, const Engine::Containers::CString& file = "<string>");
				~CScriptCompileContext		();
				
				Engine::Containers::CArray<Instructions::CScriptInstruction*>&		GetInstructions();
				AST::CScriptASTNode*												GetASTRoot();

				u32							GetErrorCount	(ScriptErrorLevel level = SCRIPT_ERROR_ALL);
				CScriptError&				GetError		(u32 index);
				Engine::Containers::CString FormatError		(const CScriptError& error);

				bool						Load			(Engine::FileSystem::Streams::CStream* stream);
				void						Save			(Engine::FileSystem::Streams::CStream* stream);

				void						ReloadRawSource	();

			friend class CScriptLexer;
			friend class CScriptParser;
			friend class CScriptGenerator;
			friend class CScriptExecutionContext;

			friend class AST::CScriptASTNode;
			friend class AST::CScriptClassASTNode;
			friend class AST::CScriptUsingASTNode;
			friend class AST::CScriptVariableASTNode;
			friend class AST::CScriptForeachASTNode;
			friend class AST::CScriptFunctionASTNode;

		};

	}
}