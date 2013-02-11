///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptLexer.h"
#include "CScriptCompileContext.h"

using namespace Engine::Scripting;

TokenKeywordTableEntry Engine::Scripting::TokenKeywordTable[] = {
	TokenKeywordTableEntry("using",			TOKEN_KEYWORD_USING),
	TokenKeywordTableEntry("include",		TOKEN_KEYWORD_INCLUDE),
	TokenKeywordTableEntry("define",		TOKEN_KEYWORD_DEFINE),
	TokenKeywordTableEntry("ifdef",			TOKEN_KEYWORD_IFDEF),
	TokenKeywordTableEntry("ifndef",		TOKEN_KEYWORD_IFNDEF),
	TokenKeywordTableEntry("object",		TOKEN_KEYWORD_OBJECT),
	TokenKeywordTableEntry("class",			TOKEN_KEYWORD_CLASS),
	TokenKeywordTableEntry("extends",		TOKEN_KEYWORD_EXTENDS),
	TokenKeywordTableEntry("while",			TOKEN_KEYWORD_WHILE),
	TokenKeywordTableEntry("for",			TOKEN_KEYWORD_FOR),
	TokenKeywordTableEntry("foreach",		TOKEN_KEYWORD_FOREACH),
	TokenKeywordTableEntry("do",			TOKEN_KEYWORD_DO),
	TokenKeywordTableEntry("forever",		TOKEN_KEYWORD_FOREVER),
	TokenKeywordTableEntry("eachin",		TOKEN_KEYWORD_EACHIN),
	TokenKeywordTableEntry("to",			TOKEN_KEYWORD_TO),
	TokenKeywordTableEntry("enum",			TOKEN_KEYWORD_ENUM),
	TokenKeywordTableEntry("var",			TOKEN_KEYWORD_VAR),
	TokenKeywordTableEntry("const",			TOKEN_KEYWORD_CONST),
	TokenKeywordTableEntry("static",		TOKEN_KEYWORD_STATIC),
	TokenKeywordTableEntry("internal",		TOKEN_KEYWORD_INTERNAL),
	TokenKeywordTableEntry("network",		TOKEN_KEYWORD_NETWORK),
	TokenKeywordTableEntry("sync",			TOKEN_KEYWORD_SYNC),
	TokenKeywordTableEntry("virtual",		TOKEN_KEYWORD_VIRTUAL),
	TokenKeywordTableEntry("override",		TOKEN_KEYWORD_OVERRIDE),
	TokenKeywordTableEntry("property",		TOKEN_KEYWORD_PROPERTY),
	TokenKeywordTableEntry("extern",		TOKEN_KEYWORD_EXTERN),
	TokenKeywordTableEntry("export",		TOKEN_KEYWORD_EXPORT),
	TokenKeywordTableEntry("explicit",		TOKEN_KEYWORD_EXPLICIT),
	TokenKeywordTableEntry("friend",		TOKEN_KEYWORD_FRIEND),
	TokenKeywordTableEntry("goto",			TOKEN_KEYWORD_GOTO),
	TokenKeywordTableEntry("namespace",		TOKEN_KEYWORD_NAMESPACE),
	TokenKeywordTableEntry("new",			TOKEN_KEYWORD_NEW),
	TokenKeywordTableEntry("delete",		TOKEN_KEYWORD_DELETE),
	TokenKeywordTableEntry("operator",		TOKEN_KEYWORD_OPERATOR),
	TokenKeywordTableEntry("private",		TOKEN_KEYWORD_PRIVATE),
	TokenKeywordTableEntry("ref",			TOKEN_KEYWORD_REF),
	TokenKeywordTableEntry("ptr",			TOKEN_KEYWORD_PTR),
	TokenKeywordTableEntry("sealed",		TOKEN_KEYWORD_SEALED),
	TokenKeywordTableEntry("signed",		TOKEN_KEYWORD_SIGNED),
	TokenKeywordTableEntry("unsigned",		TOKEN_KEYWORD_UNSIGNED),
	TokenKeywordTableEntry("protected",		TOKEN_KEYWORD_PROTECTED),
	TokenKeywordTableEntry("public",		TOKEN_KEYWORD_PUBLIC),
	TokenKeywordTableEntry("strict",		TOKEN_KEYWORD_STRUCT),
	TokenKeywordTableEntry("final",			TOKEN_KEYWORD_FINAL),
	TokenKeywordTableEntry("try",			TOKEN_KEYWORD_TRY),
	TokenKeywordTableEntry("catch",			TOKEN_KEYWORD_CATCH),
	TokenKeywordTableEntry("finally",		TOKEN_KEYWORD_FINALLY),
	TokenKeywordTableEntry("throw",			TOKEN_KEYWORD_THROW),
	TokenKeywordTableEntry("raise",			TOKEN_KEYWORD_RAISE),
	TokenKeywordTableEntry("literal",		TOKEN_KEYWORD_LITERAL),
	TokenKeywordTableEntry("if",			TOKEN_KEYWORD_IF),
	TokenKeywordTableEntry("else",			TOKEN_KEYWORD_ELSE),
	TokenKeywordTableEntry("end",			TOKEN_KEYWORD_END),
	TokenKeywordTableEntry("switch",		TOKEN_KEYWORD_SWITCH),
	TokenKeywordTableEntry("case",			TOKEN_KEYWORD_CASE),
	TokenKeywordTableEntry("default",		TOKEN_KEYWORD_DEFAULT),
	TokenKeywordTableEntry("break",			TOKEN_KEYWORD_BREAK),
	TokenKeywordTableEntry("continue",		TOKEN_KEYWORD_CONTINUE),
	TokenKeywordTableEntry("state",			TOKEN_KEYWORD_STATE),
	TokenKeywordTableEntry("gotostate",		TOKEN_KEYWORD_GOTOSTATE),
	TokenKeywordTableEntry("function",		TOKEN_KEYWORD_FUNCTION),
	TokenKeywordTableEntry("event",			TOKEN_KEYWORD_EVENT),
	TokenKeywordTableEntry("generator",		TOKEN_KEYWORD_GENERATOR),
	TokenKeywordTableEntry("native",		TOKEN_KEYWORD_NATIVE),
	TokenKeywordTableEntry("abstract",		TOKEN_KEYWORD_ABSTRACT),
	TokenKeywordTableEntry("delegate",		TOKEN_KEYWORD_DELEGATE),
	TokenKeywordTableEntry("return",		TOKEN_KEYWORD_RETURN),
	TokenKeywordTableEntry("generic",		TOKEN_KEYWORD_GENERIC),
	TokenKeywordTableEntry("interface",		TOKEN_KEYWORD_INTERFACE),
	TokenKeywordTableEntry("deprecated",	TOKEN_KEYWORD_DEPRECATED),
	TokenKeywordTableEntry("dynamic",		TOKEN_KEYWORD_DYNAMIC),
	TokenKeywordTableEntry("mutable",		TOKEN_KEYWORD_MUTABLE),
	TokenKeywordTableEntry("volatile",		TOKEN_KEYWORD_VOLATILE),
	TokenKeywordTableEntry("register",		TOKEN_KEYWORD_REGISTER),
	TokenKeywordTableEntry("yield",			TOKEN_KEYWORD_YIELD),
	TokenKeywordTableEntry("int",			TOKEN_KEYWORD_INT),
	TokenKeywordTableEntry("long",			TOKEN_KEYWORD_LONG),
	TokenKeywordTableEntry("double",		TOKEN_KEYWORD_DOUBLE),
	TokenKeywordTableEntry("float",			TOKEN_KEYWORD_FLOAT),
	TokenKeywordTableEntry("string",		TOKEN_KEYWORD_STRING),			
	TokenKeywordTableEntry("dict",			TOKEN_KEYWORD_DICT),
	TokenKeywordTableEntry("list",			TOKEN_KEYWORD_LIST),
	TokenKeywordTableEntry("void",			TOKEN_KEYWORD_VOID),
	TokenKeywordTableEntry("value",			TOKEN_KEYWORD_VALUE),
	TokenKeywordTableEntry("lock",			TOKEN_KEYWORD_LOCK),
	TokenKeywordTableEntry("jump",			TOKEN_KEYWORD_JUMP),
	TokenKeywordTableEntry("safe",			TOKEN_KEYWORD_SAFE),
	TokenKeywordTableEntry("unsafe",		TOKEN_KEYWORD_UNSAFE),
	TokenKeywordTableEntry("as",			TOKEN_KEYWORD_AS),
	TokenKeywordTableEntry("base",			TOKEN_KEYWORD_BASE),
	TokenKeywordTableEntry("fixed",			TOKEN_KEYWORD_FIXED),
	TokenKeywordTableEntry("out",			TOKEN_KEYWORD_OUT),
	TokenKeywordTableEntry("in",			TOKEN_KEYWORD_IN),
	TokenKeywordTableEntry("this",			TOKEN_KEYWORD_THIS),
	TokenKeywordTableEntry("self",			TOKEN_KEYWORD_SELF),
	TokenKeywordTableEntry("get",			TOKEN_KEYWORD_GET),
	TokenKeywordTableEntry("set",			TOKEN_KEYWORD_SET),
	TokenKeywordTableEntry("is",			TOKEN_KEYWORD_IS),

	TokenKeywordTableEntry("true",			TOKEN_LITERAL_TRUE),
	TokenKeywordTableEntry("false",			TOKEN_LITERAL_FALSE),
	TokenKeywordTableEntry("null",			TOKEN_LITERAL_NULL),

	// End of table.
	TokenKeywordTableEntry(NULL,			(TokenID)0)
};

CScriptLexer::CScriptLexer()
{
}

CScriptLexer::~CScriptLexer()
{
}

bool CScriptLexer::Analyze(CScriptCompileContext* context)
{
	_context = context;
	_position = 0;
	_line = 1;
	_column = 0;

	NextChar();

	while (!EndOfFile() && context->GetErrorCount(SCRIPT_ERROR_FATAL) <= 0)
	{
		CScriptToken token;
		if (ReadToken(token))
		{
			context->PushToken(token);
		}
	}

	context->_lastLineIndex  = _line;
	context->_lastLineColumn = _column; 

	return (context->GetErrorCount(SCRIPT_ERROR_FATAL) <= 0);
}

bool CScriptLexer::ReadToken(CScriptToken& token)
{
	u8 chr = CurrentChar();

	// Whietspace, ignore.
	if (chr <= 32)
	{
		NextChar();
		return false;
	}

	StartToken();

	switch (chr)
	{
		case '=':
			switch (NextChar())
			{
				case '=': NextChar(); token = MakeToken(TOKEN_OP_EQUAL, "==");  return true;
				default:			  token = MakeToken(TOKEN_OP_ASSIGN, "=");  return true;
			}

		case '^':
			switch (NextChar())
			{
				case '=': NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_BITWISE_XOR, "!=");  return true;
				default:			  token = MakeToken(TOKEN_OP_BITWISE_XOR, "^");			 return true;
			}
			
		case '~':
			switch (NextChar())
			{
				case '=': NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_BITWISE_NOT, "~=");  return true;
				default:			  token = MakeToken(TOKEN_OP_BITWISE_NOT, "~");			 return true;
			}

		case '%':
			switch (NextChar())
			{
				case '=': NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_MOD, "%=");	 return true;
				default:			  token = MakeToken(TOKEN_OP_MOD, "%");			 return true;
			}

		case '*':
			switch (NextChar())
			{
				case '=': NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_MUL, "*=");	 return true;
				default:			  token = MakeToken(TOKEN_OP_MUL, "*");			 return true;
			}

		case '!':
			switch (NextChar())
			{
				case '=': NextChar(); token = MakeToken(TOKEN_OP_NOT_EQUAL, "!=");		 return true;
				default:			  token = MakeToken(TOKEN_OP_LOGICAL_NOT, "!");		 return true;
			}

		case '|':
			switch (NextChar())
			{
				case '=':  NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_BITWISE_OR, "|=");		return true;
				case '|': NextChar(); token = MakeToken(TOKEN_OP_LOGICAL_OR, "||");			return true;
				default:	 		   token = MakeToken(TOKEN_OP_BITWISE_OR, "|");				return true;
			}
			
		case '&':
			switch (NextChar())
			{
				case '=':  NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_BITWISE_AND, "&=");	return true;
				case '&': NextChar(); token = MakeToken(TOKEN_OP_LOGICAL_AND, "&&");			return true;
				default:	 		   token = MakeToken(TOKEN_OP_BITWISE_AND, "&");		    return true;
			}
			
		case '<':
			switch (NextChar())
			{
				case '=':			NextChar(); token = MakeToken(TOKEN_OP_LESS_EQUAL, "<=");			return true;
				case '<': 
					switch (NextChar())
					{
						case '=':	NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_BITWISE_SHL, "<<=");	return true;
						default:	NextChar(); token = MakeToken(TOKEN_OP_BITWISE_SHL, "<<");			return true;
					}
					break;

				case '>':			NextChar();  token = MakeToken(TOKEN_OP_NOT_EQUAL, "<>");			return true;
				default:	 					 token = MakeToken(TOKEN_OP_LESS, "<");					return true;
			}
			
		case '>':
			switch (NextChar())
			{
				case '=':			NextChar(); token = MakeToken(TOKEN_OP_GREATER_EQUAL, ">=");		return true;
				case '>': 
					switch (NextChar())
					{
						case '=':	NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_BITWISE_SHR, ">>=");	return true;
						default:	NextChar(); token = MakeToken(TOKEN_OP_BITWISE_SHR, ">>");			return true;
					}
					break;

				case '<':			NextChar();  token = MakeToken(TOKEN_OP_NOT_EQUAL, "><");			return true;
				default:	 					 token = MakeToken(TOKEN_OP_GREATER, ">");				return true;
			}

		case '{':  NextChar(); token = MakeToken(TOKEN_OP_OPEN_BRACE, "{");		return true;
		case '}':  NextChar(); token = MakeToken(TOKEN_OP_CLOSE_BRACE, "}");	return true;
		case '[':  NextChar(); token = MakeToken(TOKEN_OP_OPEN_BRACKET, "[");	return true;
		case ']':  NextChar(); token = MakeToken(TOKEN_OP_CLOSE_BRACKET, "]");	return true;
		case '(':  NextChar(); token = MakeToken(TOKEN_OP_OPEN_PARENT, "(");	return true;
		case ')':  NextChar(); token = MakeToken(TOKEN_OP_CLOSE_PARENT, ")");	return true;
		case ',':  NextChar(); token = MakeToken(TOKEN_OP_COMMA, ",");			return true;
		case ':':  NextChar(); token = MakeToken(TOKEN_OP_COLON, ":");			return true;
		case ';':  NextChar(); token = MakeToken(TOKEN_OP_SEMICOLON, ";");		return true;
		case '?':  NextChar(); token = MakeToken(TOKEN_OP_QUESTION, "?");		return true;

		// Strings
		case '@':
		case '"':
		case '\'':
		{
			bool						noEscapes	= false;
			Engine::Containers::CString lit			= "";

			// Make sure a string start follows a @ symbol!
			if (CurrentChar() == '@')
			{
				NextChar();

				if (CurrentChar() == '"' || CurrentChar() == '\'')
				{
					noEscapes = true;				
				}
				else
				{
					token = MakeToken(TOKEN_OP_AT, "@");
					return true;
				}
			}

			// Start character!
			u32  openChar	  = CurrentChar();
			bool longOpenSeq  = false;

			// We a long sequence?
			if (LookAheadChar() == openChar && LookAheadChar(1) == openChar)
			{
				longOpenSeq = true;
				NextChar();
				NextChar();
			}

			// Start reading in the string.
			while (true)
			{
				u8 chr = NextChar();
				
				// End of the string yet.
				if (longOpenSeq == true)
				{
					if (openChar == chr && LookAheadChar() == openChar && LookAheadChar(1) == openChar)
					{
						NextChar();
						NextChar();
						break;
					}
				}
				else
				{
					if (openChar == chr)
					{
						break;
					}
				}

				// Escape sequence?
				if (noEscapes == false)
				{
					if (chr == '\\')
					{
						if (LookAheadChar() == openChar && longOpenSeq == false)
						{
							lit += NextChar();
							continue;
						}
						else
						{
							switch (NextChar())
							{
								case '0': lit += "\0"; break;
								case 'a': lit += "\a"; break;
								case 'b': lit += "\b"; break;
								case 'f': lit += "\f"; break;
								case 'n': lit += "\n"; break;
								case 'r': lit += "\r"; break;
								case 't': lit += "\t"; break;
								case 'v': lit += "\v"; break;
								case 'x':
								case 'X':
									
									u8 digit0 = NextChar();
									u8 digit1 = NextChar();
									_tokenStartColumn = _column - 1;

									if ((digit0 >= '0' && digit0 <= '9') ||
										(digit0 >= 'A' && digit0 <= 'F') ||
										(digit0 >= 'a' && digit0 <= 'f'))
									{
										_tokenStartColumn = _column;

										if ((digit1 >= '0' && digit1 <= '9') ||
											(digit1 >= 'A' && digit1 <= 'F') ||
											(digit1 >= 'a' && digit1 <= 'f'))
										{
											Engine::Containers::CString val = S("0x") + S(digit0) + digit1;
											u8 v = (u8)strtol(val.c_str(), NULL, 16);
											lit += v;

											continue;
										}

									}
									
									Error(S("Encountered unexpected hex character escape sequence '%c%c' (0x%x/0x%x).").Format(digit0, digit1, (u32)digit0, (u32)digit1));
									return false;
							}
							continue;
						}
					}
				}

				lit += chr;
			}

			token = MakeToken(TOKEN_LITERAL_STRING, lit);
			NextChar();
			return true;
		}

		// Comments.
		case '/':
			switch (NextChar())
			{
				// Single line comment.
				case '/': 
				{
					//Engine::Containers::CString comment;

					while (!EndOfFile())
					{
						u8 c = NextChar();
						if (c == '\n')
							break;
						//else
						//	comment += c;
					}

					return false;
				}

				// Multi-line comment.
				case '*':
				{
					s32 nested = 0;
					Engine::Containers::CString comment;

					while (true)
					{
						// End?
						if (EndOfFile())
						{					
							StartToken(); // Make sure the error message points to the correct place.
							Error(S("Encountered unexpected end of file, expecting end of block comment."));
							return false;
						}

						// Nested comments.
						u8 c = NextChar();
						if (PreviousChar() == '/' && c == '*')
						{
							nested++;
						}
						else if (c == '*' && LookAheadChar() == '/')
						{
							NextChar();
							nested--;

							if (nested < 0)
							{
								NextChar();
								break;
							}
						}

						comment += c;
					}

					return false;
				}
					
				// Divide / Divide Assign
				case '=': NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_DIV, "/="); return true;
				default:              token = MakeToken(TOKEN_OP_DIV, "/");         return true;
			}
			break;

		// Da fuck is this.
		default:

			// Number?
			if ((chr >= '0' && chr <= '9') || chr == '.' || chr == '-' || chr == '+')
			{
				return ReadNumber(chr, token);
			}

			// Identifier?
			else if ((chr >= 'A' && chr <= 'Z') ||
					 (chr >= 'a' && chr <= 'z') ||
					  chr == '_')
			{
				return ReadIdentifier(chr, token);
			}

			// ???
			else
			{
				Error(S("Encountered unexpected character '%c' (0x%x)").Format(chr, (u32)chr));
			}
	}

	return false;
}

void CScriptLexer::Error(const Engine::Containers::CString& str)
{
	_context->PushError(CScriptError(SCRIPT_ERROR_FATAL, 
									 str, 
									 _context->_initialFile, _tokenStartLine, _tokenStartColumn));
}

// Supports the following syntax's
//
//	Ocatal (side effect of strtol, not really an intentional part of the language, who uses octal? D:)
//		0113 
//	Hex:
//		0x2343AFCD
//		0X2343AFCD
//	Floats:
//		.123123
//		0.2134234
//		+.123123
//		-.123123
//		+0.2134234
//		-0.2134234
//	With Exponents:
//		3.16e-19
//		3.16e+19
//		6.02e23
//		1e-010
//		.e-010
//	Integers:
//		+234234234234
//		-234234234234
//		234234234234
//
bool CScriptLexer::ReadNumber(u8 startChar, CScriptToken& token)
{
	Engine::Containers::CString lit			= "";
	TokenID						id			= TOKEN_LITERAL_INT;
	bool						isHex		= false;
	bool						isFloat		= false;
	bool						foundRadix	= false;
	bool						foundExp	= false;
	u32							expPosition = 0;
	u32							offset		= 0;
	u32							numberCount = 0;

	StartToken();
	
	u8 chr = startChar;

	while (!EndOfFile())
	{
		// Hex prefix 0X or 0x
		if ((chr == 'x' || chr == 'X') && offset == 1 && startChar == '0')
		{
			isHex	 = true;		
			lit		+= chr;
		}
		
		// Floating point radix.
		else if (chr == '.' && isHex == false && foundRadix == false && foundExp == false)
		{
			isFloat		 = true;			
			foundRadix	 = true;
			lit			+= chr;
			id			= TOKEN_LITERAL_FLOAT;
		}	

		// Exponent
		else if (chr == 'e' && numberCount > 0 && isHex == false && foundExp == false)
		{
			isFloat		 = true;			
			foundExp 	 = true;
			expPosition  = offset;
			lit			+= chr;
		}	

		// Unary +/-
		else if ((chr == '-' || chr == '+') && (offset == 0 || (foundExp == true && expPosition == offset - 1)))
		{
			lit	+= chr;
		}		

		// Hex digit.
		else if (((chr >= 'A' && chr <= 'F') || (chr >= 'a' && chr <= 'f')) && isHex == true)
		{
			lit += chr;
			numberCount++;
		}

		// Standard digit.
		else if (chr >= '0' && chr <= '9')
		{
			lit += chr;
			numberCount++;
		}

		// lolwut
		else
		{
			break;
		}

		offset++;
		chr = NextChar();
	}

	// . - +
	// Parse sepcial unary operators.
	if (lit == ".")
	{
		token = MakeToken(TOKEN_OP_PERIOD, lit);
		return true;
	}
	else if (lit == "+")
	{			
		switch (CurrentChar())
		{
			case '=': NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_ADD, "+="); break;
			case '+': NextChar(); token = MakeToken(TOKEN_OP_INCREMENT,  "++"); break;
			default :			  token = MakeToken(TOKEN_OP_ADD,  "+"); break;
		}
		return true;
	}
	else if (lit == "-")
	{		
		switch (CurrentChar())
		{
			case '=': NextChar(); token = MakeToken(TOKEN_OP_ASSIGN_SUB, "-="); break;
			case '-': NextChar(); token = MakeToken(TOKEN_OP_DECREMENT,  "--"); break;
			default :			  token = MakeToken(TOKEN_OP_SUB,  "-"); break;
		}
		return true;
	}

	Engine::Containers::CString s = lit;

	// Convert any odd encodings (hex etc) into a numeric reference.
	if (isFloat == true)
		lit = (f32)strtod(lit.c_str(), NULL);
	else
		lit = strtol(lit.c_str(), NULL, isHex == true ? 16 : 10);

	token = MakeToken(id, lit);
	return true;
}

bool CScriptLexer::ReadIdentifier(u8 startChar, CScriptToken& token)
{
	Engine::Containers::CString lit			= "";
	TokenID						id			= TOKEN_LITERAL_IDENTIFIER;

	StartToken();
	
	u8 chr = startChar;

	while (!EndOfFile())
	{
		// Hex prefix 0X or 0x
		if ((chr >= 'A' && chr <= 'Z') ||
			(chr >= 'a' && chr <= 'z') ||
			(chr >= '0' && chr <= '9') ||
			chr == '_')
		{
			lit += chr;
		}
		else
		{
			break;
		}

		chr = NextChar();
	}

	// Convert to keywords.
	u32 index = 0;
	Engine::Containers::CString lower = lit.ToLower();
	while (TokenKeywordTable[index].Identifier != NULL)
	{
		if (strcmp(lower.c_str(), TokenKeywordTable[index].Identifier) == 0)
		{
			id = TokenKeywordTable[index++].ID;
			break;
		}
		index++;
	}

	// Convert true/false/null to integer tokens.
	if (id == TOKEN_LITERAL_TRUE)
	{
		lit = "1";
		id = TOKEN_LITERAL_INT;
	}
	else if (id == TOKEN_LITERAL_FALSE)
	{
		lit = "0";
		id = TOKEN_LITERAL_INT;
	}

	token = MakeToken(id, lit);
	return true;
}

void CScriptLexer::StartToken()
{
	_tokenStart       = _position - 1;
	_tokenStartLine   = _line;
	_tokenStartColumn = _column;
}

CScriptToken CScriptLexer::MakeToken(TokenID id, const Engine::Containers::CString& str)
{
	CScriptToken tok;
	tok.ID		= id;
	tok.Literal = str;
	tok.Line	= _tokenStartLine;
	tok.Column	= _tokenStartColumn;
	return tok;
}

bool CScriptLexer::EndOfFile(u32 offset)
{
	return _position + offset >= _context->_rawSource.Length();
}

u8 CScriptLexer::NextChar()
{
	if (EndOfFile())
		return '\0';

	u8 c = _context->_rawSource[_position];
	_position++;

	_column++;
	if (c == '\n')
	{
		_line++;
		_column = 0;
	}

	return c;
}

u8 CScriptLexer::CurrentChar()
{
	return _context->_rawSource[_position - 1];
}

u8 CScriptLexer::PreviousChar()
{
	return _context->_rawSource[_position - 2];
}

u8 CScriptLexer::LookAheadChar(u32 offset)
{
	if (EndOfFile(offset))
		return '\0';

	u8 c = _context->_rawSource[_position + offset];

	return c;
}

