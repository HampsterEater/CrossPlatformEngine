///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

namespace Engine
{
    namespace Scripting
    {
		class CScriptCompileContext;

		// Numeric identifier of a token.
		enum TokenID
		{
			// Keywords.
			TOKEN_KEYWORD_USING,
			TOKEN_KEYWORD_INCLUDE,
			TOKEN_KEYWORD_DEFINE,
			TOKEN_KEYWORD_IFDEF,
			TOKEN_KEYWORD_IFNDEF,
			TOKEN_KEYWORD_OBJECT,
			TOKEN_KEYWORD_CLASS,
			TOKEN_KEYWORD_EXTENDS,
			TOKEN_KEYWORD_WHILE,
			TOKEN_KEYWORD_FOR,
			TOKEN_KEYWORD_FOREACH,
			TOKEN_KEYWORD_DO,
			TOKEN_KEYWORD_FOREVER,
			TOKEN_KEYWORD_EACHIN,
			TOKEN_KEYWORD_TO,
			TOKEN_KEYWORD_ENUM,
			TOKEN_KEYWORD_VAR,
			TOKEN_KEYWORD_CONST,
			TOKEN_KEYWORD_STATIC,
			TOKEN_KEYWORD_INTERNAL,
			TOKEN_KEYWORD_NETWORK,
			TOKEN_KEYWORD_SYNC,
			TOKEN_KEYWORD_VIRTUAL,
			TOKEN_KEYWORD_OVERRIDE,
			TOKEN_KEYWORD_PROPERTY,
			TOKEN_KEYWORD_EXTERN,
			TOKEN_KEYWORD_EXPORT,
			TOKEN_KEYWORD_EXPLICIT,
			TOKEN_KEYWORD_FRIEND,
			TOKEN_KEYWORD_GOTO,
			TOKEN_KEYWORD_NAMESPACE,
			TOKEN_KEYWORD_NEW,
			TOKEN_KEYWORD_DELETE,
			TOKEN_KEYWORD_OPERATOR,
			TOKEN_KEYWORD_PRIVATE,
			TOKEN_KEYWORD_REF,
			TOKEN_KEYWORD_PTR,
			TOKEN_KEYWORD_SEALED,
			TOKEN_KEYWORD_SIGNED,
			TOKEN_KEYWORD_UNSIGNED,
			TOKEN_KEYWORD_PROTECTED,
			TOKEN_KEYWORD_PUBLIC,
			TOKEN_KEYWORD_STRUCT,
			TOKEN_KEYWORD_FINAL,
			TOKEN_KEYWORD_TRY,
			TOKEN_KEYWORD_CATCH,
			TOKEN_KEYWORD_FINALLY,
			TOKEN_KEYWORD_THROW,
			TOKEN_KEYWORD_RAISE,
			TOKEN_KEYWORD_LITERAL,
			TOKEN_KEYWORD_IF,
			TOKEN_KEYWORD_ELSE,
			TOKEN_KEYWORD_END,
			TOKEN_KEYWORD_SWITCH,
			TOKEN_KEYWORD_CASE,
			TOKEN_KEYWORD_DEFAULT,
			TOKEN_KEYWORD_BREAK,
			TOKEN_KEYWORD_CONTINUE,
			TOKEN_KEYWORD_STATE,
			TOKEN_KEYWORD_GOTOSTATE,
			TOKEN_KEYWORD_FUNCTION,
			TOKEN_KEYWORD_EVENT,
			TOKEN_KEYWORD_GENERATOR,
			TOKEN_KEYWORD_NATIVE,
			TOKEN_KEYWORD_ABSTRACT,
			TOKEN_KEYWORD_DELEGATE,
			TOKEN_KEYWORD_RETURN,
			TOKEN_KEYWORD_GENERIC,
			TOKEN_KEYWORD_INTERFACE,
			TOKEN_KEYWORD_DEPRECATED,
			TOKEN_KEYWORD_DYNAMIC,
			TOKEN_KEYWORD_MUTABLE,
			TOKEN_KEYWORD_VOLATILE,
			TOKEN_KEYWORD_REGISTER,
			TOKEN_KEYWORD_YIELD,
			TOKEN_KEYWORD_INT,
			TOKEN_KEYWORD_LONG,
			TOKEN_KEYWORD_DOUBLE,
			TOKEN_KEYWORD_FLOAT,
			TOKEN_KEYWORD_STRING,			
			TOKEN_KEYWORD_DICT,
			TOKEN_KEYWORD_LIST,
			TOKEN_KEYWORD_VOID,
			TOKEN_KEYWORD_VALUE,
			TOKEN_KEYWORD_LOCK,
			TOKEN_KEYWORD_JUMP,
			TOKEN_KEYWORD_SAFE,
			TOKEN_KEYWORD_UNSAFE,
			TOKEN_KEYWORD_AS,
			TOKEN_KEYWORD_BASE,
			TOKEN_KEYWORD_FIXED,
			TOKEN_KEYWORD_OUT,
			TOKEN_KEYWORD_IN,
			TOKEN_KEYWORD_THIS,
			TOKEN_KEYWORD_SELF,
			TOKEN_KEYWORD_GET,
			TOKEN_KEYWORD_SET,
			TOKEN_KEYWORD_IS,

			// Literals.
			TOKEN_LITERAL_NULL,
			TOKEN_LITERAL_TRUE,
			TOKEN_LITERAL_FALSE,
			//TOKEN_LITERAL_DICT,
			//TOKEN_LITERAL_LIST,
			TOKEN_LITERAL_INT,
			//TOKEN_LITERAL_LONG,
			//TOKEN_LITERAL_DOUBLE,
			TOKEN_LITERAL_FLOAT,
			TOKEN_LITERAL_STRING,
			TOKEN_LITERAL_IDENTIFIER,

			// Punctuators
			TOKEN_OP_AT,
			TOKEN_OP_SEMICOLON,
			TOKEN_OP_COLON,
			TOKEN_OP_COMMA,
			TOKEN_OP_PERIOD,
			TOKEN_OP_QUESTION,
			TOKEN_OP_HASH,
			TOKEN_OP_FOWARD_SLASH,
			TOKEN_OP_TIDLE,

			TOKEN_OP_OPEN_BRACKET,
			TOKEN_OP_CLOSE_BRACKET,
			TOKEN_OP_OPEN_PARENT,
			TOKEN_OP_CLOSE_PARENT,
			TOKEN_OP_OPEN_BRACE,
			TOKEN_OP_CLOSE_BRACE,

			TOKEN_OP_ASSIGN_BITWISE_SHR,
			TOKEN_OP_ASSIGN_BITWISE_SHL,
			TOKEN_OP_ASSIGN_BITWISE_NOT,
			TOKEN_OP_ASSIGN_BITWISE_XOR,
			TOKEN_OP_ASSIGN_BITWISE_OR,
			TOKEN_OP_ASSIGN_BITWISE_AND,
			TOKEN_OP_ASSIGN_MOD,
			TOKEN_OP_ASSIGN_DIV,
			TOKEN_OP_ASSIGN_MUL,
			TOKEN_OP_ASSIGN_SUB,
			TOKEN_OP_ASSIGN_ADD,
			TOKEN_OP_ASSIGN,
			
			TOKEN_OP_INCREMENT,			
			TOKEN_OP_DECREMENT,

			TOKEN_OP_BITWISE_OR,
			TOKEN_OP_BITWISE_AND,
			TOKEN_OP_BITWISE_XOR,
			TOKEN_OP_BITWISE_NOT,
			TOKEN_OP_BITWISE_SHL,
			TOKEN_OP_BITWISE_SHR,
			TOKEN_OP_MUL,
			TOKEN_OP_DIV,
			TOKEN_OP_ADD,
			TOKEN_OP_SUB,
			TOKEN_OP_MOD,

			TOKEN_OP_LOGICAL_AND,
			TOKEN_OP_LOGICAL_OR,
			TOKEN_OP_LOGICAL_NOT,
			
			TOKEN_OP_LESS,
			TOKEN_OP_GREATER,
			TOKEN_OP_LESS_EQUAL,
			TOKEN_OP_GREATER_EQUAL,
			TOKEN_OP_NOT_EQUAL,
			TOKEN_OP_EQUAL,

			TOKEN_OP_EOF,
			TOKEN_OP_SOF
		};

		// Used for converting a literal keyword to a numeric identifier.
		struct TokenKeywordTableEntry
		{
			u8*		Identifier;
			TokenID ID;

			TokenKeywordTableEntry(u8* identifier, TokenID id)
			{
				Identifier = identifier;
				ID = id;
			}
		};

		extern TokenKeywordTableEntry TokenKeywordTable[];

		// Stores a tokenized segment of the script.
		struct CScriptToken
		{
			TokenID						ID;
			Engine::Containers::CString Literal;
			u32							Line;
			u32							Column;

			CScriptToken()
			{

			}
			CScriptToken(TokenID id, const Engine::Containers::CString& lit, u32 line=0, u32 column=0)
			{
				ID		= id;
				Literal = lit;
				Line	= line;
				Column	= column;
			}
		};

		// The lexical analyser is responsible for taking a scripting and converting
		// it into a stream of tokens.
		class CScriptLexer
		{
			private:
				CScriptCompileContext* _context;
				s32					   _position;

				s32					   _line;
				s32					   _column;

				s32					   _tokenStart;
				s32					   _tokenStartLine;
				s32					   _tokenStartColumn;
				
			public:
				CScriptLexer					();
				~CScriptLexer					();

				bool			EndOfFile		(u32 offset=0);
				u8				NextChar		();
				u8				CurrentChar		();
				u8				PreviousChar	();
				u8				LookAheadChar	(u32 offset=0);

				void			Error			(const Engine::Containers::CString& str);

				void			StartToken		();
				CScriptToken	MakeToken		(TokenID id, const Engine::Containers::CString& str);

				bool			ReadToken		(CScriptToken& token);
				bool			ReadNumber		(u8 startChar, CScriptToken& token);
				bool			ReadIdentifier	(u8 startChar, CScriptToken& token);

				bool			Analyze			(CScriptCompileContext* context);

		};

	}
}