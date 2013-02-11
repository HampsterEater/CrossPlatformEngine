/*
Original code by Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

//#include "tinyxml2.h"

#if 1
	#include <cstdio>
	#include <cstdlib>
	#include <new>
#else
	#include <string.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <ctype.h>
	#include <new>
	#include <stdarg.h>
#endif

using namespace Engine::FileSystem::Config;

static const char LINE_FEED				= (char)0x0a;			// all line endings are normalized to LF
static const char LF = LINE_FEED;
static const char CARRIAGE_RETURN		= (char)0x0d;			// CR gets filtered out
static const char CR = CARRIAGE_RETURN;
static const char SINGLE_QUOTE			= '\'';
static const char DOUBLE_QUOTE			= '\"';

// Bunch of unicode info at:
//		http://www.unicode.org/faq/utf_bom.html
//	ef bb bf (Microsoft "lead bytes") - designates UTF-8

static const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
static const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
static const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;


#define DELETE_NODE( node )	{			\
	if ( node ) {						\
		CXMLMemPool* pool = node->XMLMemPool;	\
		node->~CXMLNode();				\
		pool->Free( node );				\
	}									\
}
#define DELETE_ATTRIBUTE( attrib ) {		\
	if ( attrib ) {							\
		CXMLMemPool* pool = attrib->XMLMemPool;	\
		attrib->~CXMLAttribute();			\
		pool->Free( attrib );				\
	}										\
}

struct Entity {
	const char* pattern;
	int length;
	char value;
};

static const int NUM_ENTITIES = 5;
static const Entity entities[NUM_ENTITIES] = 
{
	{ "quot", 4,	DOUBLE_QUOTE },
	{ "amp", 3,		'&'  },
	{ "apos", 4,	SINGLE_QUOTE },
	{ "lt",	2, 		'<'	 },
	{ "gt",	2,		'>'	 }
};


Engine::FileSystem::Config::CXMLStrPair::~CXMLStrPair()
{
	Reset();
}


void Engine::FileSystem::Config::CXMLStrPair::Reset()
{
	if ( flags & NEEDS_DELETE ) {
		if (start != NULL)
			Engine::Memory::GetDefaultAllocator()->Free(&start);
		//delete [] start;
	}
	flags = 0;
	start = 0;
	end = 0;
}


void Engine::FileSystem::Config::CXMLStrPair::SetStr( const char* str, int flags )
{
	Reset();
	size_t len = strlen( str );
	start = (char*)Engine::Memory::GetDefaultAllocator()->Alloc(len + 1);//;new char[ len+1 ];
	memcpy( start, str, len+1 );
	end = start + len;
	this->flags = flags | NEEDS_DELETE;
}


char* Engine::FileSystem::Config::CXMLStrPair::ParseText( char* p, const char* endTag, int strFlags )
{
	TIXMLASSERT( endTag && *endTag );

	char* start = p;	// fixme: hides a member
	char  endChar = *endTag;
	int   length = strlen( endTag );	

	// Inner loop of text parsing.
	while ( *p ) {
		if ( *p == endChar && strncmp( p, endTag, length ) == 0 ) {
			Set( start, p, strFlags );
			return p + length;
		}
		++p;
	}	
	return 0;
}


char* Engine::FileSystem::Config::CXMLStrPair::ParseName( char* p )
{
	char* start = p;

	if ( !start || !(*start) ) {
		return 0;
	}

	if ( !CXMLUtil::IsAlpha( *p ) ) {
		return 0;
	}

	while( *p && (
			   CXMLUtil::IsAlphaNum( (unsigned char) *p ) 
			|| *p == '_'
			|| *p == '-'
			|| *p == '.'
			|| *p == ':' ))
	{
		++p;
	}

	if ( p > start ) {
		Set( start, p, 0 );
		return p;
	}
	return 0;
}



const char* Engine::FileSystem::Config::CXMLStrPair::GetStr()
{
	if ( flags & NEEDS_FLUSH ) {
		*end = 0;
		flags ^= NEEDS_FLUSH;

		if ( flags ) {
			char* p = start;	// the read pointer
			char* q = start;	// the write pointer

			while( p < end ) {
				if ( (flags & NEEDS_NEWLINE_NORMALIZATION) && *p == CR ) {
					// CR-LF pair becomes LF
					// CR alone becomes LF
					// LF-CR becomes LF
					if ( *(p+1) == LF ) {
						p += 2;
					}
					else {
						++p;
					}
					*q++ = LF;
				}
				else if ( (flags & NEEDS_NEWLINE_NORMALIZATION) && *p == LF ) {
					if ( *(p+1) == CR ) {
						p += 2;
					}
					else {
						++p;
					}
					*q++ = LF;
				}
				else if ( (flags & NEEDS_ENTITY_PROCESSING) && *p == '&' ) {
					int i=0;

					// Entities handled by tinyXML2:
					// - special entities in the entity table [in/out]
					// - numeric character reference [in]
					//   &#20013; or &#x4e2d;

					if ( *(p+1) == '#' ) {
						char buf[10] = { 0 };
						int len;
						p = const_cast<char*>( CXMLUtil::GetCharacterRef( p, buf, &len ) );
						for( int i=0; i<len; ++i ) {
							*q++ = buf[i];
						}
						TIXMLASSERT( q <= p );
					}
					else {
						for( i=0; i<NUM_ENTITIES; ++i ) {
							if (    strncmp( p+1, entities[i].pattern, entities[i].length ) == 0
								 && *(p+entities[i].length+1) == ';' ) 
							{
								// Found an entity convert;
								*q = entities[i].value;
								++q;
								p += entities[i].length + 2;
								break;
							}
						}
						if ( i == NUM_ENTITIES ) {
							// fixme: treat as error?
							++p;
							++q;
						}
					}
				}
				else {
					*q = *p;
					++p;
					++q;
				}
			}
			*q = 0;
		}
		flags = (flags & NEEDS_DELETE);
	}
	return start;
}




// --------- CXMLUtil ----------- //

const char* Engine::FileSystem::Config::CXMLUtil::ReadBOM( const char* p, bool* bom )
{
	*bom = false;
	const unsigned char* pu = reinterpret_cast<const unsigned char*>(p);
	// Check for BOM:
	if (    *(pu+0) == TIXML_UTF_LEAD_0 
		 && *(pu+1) == TIXML_UTF_LEAD_1
		 && *(pu+2) == TIXML_UTF_LEAD_2 ) 
	{
		*bom = true;
		p += 3;
	}
	return p;
}


void Engine::FileSystem::Config::CXMLUtil::ConvertUTF32ToUTF8( unsigned long input, char* output, int* length )
{
	const unsigned long BYTE_MASK = 0xBF;
	const unsigned long BYTE_MARK = 0x80;
	const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

	if (input < 0x80) 
		*length = 1;
	else if ( input < 0x800 )
		*length = 2;
	else if ( input < 0x10000 )
		*length = 3;
	else if ( input < 0x200000 )
		*length = 4;
	else
		{ *length = 0; return; }	// This code won't covert this correctly anyway.

	output += *length;

	// Scary scary fall throughs.
	switch (*length) 
	{
		case 4:
			--output; 
			*output = (char)((input | BYTE_MARK) & BYTE_MASK); 
			input >>= 6;
		case 3:
			--output; 
			*output = (char)((input | BYTE_MARK) & BYTE_MASK); 
			input >>= 6;
		case 2:
			--output; 
			*output = (char)((input | BYTE_MARK) & BYTE_MASK); 
			input >>= 6;
		case 1:
			--output; 
			*output = (char)(input | FIRST_BYTE_MARK[*length]);
	}
}


const char* Engine::FileSystem::Config::CXMLUtil::GetCharacterRef( const char* p, char* value, int* length )
{
	// Presume an entity, and pull it out.
	*length = 0;

	if ( *(p+1) == '#' && *(p+2) )
	{
		unsigned long ucs = 0;
		int delta = 0;
		unsigned mult = 1;

		if ( *(p+2) == 'x' )
		{
			// Hexadecimal.
			if ( !*(p+3) ) return 0;

			const char* q = p+3;
			q = strchr( q, ';' );

			if ( !q || !*q ) return 0;

			delta = (q-p);
			--q;

			while ( *q != 'x' )
			{
				if ( *q >= '0' && *q <= '9' )
					ucs += mult * (*q - '0');
				else if ( *q >= 'a' && *q <= 'f' )
					ucs += mult * (*q - 'a' + 10);
				else if ( *q >= 'A' && *q <= 'F' )
					ucs += mult * (*q - 'A' + 10 );
				else 
					return 0;
				mult *= 16;
				--q;
			}
		}
		else
		{
			// Decimal.
			if ( !*(p+2) ) return 0;

			const char* q = p+2;
			q = strchr( q, ';' );

			if ( !q || !*q ) return 0;

			delta = q-p;
			--q;

			while ( *q != '#' )
			{
				if ( *q >= '0' && *q <= '9' )
					ucs += mult * (*q - '0');
				else 
					return 0;
				mult *= 10;
				--q;
			}
		}
		// convert the UCS to UTF-8
		ConvertUTF32ToUTF8( ucs, value, length );
		return p + delta + 1;
	}
	return p+1;
}


char* Engine::FileSystem::Config::CXMLDocument::Identify( char* p, CXMLNode** node ) 
{
	CXMLNode* returnNode = 0;
	char* start = p;
	p = CXMLUtil::SkipWhiteSpace( p );
	if( !p || !*p )
	{
		return p;
	}

	// What is this thing? 
	// - Elements start with a letter or underscore, but xml is reserved.
	// - Comments: <!--
	// - Decleration: <?
	// - Everthing else is unknown to tinyxml.
	//

	static const char* xmlHeader		= { "<?" };
	static const char* commentHeader	= { "<!--" };
	static const char* dtdHeader		= { "<!" };
	static const char* cdataHeader		= { "<![CDATA[" };
	static const char* elementHeader	= { "<" };	// and a header for everything else; check last.

	static const int xmlHeaderLen		= 2;
	static const int commentHeaderLen	= 4;
	static const int dtdHeaderLen		= 2;
	static const int cdataHeaderLen		= 9;
	static const int elementHeaderLen	= 1;

#if defined(_MSC_VER)
#pragma warning ( push )
#pragma warning ( disable : 4127 )
#endif
	TIXMLASSERT( sizeof( CXMLComment ) == sizeof( CXMLUnknown ) );		// use same memory pool
	TIXMLASSERT( sizeof( CXMLComment ) == sizeof( CXMLDeclaration ) );	// use same memory pool
#if defined(_MSC_VER)
#pragma warning (pop)
#endif
	if ( CXMLUtil::StringEqual( p, xmlHeader, xmlHeaderLen ) ) {
		returnNode = new (commentPool.Alloc()) CXMLDeclaration( this );
		returnNode->XMLMemPool = &commentPool;
		p += xmlHeaderLen;
	}
	else if ( CXMLUtil::StringEqual( p, commentHeader, commentHeaderLen ) ) {
		returnNode = new (commentPool.Alloc()) CXMLComment( this );
		returnNode->XMLMemPool = &commentPool;
		p += commentHeaderLen;
	}
	else if ( CXMLUtil::StringEqual( p, cdataHeader, cdataHeaderLen ) ) {
		CXMLText* text = new (textPool.Alloc()) CXMLText( this );
		returnNode = text;
		returnNode->XMLMemPool = &textPool;
		p += cdataHeaderLen;
		text->SetCData( true );
	}
	else if ( CXMLUtil::StringEqual( p, dtdHeader, dtdHeaderLen ) ) {
		returnNode = new (commentPool.Alloc()) CXMLUnknown( this );
		returnNode->XMLMemPool = &commentPool;
		p += dtdHeaderLen;
	}
	else if ( CXMLUtil::StringEqual( p, elementHeader, elementHeaderLen ) ) {
		returnNode = new (elementPool.Alloc()) CXMLElement( this );
		returnNode->XMLMemPool = &elementPool;
		p += elementHeaderLen;
	}
	else {
		returnNode = new (textPool.Alloc()) CXMLText( this );
		returnNode->XMLMemPool = &textPool;
		p = start;	// Back it up, all the text counts.
	}

	*node = returnNode;
	return p;
}


bool Engine::FileSystem::Config::CXMLDocument::Accept( CXMLVisitor* visitor ) const
{
	if ( visitor->VisitEnter( *this ) )
	{
		for ( const CXMLNode* node=FirstChild(); node; node=node->NextSibling() )
		{
			if ( !node->Accept( visitor ) )
				break;
		}
	}
	return visitor->VisitExit( *this );
}


// --------- CXMLNode ----------- //

Engine::FileSystem::Config::CXMLNode::CXMLNode( CXMLDocument* doc ) :
	document( doc ),
	parent( 0 ),
	firstChild( 0 ), lastChild( 0 ),
	prev( 0 ), next( 0 )
{
}


Engine::FileSystem::Config::CXMLNode::~CXMLNode()
{
	DeleteChildren();
	if ( parent ) {
		parent->Unlink( this );
	}
}


void Engine::FileSystem::Config::CXMLNode::SetValue( const char* str, bool staticMem )
{
	if ( staticMem )
		value.SetInternedStr( str );
	else
		value.SetStr( str );
}


void Engine::FileSystem::Config::CXMLNode::DeleteChildren()
{
	while( firstChild ) {
		CXMLNode* node = firstChild;
		Unlink( node );
		
		DELETE_NODE( node );
	}
	firstChild = lastChild = 0;
}


void Engine::FileSystem::Config::CXMLNode::Unlink( CXMLNode* child )
{
	TIXMLASSERT( child->parent == this );
	if ( child == firstChild ) 
		firstChild = firstChild->next;
	if ( child == lastChild ) 
		lastChild = lastChild->prev;

	if ( child->prev ) {
		child->prev->next = child->next;
	}
	if ( child->next ) {
		child->next->prev = child->prev;
	}
	child->parent = 0;
}


void Engine::FileSystem::Config::CXMLNode::DeleteChild( CXMLNode* node )
{
	TIXMLASSERT( node->parent == this );
	DELETE_NODE( node );
}


CXMLNode* Engine::FileSystem::Config::CXMLNode::InsertEndChild( CXMLNode* addThis )
{
	if ( lastChild ) {
		TIXMLASSERT( firstChild );
		TIXMLASSERT( lastChild->next == 0 );
		lastChild->next = addThis;
		addThis->prev = lastChild;
		lastChild = addThis;

		addThis->next = 0;
	}
	else {
		TIXMLASSERT( firstChild == 0 );
		firstChild = lastChild = addThis;

		addThis->prev = 0;
		addThis->next = 0;
	}
	addThis->parent = this;
	return addThis;
}


CXMLNode* Engine::FileSystem::Config::CXMLNode::InsertFirstChild( CXMLNode* addThis )
{
	if ( firstChild ) {
		TIXMLASSERT( lastChild );
		TIXMLASSERT( firstChild->prev == 0 );

		firstChild->prev = addThis;
		addThis->next = firstChild;
		firstChild = addThis;

		addThis->prev = 0;
	}
	else {
		TIXMLASSERT( lastChild == 0 );
		firstChild = lastChild = addThis;

		addThis->prev = 0;
		addThis->next = 0;
	}
	addThis->parent = this;
	return addThis;
}


CXMLNode* Engine::FileSystem::Config::CXMLNode::InsertAfterChild( CXMLNode* afterThis, CXMLNode* addThis )
{
	TIXMLASSERT( afterThis->parent == this );
	if ( afterThis->parent != this )
		return 0;

	if ( afterThis->next == 0 ) {
		// The last node or the only node.
		return InsertEndChild( addThis );
	}
	addThis->prev = afterThis;
	addThis->next = afterThis->next;
	afterThis->next->prev = addThis;
	afterThis->next = addThis;
	addThis->parent = this;
	return addThis;
}




const CXMLElement* Engine::FileSystem::Config::CXMLNode::FirstChildElement( const char* value ) const
{
	for( CXMLNode* node=firstChild; node; node=node->next ) {
		CXMLElement* element = node->ToElement();
		if ( element ) {
			if ( !value || CXMLUtil::StringEqual( element->Name(), value ) ) {
				return element;
			}
		}
	}
	return 0;
}


const CXMLElement* Engine::FileSystem::Config::CXMLNode::LastChildElement( const char* value ) const
{
	for( CXMLNode* node=lastChild; node; node=node->prev ) {
		CXMLElement* element = node->ToElement();
		if ( element ) {
			if ( !value || CXMLUtil::StringEqual( element->Name(), value ) ) {
				return element;
			}
		}
	}
	return 0;
}


const CXMLElement* Engine::FileSystem::Config::CXMLNode::NextSiblingElement( const char* value ) const
{
	for( CXMLNode* element=this->next; element; element = element->next ) {
		if (    element->ToElement() 
			 && (!value || CXMLUtil::StringEqual( value, element->Value() ))) 
		{
			return element->ToElement();
		}
	}
	return 0;
}


const CXMLElement* Engine::FileSystem::Config::CXMLNode::PreviousSiblingElement( const char* value ) const
{
	for( CXMLNode* element=this->prev; element; element = element->prev ) {
		if (    element->ToElement()
			 && (!value || CXMLUtil::StringEqual( value, element->Value() ))) 
		{
			return element->ToElement();
		}
	}
	return 0;
}


char* Engine::FileSystem::Config::CXMLNode::ParseDeep( char* p, CXMLStrPair* parentEnd )
{
	// This is a recursive method, but thinking about it "at the current level"
	// it is a pretty simple flat list:
	//		<foo/>
	//		<!-- comment -->
	//
	// With a special case:
	//		<foo>
	//		</foo>
	//		<!-- comment -->
	//		
	// Where the closing element (/foo) *must* be the next thing after the opening
	// element, and the names must match. BUT the tricky bit is that the closing
	// element will be read by the child.
	// 
	// 'endTag' is the end tag for this node, it is returned by a call to a child.
	// 'parentEnd' is the end tag for the parent, which is filled in and returned.

	while( p && *p ) {
		CXMLNode* node = 0;

		p = document->Identify( p, &node );
		if ( p == 0 || node == 0 ) {
			break;
		}

		CXMLStrPair endTag;
		p = node->ParseDeep( p, &endTag );
		if ( !p ) {
			DELETE_NODE( node );
			node = 0;
			if ( !document->Error() ) {
				document->SetError( XML_ERROR_PARSING, 0, 0 );
			}
			break;
		}

		// We read the end tag. Return it to the parent.
		if ( node->ToElement() && node->ToElement()->ClosingType() == CXMLElement::CLOSING ) {
			if ( parentEnd ) {
				*parentEnd = ((CXMLElement*)node)->value;
			}
			DELETE_NODE( node );
			return p;
		}

		// Handle an end tag returned to this level.
		// And handle a bunch of annoying errors.
		CXMLElement* ele = node->ToElement();
		if ( ele ) {
			if ( endTag.Empty() && ele->ClosingType() == CXMLElement::OPEN ) {
				document->SetError( XML_ERROR_MISMATCHED_ELEMENT, node->Value(), 0 );
				p = 0;
			}
			else if ( !endTag.Empty() && ele->ClosingType() != CXMLElement::OPEN ) {
				document->SetError( XML_ERROR_MISMATCHED_ELEMENT, node->Value(), 0 );
				p = 0;
			}
			else if ( !endTag.Empty() ) {
				if ( !CXMLUtil::StringEqual( endTag.GetStr(), node->Value() )) { 
					document->SetError( XML_ERROR_MISMATCHED_ELEMENT, node->Value(), 0 );
					p = 0;
				}
			}
		}
		if ( p == 0 ) {
			DELETE_NODE( node );
			node = 0;
		}
		if ( node ) {
			this->InsertEndChild( node );
		}
	}
	return 0;
}

// --------- CXMLText ---------- //
char* Engine::FileSystem::Config::CXMLText::ParseDeep( char* p, CXMLStrPair* )
{
	const char* start = p;
	if ( this->CData() ) {
		p = value.ParseText( p, "]]>", CXMLStrPair::NEEDS_NEWLINE_NORMALIZATION );
		if ( !p ) {
			document->SetError( XML_ERROR_PARSING_CDATA, start, 0 );
		}
		return p;
	}
	else {
		p = value.ParseText( p, "<", document->ProcessEntities() ? CXMLStrPair::TEXT_ELEMENT : CXMLStrPair::TEXT_ELEMENT_LEAVE_ENTITIES );
		if ( !p ) {
			document->SetError( XML_ERROR_PARSING_TEXT, start, 0 );
		}
		if ( p && *p ) {
			return p-1;
		}
	}
	return 0;
}


CXMLNode* Engine::FileSystem::Config::CXMLText::ShallowClone( CXMLDocument* doc ) const
{
	if ( !doc ) {
		doc = document;
	}
	CXMLText* text = doc->NewText( Value() );	// fixme: this will always allocate memory. Intern?
	text->SetCData( this->CData() );
	return text;
}


bool Engine::FileSystem::Config::CXMLText::ShallowEqual( const CXMLNode* compare ) const
{
	return ( compare->ToText() && CXMLUtil::StringEqual( compare->ToText()->Value(), Value() ));
}


bool Engine::FileSystem::Config::CXMLText::Accept( CXMLVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


// --------- CXMLComment ---------- //

Engine::FileSystem::Config::CXMLComment::CXMLComment( CXMLDocument* doc ) : CXMLNode( doc )
{
}


Engine::FileSystem::Config::CXMLComment::~CXMLComment()
{
	//printf( "~CXMLComment\n" );
}


char* Engine::FileSystem::Config::CXMLComment::ParseDeep( char* p, CXMLStrPair* )
{
	// Comment parses as text.
	const char* start = p;
	p = value.ParseText( p, "-->", CXMLStrPair::COMMENT );
	if ( p == 0 ) {
		document->SetError( XML_ERROR_PARSING_COMMENT, start, 0 );
	}
	return p;
}


CXMLNode* Engine::FileSystem::Config::CXMLComment::ShallowClone( CXMLDocument* doc ) const
{
	if ( !doc ) {
		doc = document;
	}
	CXMLComment* comment = doc->NewComment( Value() );	// fixme: this will always allocate memory. Intern?
	return comment;
}


bool Engine::FileSystem::Config::CXMLComment::ShallowEqual( const CXMLNode* compare ) const
{
	return ( compare->ToComment() && CXMLUtil::StringEqual( compare->ToComment()->Value(), Value() ));
}


bool Engine::FileSystem::Config::CXMLComment::Accept( CXMLVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


// --------- CXMLDeclaration ---------- //

Engine::FileSystem::Config::CXMLDeclaration::CXMLDeclaration( CXMLDocument* doc ) : CXMLNode( doc )
{
}


Engine::FileSystem::Config::CXMLDeclaration::~CXMLDeclaration()
{
	//printf( "~CXMLDeclaration\n" );
}


char* Engine::FileSystem::Config::CXMLDeclaration::ParseDeep( char* p, CXMLStrPair* )
{
	// Declaration parses as text.
	const char* start = p;
	p = value.ParseText( p, "?>", CXMLStrPair::NEEDS_NEWLINE_NORMALIZATION );
	if ( p == 0 ) {
		document->SetError( XML_ERROR_PARSING_DECLARATION, start, 0 );
	}
	return p;
}


CXMLNode* Engine::FileSystem::Config::CXMLDeclaration::ShallowClone( CXMLDocument* doc ) const
{
	if ( !doc ) {
		doc = document;
	}
	CXMLDeclaration* dec = doc->NewDeclaration( Value() );	// fixme: this will always allocate memory. Intern?
	return dec;
}


bool Engine::FileSystem::Config::CXMLDeclaration::ShallowEqual( const CXMLNode* compare ) const
{
	return ( compare->ToDeclaration() && CXMLUtil::StringEqual( compare->ToDeclaration()->Value(), Value() ));
}



bool Engine::FileSystem::Config::CXMLDeclaration::Accept( CXMLVisitor* visitor ) const
{
	return visitor->Visit( *this );
}

// --------- CXMLUnknown ---------- //

Engine::FileSystem::Config::CXMLUnknown::CXMLUnknown( CXMLDocument* doc ) : CXMLNode( doc )
{
}


Engine::FileSystem::Config::CXMLUnknown::~CXMLUnknown()
{
}


char* Engine::FileSystem::Config::CXMLUnknown::ParseDeep( char* p, CXMLStrPair* )
{
	// Unknown parses as text.
	const char* start = p;

	p = value.ParseText( p, ">", CXMLStrPair::NEEDS_NEWLINE_NORMALIZATION );
	if ( !p ) {
		document->SetError( XML_ERROR_PARSING_UNKNOWN, start, 0 );
	}
	return p;
}


CXMLNode* Engine::FileSystem::Config::CXMLUnknown::ShallowClone( CXMLDocument* doc ) const
{
	if ( !doc ) {
		doc = document;
	}
	CXMLUnknown* text = doc->NewUnknown( Value() );	// fixme: this will always allocate memory. Intern?
	return text;
}


bool Engine::FileSystem::Config::CXMLUnknown::ShallowEqual( const CXMLNode* compare ) const
{
	return ( compare->ToUnknown() && CXMLUtil::StringEqual( compare->ToUnknown()->Value(), Value() ));
}


bool Engine::FileSystem::Config::CXMLUnknown::Accept( CXMLVisitor* visitor ) const
{
	return visitor->Visit( *this );
}

// --------- CXMLAttribute ---------- //
char* Engine::FileSystem::Config::CXMLAttribute::ParseDeep( char* p, bool processEntities )
{
	p = name.ParseText( p, "=", CXMLStrPair::ATTRIBUTE_NAME );
	if ( !p || !*p ) return 0;

	char endTag[2] = { *p, 0 };
	++p;
	p = value.ParseText( p, endTag, processEntities ? CXMLStrPair::ATTRIBUTE_VALUE : CXMLStrPair::ATTRIBUTE_VALUE_LEAVE_ENTITIES );
	//if ( value.Empty() ) return 0;
	return p;
}


void Engine::FileSystem::Config::CXMLAttribute::SetName( const char* n )
{
	name.SetStr( n );
}


int Engine::FileSystem::Config::CXMLAttribute::QueryIntValue( int* value ) const
{
	if ( TIXML_SSCANF( Value(), "%d", value ) == 1 )
		return XML_NO_ERROR;
	return XML_WRONG_ATTRIBUTE_TYPE;
}


int Engine::FileSystem::Config::CXMLAttribute::QueryUnsignedValue( unsigned int* value ) const
{
	if ( TIXML_SSCANF( Value(), "%u", value ) == 1 )
		return XML_NO_ERROR;
	return XML_WRONG_ATTRIBUTE_TYPE;
}


int Engine::FileSystem::Config::CXMLAttribute::QueryBoolValue( bool* value ) const
{
	int ival = -1;
	QueryIntValue( &ival );

	if ( ival > 0 || CXMLUtil::StringEqual( Value(), "true" ) ) {
		*value = true;
		return XML_NO_ERROR;
	}
	else if ( ival == 0 || CXMLUtil::StringEqual( Value(), "false" ) ) {
		*value = false;
		return XML_NO_ERROR;
	}
	return XML_WRONG_ATTRIBUTE_TYPE;
}


int Engine::FileSystem::Config::CXMLAttribute::QueryDoubleValue( double* value ) const
{
	if ( TIXML_SSCANF( Value(), "%lf", value ) == 1 )
		return XML_NO_ERROR;
	return XML_WRONG_ATTRIBUTE_TYPE;
}


int Engine::FileSystem::Config::CXMLAttribute::QueryFloatValue( float* value ) const
{
	if ( TIXML_SSCANF( Value(), "%f", value ) == 1 )
		return XML_NO_ERROR;
	return XML_WRONG_ATTRIBUTE_TYPE;
}


void Engine::FileSystem::Config::CXMLAttribute::SetAttribute( const char* v )
{
	value.SetStr( v );
}


void Engine::FileSystem::Config::CXMLAttribute::SetAttribute( int v )
{
	char buf[BUF_SIZE];
	TIXML_SNPRINTF( buf, BUF_SIZE, "%d", v );	
	value.SetStr( buf );
}


void Engine::FileSystem::Config::CXMLAttribute::SetAttribute( unsigned v )
{
	char buf[BUF_SIZE];
	TIXML_SNPRINTF( buf, BUF_SIZE, "%u", v );	
	value.SetStr( buf );
}


void Engine::FileSystem::Config::CXMLAttribute::SetAttribute( bool v )
{
	char buf[BUF_SIZE];
	TIXML_SNPRINTF( buf, BUF_SIZE, "%d", v ? 1 : 0 );	
	value.SetStr( buf );
}

void Engine::FileSystem::Config::CXMLAttribute::SetAttribute( double v )
{
	char buf[BUF_SIZE];
	TIXML_SNPRINTF( buf, BUF_SIZE, "%f", v );	
	value.SetStr( buf );
}

void Engine::FileSystem::Config::CXMLAttribute::SetAttribute( float v )
{
	char buf[BUF_SIZE];
	TIXML_SNPRINTF( buf, BUF_SIZE, "%f", v );	
	value.SetStr( buf );
}


// --------- CXMLElement ---------- //
Engine::FileSystem::Config::CXMLElement::CXMLElement( CXMLDocument* doc ) : CXMLNode( doc ),
	closingType( 0 ),
	rootAttribute( 0 )
{
}


Engine::FileSystem::Config::CXMLElement::~CXMLElement()
{
	while( rootAttribute ) {
		CXMLAttribute* next = rootAttribute->next;
		DELETE_ATTRIBUTE( rootAttribute );
		rootAttribute = next;
	}
}


CXMLAttribute* Engine::FileSystem::Config::CXMLElement::FindAttribute( const char* name )
{
	CXMLAttribute* a = 0;
	for( a=rootAttribute; a; a = a->next ) {
		if ( CXMLUtil::StringEqual( a->Name(), name ) )
			return a;
	}
	return 0;
}


const CXMLAttribute* Engine::FileSystem::Config::CXMLElement::FindAttribute( const char* name ) const
{
	CXMLAttribute* a = 0;
	for( a=rootAttribute; a; a = a->next ) {
		if ( CXMLUtil::StringEqual( a->Name(), name ) )
			return a;
	}
	return 0;
}


const char* Engine::FileSystem::Config::CXMLElement::Attribute( const char* name, const char* value ) const
{ 
	const CXMLAttribute* a = FindAttribute( name ); 
	if ( !a ) 
		return 0; 
	if ( !value || CXMLUtil::StringEqual( a->Value(), value ))
		return a->Value();
	return 0;
}


const char* Engine::FileSystem::Config::CXMLElement::GetText() const
{
	if ( FirstChild() && FirstChild()->ToText() ) {
		return FirstChild()->ToText()->Value();
	}
	return 0;
}


CXMLAttribute* Engine::FileSystem::Config::CXMLElement::FindOrCreateAttribute( const char* name )
{
	CXMLAttribute* last = 0;
	CXMLAttribute* attrib = 0;
	for( attrib = rootAttribute;
		 attrib;
		 last = attrib, attrib = attrib->next )
	{		 
		if ( CXMLUtil::StringEqual( attrib->Name(), name ) ) {
			break;
		}
	}
	if ( !attrib ) {
		attrib = new (document->attributePool.Alloc() ) CXMLAttribute();
		attrib->XMLMemPool = &document->attributePool;
		if ( last ) {
			last->next = attrib;
		}
		else {
			rootAttribute = attrib;
		}
		attrib->SetName( name );
	}
	return attrib;
}


void Engine::FileSystem::Config::CXMLElement::DeleteAttribute( const char* name )
{
	CXMLAttribute* prev = 0;
	for( CXMLAttribute* a=rootAttribute; a; a=a->next ) {
		if ( CXMLUtil::StringEqual( name, a->Name() ) ) {
			if ( prev ) {
				prev->next = a->next;
			}
			else {
				rootAttribute = a->next;
			}
			DELETE_ATTRIBUTE( a );
			break;
		}
		prev = a;
	}
}


char* Engine::FileSystem::Config::CXMLElement::ParseAttributes( char* p )
{
	const char* start = p;
	CXMLAttribute* prevAttribute = 0;

	// Read the attributes.
	while( p ) {
		p = CXMLUtil::SkipWhiteSpace( p );
		if ( !p || !(*p) ) {
			document->SetError( XML_ERROR_PARSING_ELEMENT, start, Name() );
			return 0;
		}

		// attribute.
		if ( CXMLUtil::IsAlpha( *p ) ) {
			CXMLAttribute* attrib = new (document->attributePool.Alloc() ) CXMLAttribute();
			attrib->XMLMemPool = &document->attributePool;

			p = attrib->ParseDeep( p, document->ProcessEntities() );
			if ( !p || Attribute( attrib->Name() ) ) {
				DELETE_ATTRIBUTE( attrib );
				document->SetError( XML_ERROR_PARSING_ATTRIBUTE, start, p );
				return 0;
			}
			// There is a minor bug here: if the attribute in the source xml
			// document is duplicated, it will not be detected and the
			// attribute will be doubly added. However, tracking the 'prevAttribute'
			// avoids re-scanning the attribute list. Preferring performance for
			// now, may reconsider in the future.
			if ( prevAttribute ) { 
				prevAttribute->next = attrib;
			}
			else {
				rootAttribute = attrib;
			}	
			prevAttribute = attrib;
		}
		// end of the tag
		else if ( *p == '/' && *(p+1) == '>' ) {
			closingType = CLOSED;
			return p+2;	// done; sealed element.
		}
		// end of the tag
		else if ( *p == '>' ) {
			++p;
			break;
		}
		else {
			document->SetError( XML_ERROR_PARSING_ELEMENT, start, p );
			return 0;
		}
	}
	return p;
}


//
//	<ele></ele>
//	<ele>foo<b>bar</b></ele>
//
char* Engine::FileSystem::Config::CXMLElement::ParseDeep( char* p, CXMLStrPair* CXMLStrPair )
{
	// Read the element name.
	p = CXMLUtil::SkipWhiteSpace( p );
	if ( !p ) return 0;

	// The closing element is the </element> form. It is
	// parsed just like a regular element then deleted from
	// the DOM.
	if ( *p == '/' ) {
		closingType = CLOSING;
		++p;
	}

	p = value.ParseName( p );
	if ( value.Empty() ) return 0;

	p = ParseAttributes( p );
	if ( !p || !*p || closingType ) 
		return p;

	p = CXMLNode::ParseDeep( p, CXMLStrPair );
	return p;
}



CXMLNode* Engine::FileSystem::Config::CXMLElement::ShallowClone( CXMLDocument* doc ) const
{
	if ( !doc ) {
		doc = document;
	}
	CXMLElement* element = doc->NewElement( Value() );					// fixme: this will always allocate memory. Intern?
	for( const CXMLAttribute* a=FirstAttribute(); a; a=a->Next() ) {
		element->SetAttribute( a->Name(), a->Value() );					// fixme: this will always allocate memory. Intern?
	}
	return element;
}


bool Engine::FileSystem::Config::CXMLElement::ShallowEqual( const CXMLNode* compare ) const
{
	const CXMLElement* other = compare->ToElement();
	if ( other && CXMLUtil::StringEqual( other->Value(), Value() )) {

		const CXMLAttribute* a=FirstAttribute();
		const CXMLAttribute* b=other->FirstAttribute();

		while ( a && b ) {
			if ( !CXMLUtil::StringEqual( a->Value(), b->Value() ) ) {
				return false;
			}
		}	
		if ( a || b ) {
			// different count
			return false;
		}
		return true;
	}
	return false;
}


bool Engine::FileSystem::Config::CXMLElement::Accept( CXMLVisitor* visitor ) const
{
	if ( visitor->VisitEnter( *this, rootAttribute ) ) 
	{
		for ( const CXMLNode* node=FirstChild(); node; node=node->NextSibling() )
		{
			if ( !node->Accept( visitor ) )
				break;
		}
	}
	return visitor->VisitExit( *this );
}


// --------- CXMLDocument ----------- //
Engine::FileSystem::Config::CXMLDocument::CXMLDocument( bool _processEntities ) :
	CXMLNode( 0 ),
	writeBOM( false ),
	processEntities( _processEntities ),
	errorID( 0 ),
	errorStr1( 0 ),
	errorStr2( 0 ),
	charBuffer( 0 )
{
	document = this;	// avoid warning about 'this' in initializer list
}


Engine::FileSystem::Config::CXMLDocument::~CXMLDocument()
{
	DeleteChildren();
	//delete [] charBuffer;
	if (charBuffer != NULL)
		Engine::Memory::GetDefaultAllocator()->Free(&charBuffer);

#if 0
	textPool.Trace( "text" );
	elementPool.Trace( "element" );
	commentPool.Trace( "comment" );
	attributePool.Trace( "attribute" );
#endif

	TIXMLASSERT( textPool.CurrentAllocs() == 0 );
	TIXMLASSERT( elementPool.CurrentAllocs() == 0 );
	TIXMLASSERT( commentPool.CurrentAllocs() == 0 );
	TIXMLASSERT( attributePool.CurrentAllocs() == 0 );
}


void Engine::FileSystem::Config::CXMLDocument::InitDocument()
{
	errorID = XML_NO_ERROR;
	errorStr1 = 0;
	errorStr2 = 0;

	//delete [] charBuffer;
	if (charBuffer != NULL)
	{
		Engine::Memory::GetDefaultAllocator()->Free(&charBuffer);
		charBuffer = 0;
	}
}


CXMLElement* Engine::FileSystem::Config::CXMLDocument::NewElement( const char* name )
{
	CXMLElement* ele = new (elementPool.Alloc()) CXMLElement( this );
	ele->XMLMemPool = &elementPool;
	ele->SetName( name );
	return ele;
}


CXMLComment* Engine::FileSystem::Config::CXMLDocument::NewComment( const char* str )
{
	CXMLComment* comment = new (commentPool.Alloc()) CXMLComment( this );
	comment->XMLMemPool = &commentPool;
	comment->SetValue( str );
	return comment;
}


CXMLText* Engine::FileSystem::Config::CXMLDocument::NewText( const char* str )
{
	CXMLText* text = new (textPool.Alloc()) CXMLText( this );
	text->XMLMemPool = &textPool;
	text->SetValue( str );
	return text;
}


CXMLDeclaration* Engine::FileSystem::Config::CXMLDocument::NewDeclaration( const char* str )
{
	CXMLDeclaration* dec = new (commentPool.Alloc()) CXMLDeclaration( this );
	dec->XMLMemPool = &commentPool;
	dec->SetValue( str ? str : "xml version=\"1.0\" encoding=\"UTF-8\"" );
	return dec;
}


CXMLUnknown* Engine::FileSystem::Config::CXMLDocument::NewUnknown( const char* str )
{
	CXMLUnknown* unk = new (commentPool.Alloc()) CXMLUnknown( this );
	unk->XMLMemPool = &commentPool;
	unk->SetValue( str );
	return unk;
}


int Engine::FileSystem::Config::CXMLDocument::LoadFile( const char* filename )
{
	DeleteChildren();
	InitDocument();

#if defined(_MSC_VER)
#pragma warning ( push )
#pragma warning ( disable : 4996 )		// Fail to see a compelling reason why this should be deprecated.
#endif
	FILE* fp = fopen( filename, "rb" );
#if defined(_MSC_VER)
#pragma warning ( pop )
#endif
	if ( !fp ) {
		SetError( XML_ERROR_FILE_NOT_FOUND, filename, 0 );
		return errorID;
	}
	LoadFile( fp );
	fclose( fp );
	return errorID;
}


int Engine::FileSystem::Config::CXMLDocument::LoadFile( FILE* fp ) 
{
	DeleteChildren();
	InitDocument();

	fseek( fp, 0, SEEK_END );
	unsigned size = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	if ( size == 0 ) {
		return errorID;
	}

	charBuffer = (char*)Engine::Memory::GetDefaultAllocator()->Alloc(size + 1);//new char[size+1];
	fread( charBuffer, size, 1, fp );
	charBuffer[size] = 0;

	const char* p = charBuffer;
	p = CXMLUtil::SkipWhiteSpace( p );
	p = CXMLUtil::ReadBOM( p, &writeBOM );
	if ( !p || !*p ) {
		SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
		return errorID;
	}

	ParseDeep( charBuffer + (p-charBuffer), 0 );
	return errorID;
}


int Engine::FileSystem::Config::CXMLDocument::SaveFile( const char* filename )
{
#if defined(_MSC_VER)
#pragma warning ( push )
#pragma warning ( disable : 4996 )		// Fail to see a compelling reason why this should be deprecated.
#endif
	FILE* fp = fopen( filename, "w" );
#if defined(_MSC_VER)
#pragma warning ( pop )
#endif
	if ( !fp ) {
		SetError( XML_ERROR_FILE_COULD_NOT_BE_OPENED, filename, 0 );
		return errorID;
	}
	SaveFile(fp);
	fclose( fp );
	return errorID;
}


int Engine::FileSystem::Config::CXMLDocument::SaveFile( FILE* fp )
{
	CXMLPrinter stream( fp );
	Print( &stream );
	return errorID;
}


int Engine::FileSystem::Config::CXMLDocument::Parse( const char* p )
{
	DeleteChildren();
	InitDocument();

	if ( !p || !*p ) {
		SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
		return errorID;
	}
	p = CXMLUtil::SkipWhiteSpace( p );
	p = CXMLUtil::ReadBOM( p, &writeBOM );
	if ( !p || !*p ) {
		SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
		return errorID;
	}

	size_t len = strlen( p );
	charBuffer = (char*)Engine::Memory::GetDefaultAllocator()->Alloc(len + 1);//new char[ len+1 ];
	memcpy( charBuffer, p, len+1 );

	
	ParseDeep( charBuffer, 0 );
	return errorID;
}


void Engine::FileSystem::Config::CXMLDocument::Print( CXMLPrinter* streamer ) 
{
	CXMLPrinter stdStreamer( stdout );
	if ( !streamer )
		streamer = &stdStreamer;
	Accept( streamer );
}


void Engine::FileSystem::Config::CXMLDocument::SetError( int error, const char* str1, const char* str2 )
{
	errorID = error;
	errorStr1 = str1;
	errorStr2 = str2;
}


void Engine::FileSystem::Config::CXMLDocument::PrintError() const 
{
	if ( errorID ) {
		static const int LEN = 20;
		char buf1[LEN] = { 0 };
		char buf2[LEN] = { 0 };
		
		if ( errorStr1 ) {
			TIXML_SNPRINTF( buf1, LEN, "%s", errorStr1 );
		}
		if ( errorStr2 ) {
			TIXML_SNPRINTF( buf2, LEN, "%s", errorStr2 );
		}

		printf( "CXMLDocument error id=%d str1=%s str2=%s\n",
			    errorID, buf1, buf2 );
	}
}


Engine::FileSystem::Config::CXMLPrinter::CXMLPrinter( FILE* file ) : 
	elementJustOpened( false ), 
	firstElement( true ),
	fp( file ), 
	depth( 0 ), 
	textDepth( -1 ),
	processEntities( true )
{
	for( int i=0; i<ENTITY_RANGE; ++i ) {
		entityFlag[i] = false;
		restrictedEntityFlag[i] = false;
	}
	for( int i=0; i<NUM_ENTITIES; ++i ) {
		TIXMLASSERT( entities[i].value < ENTITY_RANGE );
		if ( entities[i].value < ENTITY_RANGE ) {
			entityFlag[ (int)entities[i].value ] = true;
		}
	}
	restrictedEntityFlag[(int)'&'] = true;
	restrictedEntityFlag[(int)'<'] = true;
	restrictedEntityFlag[(int)'>'] = true;	// not required, but consistency is nice
	buffer.Push( 0 );
}


void Engine::FileSystem::Config::CXMLPrinter::Print( const char* format, ... )
{
    va_list     va;
    va_start( va, format );

	if ( fp ) {
		vfprintf( fp, format, va );
	}
	else {
		// This seems brutally complex. Haven't figured out a better
		// way on windows.
		#ifdef _MSC_VER
			int len = -1;
			int expand = 1000;
			while ( len < 0 ) {
				len = vsnprintf_s( accumulator.Mem(), accumulator.Capacity(), _TRUNCATE, format, va );
				if ( len < 0 ) {
					expand *= 3/2;
					accumulator.PushArr( expand );
				}
			}
			char* p = buffer.PushArr( len ) - 1;
			memcpy( p, accumulator.Mem(), len+1 );
		#else
			int len = vsnprintf( 0, 0, format, va );
			// Close out and re-start the va-args
			va_end( va );
			va_start( va, format );		
			char* p = buffer.PushArr( len ) - 1;
			vsnprintf( p, len+1, format, va );
		#endif
	}
    va_end( va );
}


void Engine::FileSystem::Config::CXMLPrinter::PrintSpace( int depth )
{
	for( int i=0; i<depth; ++i ) {
		Print( "    " );
	}
}


void Engine::FileSystem::Config::CXMLPrinter::PrintString( const char* p, bool restricted )
{
	// Look for runs of bytes between entities to print.
	const char* q = p;
	const bool* flag = restricted ? restrictedEntityFlag : entityFlag;

	if ( processEntities ) {
		while ( *q ) {
			// Remember, char is sometimes signed. (How many times has that bitten me?)
			if ( *q > 0 && *q < ENTITY_RANGE ) {
				// Check for entities. If one is found, flush
				// the stream up until the entity, write the 
				// entity, and keep looking.
				if ( flag[(unsigned)(*q)] ) {
					while ( p < q ) {
						Print( "%c", *p );
						++p;
					}
					for( int i=0; i<NUM_ENTITIES; ++i ) {
						if ( entities[i].value == *q ) {
							Print( "&%s;", entities[i].pattern );
							break;
						}
					}
					++p;
				}
			}
			++q;
		}
	}
	// Flush the remaining string. This will be the entire
	// string if an entity wasn't found.
	if ( !processEntities || (q-p > 0) ) {
		Print( "%s", p );
	}
}


void Engine::FileSystem::Config::CXMLPrinter::PushHeader( bool writeBOM, bool writeDec )
{
	static const unsigned char bom[] = { TIXML_UTF_LEAD_0, TIXML_UTF_LEAD_1, TIXML_UTF_LEAD_2, 0 };
	if ( writeBOM ) {
		Print( "%s", bom );
	}
	if ( writeDec ) {
		PushDeclaration( "xml version=\"1.0\"" );
	}
}


void Engine::FileSystem::Config::CXMLPrinter::OpenElement( const char* name )
{
	if ( elementJustOpened ) {
		SealElement();
	}
	stack.Push( name );

	if ( textDepth < 0 && !firstElement ) {
		Print( "\n" );
		PrintSpace( depth );
	}

	Print( "<%s", name );
	elementJustOpened = true;
	firstElement = false;
	++depth;
}


void Engine::FileSystem::Config::CXMLPrinter::PushAttribute( const char* name, const char* value )
{
	TIXMLASSERT( elementJustOpened );
	Print( " %s=\"", name );
	PrintString( value, false );
	Print( "\"" );
}


void Engine::FileSystem::Config::CXMLPrinter::PushAttribute( const char* name, int v )
{
	char buf[BUF_SIZE];
	TIXML_SNPRINTF( buf, BUF_SIZE, "%d", v );	
	PushAttribute( name, buf );
}


void Engine::FileSystem::Config::CXMLPrinter::PushAttribute( const char* name, unsigned v )
{
	char buf[BUF_SIZE];
	TIXML_SNPRINTF( buf, BUF_SIZE, "%u", v );	
	PushAttribute( name, buf );
}


void Engine::FileSystem::Config::CXMLPrinter::PushAttribute( const char* name, bool v )
{
	char buf[BUF_SIZE];
	TIXML_SNPRINTF( buf, BUF_SIZE, "%d", v ? 1 : 0 );	
	PushAttribute( name, buf );
}


void Engine::FileSystem::Config::CXMLPrinter::PushAttribute( const char* name, double v )
{
	char buf[BUF_SIZE];
	TIXML_SNPRINTF( buf, BUF_SIZE, "%f", v );	
	PushAttribute( name, buf );
}


void Engine::FileSystem::Config::CXMLPrinter::CloseElement()
{
	--depth;
	const char* name = stack.Pop();

	if ( elementJustOpened ) {
		Print( "/>" );
	}
	else {
		if ( textDepth < 0 ) {
			Print( "\n" );
			PrintSpace( depth );
		}
		Print( "</%s>", name );
	}

	if ( textDepth == depth )
		textDepth = -1;
	if ( depth == 0 )
		Print( "\n" );
	elementJustOpened = false;
}


void Engine::FileSystem::Config::CXMLPrinter::SealElement()
{
	elementJustOpened = false;
	Print( ">" );
}


void Engine::FileSystem::Config::CXMLPrinter::PushText( const char* text, bool cdata )
{
	textDepth = depth-1;

	if ( elementJustOpened ) {
		SealElement();
	}
	if ( cdata ) {
		Print( "<![CDATA[" );
		Print( "%s", text );
		Print( "]]>" );
	}
	else {
		PrintString( text, true );
	}
}


void Engine::FileSystem::Config::CXMLPrinter::PushComment( const char* comment )
{
	if ( elementJustOpened ) {
		SealElement();
	}
	if ( textDepth < 0 && !firstElement ) {
		Print( "\n" );
		PrintSpace( depth );
	}
	firstElement = false;
	Print( "<!--%s-->", comment );
}


void Engine::FileSystem::Config::CXMLPrinter::PushDeclaration( const char* value )
{
	if ( elementJustOpened ) {
		SealElement();
	}
	if ( textDepth < 0 && !firstElement) {
		Print( "\n" );
		PrintSpace( depth );
	}
	firstElement = false;
	Print( "<?%s?>", value );
}


void Engine::FileSystem::Config::CXMLPrinter::PushUnknown( const char* value )
{
	if ( elementJustOpened ) {
		SealElement();
	}
	if ( textDepth < 0 && !firstElement ) {
		Print( "\n" );
		PrintSpace( depth );
	}
	firstElement = false;
	Print( "<!%s>", value );
}


bool Engine::FileSystem::Config::CXMLPrinter::VisitEnter( const CXMLDocument& doc )
{
	processEntities = doc.ProcessEntities();
	if ( doc.HasBOM() ) {
		PushHeader( true, false );
	}
	return true;
}


bool Engine::FileSystem::Config::CXMLPrinter::VisitEnter( const CXMLElement& element, const CXMLAttribute* attribute )
{
	OpenElement( element.Name() );
	while ( attribute ) {
		PushAttribute( attribute->Name(), attribute->Value() );
		attribute = attribute->Next();
	}
	return true;
}


bool Engine::FileSystem::Config::CXMLPrinter::VisitExit( const CXMLElement& )
{
	CloseElement();
	return true;
}


bool Engine::FileSystem::Config::CXMLPrinter::Visit( const CXMLText& text )
{
	PushText( text.Value(), text.CData() );
	return true;
}


bool Engine::FileSystem::Config::CXMLPrinter::Visit( const CXMLComment& comment )
{
	PushComment( comment.Value() );
	return true;
}

bool Engine::FileSystem::Config::CXMLPrinter::Visit( const CXMLDeclaration& declaration )
{
	PushDeclaration( declaration.Value() );
	return true;
}


bool Engine::FileSystem::Config::CXMLPrinter::Visit( const CXMLUnknown& unknown )
{
	PushUnknown( unknown.Value() );
	return true;
}