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

#ifndef TINYXML2_INCLUDED
#define TINYXML2_INCLUDED

#include <cctype>
#include <climits>
#include <cstdio>
#include <cstring>
#include <cstdarg>

#include "..\..\Memory.h"
#include "..\..\CAllocator.h"
#include "..\..\CHeapAllocator.h"

#include "..\..\CLog.h"

/* 
   TODO: intern strings instead of allocation.
*/
/*
	gcc: g++ -Wall tinyxml2.cpp xmltest.cpp -o gccxmltest.exe
*/

#if defined( _DEBUG ) || defined( DEBUG ) || defined (__DEBUG__)
	#ifndef DEBUG
		#define DEBUG
	#endif
#endif

#define TIXMLASSERT(x) LOG_ASSERT(x);      

/*
#if defined(DEBUG)
        #if defined(_MSC_VER)
                #define TIXMLASSERT( x )           if ( !(x)) { __debugbreak(); } //if ( !(x)) WinDebugBreak()
        #elif defined (ANDROID_NDK)
                #include <android/log.h>
                #define TIXMLASSERT( x )           if ( !(x)) { __android_log_assert( "assert", "grinliz", "ASSERT in '%s' at %d.", __FILE__, __LINE__ ); }
        #else
                #include <assert.h>
                #define TIXMLASSERT                assert
        #endif
#else
        #define TIXMLASSERT( x )           {}
#endif
*/

#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
	// Microsoft visual studio, version 2005 and higher.
	/*int _snprintf_s(
	   char *buffer,
	   size_t sizeOfBuffer,
	   size_t count,	
	   const char *format [,
		  argument] ... 
	);*/
	inline int TIXML_SNPRINTF( char* buffer, size_t size, const char* format, ... ) {
	    va_list va;
		va_start( va, format );
		int result = vsnprintf_s( buffer, size, _TRUNCATE, format, va );
	    va_end( va );
		return result;
	}
	#define TIXML_SSCANF   sscanf_s
#else
	// GCC version 3 and higher
	//#warning( "Using sn* functions." )
	#define TIXML_SNPRINTF snprintf
	#define TIXML_SSCANF   sscanf
#endif

static const int TIXML2_MAJOR_VERSION = 1;
static const int TIXML2_MINOR_VERSION = 0;
static const int TIXML2_PATCH_VERSION = 1;

//namespace tinyxml2
//{
class CXMLDocument;
class CXMLElement;
class CXMLAttribute;
class CXMLComment;
class CXMLNode;
class CXMLText;
class CXMLDeclaration;
class CXMLUnknown;

class CXMLPrinter;

/*
	A class that wraps strings. Normally stores the start and end
	pointers into the XML file itself, and will apply normalization
	and entity translation if actually read. Can also store (and memory
	manage) a traditional char[]
*/
class CXMLStrPair
{
public:
	enum {
		NEEDS_ENTITY_PROCESSING			= 0x01,
		NEEDS_NEWLINE_NORMALIZATION		= 0x02,

		TEXT_ELEMENT		= NEEDS_ENTITY_PROCESSING | NEEDS_NEWLINE_NORMALIZATION,
		TEXT_ELEMENT_LEAVE_ENTITIES		= NEEDS_NEWLINE_NORMALIZATION,
		ATTRIBUTE_NAME		= 0,
		ATTRIBUTE_VALUE		= NEEDS_ENTITY_PROCESSING | NEEDS_NEWLINE_NORMALIZATION,
		ATTRIBUTE_VALUE_LEAVE_ENTITIES		= NEEDS_NEWLINE_NORMALIZATION,
		COMMENT				= NEEDS_NEWLINE_NORMALIZATION
	};

	CXMLStrPair() : flags( 0 ), start( 0 ), end( 0 ) {}
	~CXMLStrPair();

	void Set( char* _start, char* _end, int _flags ) {
		Reset();
		this->start = _start; this->end = _end; this->flags = _flags | NEEDS_FLUSH;
	}
	const char* GetStr();
	bool Empty() const { return start == end; }

	void SetInternedStr( const char* str ) { Reset(); this->start = const_cast<char*>(str); }
	void SetStr( const char* str, int flags=0 );

	char* ParseText( char* in, const char* endTag, int strFlags );
	char* ParseName( char* in );


private:
	void Reset();

	enum {
		NEEDS_FLUSH = 0x100,
		NEEDS_DELETE = 0x200
	};

	// After parsing, if *end != 0, it can be set to zero.
	int flags;
	char* start;	
	char* end;
};


/*
	A dynamic array of Plain Old Data. Doesn't support constructors, etc.
	Has a small initial memory pool, so that low or no usage will not
	cause a call to new/delete
*/
template <class T, int INIT>
class CXMLDynArray
{
public:
	CXMLDynArray< T, INIT >() 
	{
		mem = pool;
		allocated = INIT;
		size = 0;
	}
	~CXMLDynArray()
	{
		if ( mem != pool ) {
			Engine::Memory::GetDefaultAllocator()->FreeArray(&mem);
		}
	}
	void Push( T t )
	{
		EnsureCapacity( size+1 );
		mem[size++] = t;
	}

	T* PushArr( int count )
	{
		EnsureCapacity( size+count );
		T* ret = &mem[size];
		size += count;
		return ret;
	}
	T Pop() {
		return mem[--size];
	}
	void PopArr( int count ) 
	{
		TIXMLASSERT( size >= count );
		size -= count;
	}

	bool Empty() const					{ return size == 0; }
	T& operator[](int i)				{ TIXMLASSERT( i>= 0 && i < size ); return mem[i]; }
	const T& operator[](int i) const	{ TIXMLASSERT( i>= 0 && i < size ); return mem[i]; }
	int Size() const					{ return size; }
	int Capacity() const				{ return allocated; }
	const T* Mem() const				{ return mem; }
	T* Mem()							{ return mem; }


private:
	void EnsureCapacity( int cap ) {
		if ( cap > allocated ) {
			int newAllocated = cap * 2;
			//T* newMem = new T[newAllocated];
			T* newMem = (T*)Engine::Memory::GetDefaultAllocator()->Alloc(sizeof(T) * newAllocated); 

			memcpy( newMem, mem, sizeof(T)*size );	// warning: not using constructors, only works for PODs
			if ( mem != pool ) 
			{
				//delete [] mem;
				Engine::Memory::GetDefaultAllocator()->FreeArray(&mem);
			}

			mem = newMem;
			allocated = newAllocated;
		}
	}

	T* mem;
	T pool[INIT];
	int allocated;		// objects allocated
	int size;			// number objects in use
};


/*
	Parent virtual class a a pool for fast allocation
	and deallocation of objects.
*/
class CXMLMemPool
{
public:
	CXMLMemPool() {}
	virtual ~CXMLMemPool() {}

	virtual int ItemSize() const = 0;
	virtual void* Alloc() = 0;
	virtual void Free( void* ) = 0; 
};


/*
	Template child class to create pools of the correct type.
*/
template< int SIZE >
class CXMLMemPoolT : public CXMLMemPool
{
public:
	CXMLMemPoolT() : root(0), currentAllocs(0), nAllocs(0), maxAllocs(0)	{}
	~CXMLMemPoolT() {
		// Delete the blocks.
		for( int i=0; i<blockPtrs.Size(); ++i ) {
			Engine::Memory::GetDefaultAllocator()->Free(&blockPtrs[i]);
			//delete blockPtrs[i];
		}
	}

	virtual int ItemSize() const	{ return SIZE; }
	int CurrentAllocs() const		{ return currentAllocs; }

	virtual void* Alloc() {
		if ( !root ) {
			// Need a new block.
			Block* block = new Block();
			blockPtrs.Push( block );

			for( int i=0; i<COUNT-1; ++i ) {
				block->chunk[i].next = &block->chunk[i+1];
			}
			block->chunk[COUNT-1].next = 0;
			root = block->chunk;
		}
		void* result = root;
		root = root->next;

		++currentAllocs;
		if ( currentAllocs > maxAllocs ) maxAllocs = currentAllocs;
		nAllocs++;
		return result;
	}
	virtual void Free( void* mem ) {
		if ( !mem ) return;
		--currentAllocs;
		Chunk* chunk = (Chunk*)mem;
		memset( chunk, 0xfe, sizeof(Chunk) );
		chunk->next = root;
		root = chunk;
	}
	void Trace( const char* name ) {
		printf( "CXMLMemPool %s watermark=%d [%dk] current=%d size=%d nAlloc=%d blocks=%d\n",
				 name, maxAllocs, maxAllocs*SIZE/1024, currentAllocs, SIZE, nAllocs, blockPtrs.Size() );
	}

private:
	enum { COUNT = 1024/SIZE };
	union Chunk {
		Chunk* next;
		char mem[SIZE];
	};
	struct Block {
		Chunk chunk[COUNT];
	};
	CXMLDynArray< Block*, 10 > blockPtrs;
	Chunk* root;

	int currentAllocs;
	int nAllocs;
	int maxAllocs;
};



/**
	Implements the interface to the "Visitor pattern" (see the Accept() method.)
	If you call the Accept() method, it requires being passed a CXMLVisitor
	class to handle callbacks. For nodes that contain other nodes (Document, Element)
	you will get called with a VisitEnter/VisitExit pair. Nodes that are always leaves
	are simply called with Visit().

	If you return 'true' from a Visit method, recursive parsing will continue. If you return
	false, <b>no children of this node or its sibilings</b> will be Visited.

	All flavors of Visit methods have a default implementation that returns 'true' (continue 
	visiting). You need to only override methods that are interesting to you.

	Generally Accept() is called on the TiCXMLDocument, although all nodes suppert Visiting.

	You should never change the document from a callback.

	@sa CXMLNode::Accept()
*/
class CXMLVisitor
{
public:
	virtual ~CXMLVisitor() {}

	/// Visit a document.
	virtual bool VisitEnter( const CXMLDocument& /*doc*/ )			{ return true; }
	/// Visit a document.
	virtual bool VisitExit( const CXMLDocument& /*doc*/ )			{ return true; }

	/// Visit an element.
	virtual bool VisitEnter( const CXMLElement& /*element*/, const CXMLAttribute* /*firstAttribute*/ )	{ return true; }
	/// Visit an element.
	virtual bool VisitExit( const CXMLElement& /*element*/ )			{ return true; }

	/// Visit a declaration
	virtual bool Visit( const CXMLDeclaration& /*declaration*/ )		{ return true; }
	/// Visit a text node
	virtual bool Visit( const CXMLText& /*text*/ )					{ return true; }
	/// Visit a comment node
	virtual bool Visit( const CXMLComment& /*comment*/ )				{ return true; }
	/// Visit an unknown node
	virtual bool Visit( const CXMLUnknown& /*unknown*/ )				{ return true; }
};


/*
	Utility functionality.
*/
class CXMLUtil
{
public:
	// Anything in the high order range of UTF-8 is assumed to not be whitespace. This isn't 
	// correct, but simple, and usually works.
	static const char* SkipWhiteSpace( const char* p )	{ while( !IsUTF8Continuation(*p) && isspace( *p ) ) { ++p; } return p; }
	static char* SkipWhiteSpace( char* p )				{ while( !IsUTF8Continuation(*p) && isspace( *p ) ) { ++p; } return p; }

	inline static bool StringEqual( const char* p, const char* q, int nChar=INT_MAX )  {
		int n = 0;
		if ( p == q ) {
			return true;
		}
		while( *p && *q && *p == *q && n<nChar ) {
			++p; ++q; ++n;
		}
		if ( (n == nChar) || ( *p == 0 && *q == 0 ) ) {
			return true;
		}
		return false;
	}
	inline static int IsUTF8Continuation( const char p ) { return p & 0x80; }
	inline static int IsAlphaNum( unsigned char anyByte )	{ return ( anyByte < 128 ) ? isalnum( anyByte ) : 1; }
	inline static int IsAlpha( unsigned char anyByte )		{ return ( anyByte < 128 ) ? isalpha( anyByte ) : 1; }

	static const char* ReadBOM( const char* p, bool* hasBOM );
	// p is the starting location,
	// the UTF-8 value of the entity will be placed in value, and length filled in.
	static const char* GetCharacterRef( const char* p, char* value, int* length );
	static void ConvertUTF32ToUTF8( unsigned long input, char* output, int* length );
};


/** CXMLNode is a base class for every object that is in the
	XML Document Object Model (DOM), except CXMLAttributes.
	Nodes have siblings, a parent, and children which can
	be navigated. A node is always in a CXMLDocument.
	The type of a CXMLNode can be queried, and it can 
	be cast to its more defined type.

	An CXMLDocument allocates memory for all its Nodes.
	When the CXMLDocument gets deleted, all its Nodes
	will also be deleted.

	@verbatim
	A Document can contain:	Element	(container or leaf)
							Comment (leaf)
							Unknown (leaf)
							Declaration( leaf )

	An Element can contain:	Element (container or leaf)
							Text	(leaf)
							Attributes (not on tree)
							Comment (leaf)
							Unknown (leaf)

	@endverbatim
*/
class CXMLNode
{
	friend class CXMLDocument;
	friend class CXMLElement;
public:

	/// Get the CXMLDocument that owns this CXMLNode.
	const CXMLDocument* GetDocument() const	{ return document; }
	/// Get the CXMLDocument that owns this CXMLNode.
	CXMLDocument* GetDocument()				{ return document; }

	virtual CXMLElement*		ToElement()		{ return 0; }	///< Safely cast to an Element, or null.
	virtual CXMLText*		ToText()		{ return 0; }	///< Safely cast to Text, or null.
	virtual CXMLComment*		ToComment()		{ return 0; }	///< Safely cast to a Comment, or null.
	virtual CXMLDocument*	ToDocument()	{ return 0; }	///< Safely cast to a Document, or null.
	virtual CXMLDeclaration*	ToDeclaration()	{ return 0; }	///< Safely cast to a Declaration, or null.
	virtual CXMLUnknown*		ToUnknown()		{ return 0; }	///< Safely cast to an Unknown, or null.

	virtual const CXMLElement*		ToElement() const		{ return 0; }
	virtual const CXMLText*			ToText() const			{ return 0; }
	virtual const CXMLComment*		ToComment() const		{ return 0; }
	virtual const CXMLDocument*		ToDocument() const		{ return 0; }
	virtual const CXMLDeclaration*	ToDeclaration() const	{ return 0; }
	virtual const CXMLUnknown*		ToUnknown() const		{ return 0; }

	/** The meaning of 'value' changes for the specific type.
		@verbatim
		Document:	empy
		Element:	name of the element
		Comment:	the comment text
		Unknown:	the tag contents
		Text:		the text string
		@endverbatim
	*/
	const char* Value() const			{ return value.GetStr(); }
	/** Set the Value of an XML node.
		@sa Value()
	*/
	void SetValue( const char* val, bool staticMem=false );

	/// Get the parent of this node on the DOM.
	const CXMLNode*	Parent() const			{ return parent; }
	CXMLNode* Parent()						{ return parent; }

	/// Returns true if this node has no children.
	bool NoChildren() const					{ return !firstChild; }

	/// Get the first child node, or null if none exists.
	const CXMLNode*  FirstChild() const		{ return firstChild; }
	CXMLNode*		FirstChild()			{ return firstChild; }
	/** Get the first child element, or optionally the first child
	    element with the specified name.
	*/
	const CXMLElement* FirstChildElement( const char* value=0 ) const;
	CXMLElement* FirstChildElement( const char* _value=0 )	{ return const_cast<CXMLElement*>(const_cast<const CXMLNode*>(this)->FirstChildElement( _value )); }

	/// Get the last child node, or null if none exists.
	const CXMLNode*	LastChild() const						{ return lastChild; }
	CXMLNode*		LastChild()								{ return const_cast<CXMLNode*>(const_cast<const CXMLNode*>(this)->LastChild() ); }

	/** Get the last child element or optionally the last child
	    element with the specified name.
	*/
	const CXMLElement* LastChildElement( const char* value=0 ) const;
	CXMLElement* LastChildElement( const char* _value=0 )	{ return const_cast<CXMLElement*>(const_cast<const CXMLNode*>(this)->LastChildElement(_value) ); }
	
	/// Get the previous (left) sibling node of this node.
	const CXMLNode*	PreviousSibling() const					{ return prev; }
	CXMLNode*	PreviousSibling()							{ return prev; }

	/// Get the previous (left) sibling element of this node, with an opitionally supplied name.
	const CXMLElement*	PreviousSiblingElement( const char* value=0 ) const ;
	CXMLElement*	PreviousSiblingElement( const char* _value=0 ) { return const_cast<CXMLElement*>(const_cast<const CXMLNode*>(this)->PreviousSiblingElement( _value ) ); }
	
	/// Get the next (right) sibling node of this node.
	const CXMLNode*	NextSibling() const						{ return next; }
	CXMLNode*	NextSibling()								{ return next; }
		
	/// Get the next (right) sibling element of this node, with an opitionally supplied name.
	const CXMLElement*	NextSiblingElement( const char* value=0 ) const;
 	CXMLElement*	NextSiblingElement( const char* _value=0 )	{ return const_cast<CXMLElement*>(const_cast<const CXMLNode*>(this)->NextSiblingElement( _value ) ); }

	/**
		Add a child node as the last (right) child.
	*/
	CXMLNode* InsertEndChild( CXMLNode* addThis );

	CXMLNode* LinkEndChild( CXMLNode* addThis )	{ return InsertEndChild( addThis ); }
	/**
		Add a child node as the first (left) child.
	*/
	CXMLNode* InsertFirstChild( CXMLNode* addThis );
	/**
		Add a node after the specified child node.
	*/
	CXMLNode* InsertAfterChild( CXMLNode* afterThis, CXMLNode* addThis );
	
	/**
		Delete all the children of this node.
	*/
	void DeleteChildren();

	/**
		Delete a child of this node.
	*/
	void DeleteChild( CXMLNode* node );

	/**
		Make a copy of this node, but not its children.
		You may pass in a Document pointer that will be
		the owner of the new Node. If the 'document' is 
		null, then the node returned will be allocated
		from the current Document. (this->GetDocument())

		Note: if called on a CXMLDocument, this will return null.
	*/
	virtual CXMLNode* ShallowClone( CXMLDocument* document ) const = 0;

	/**
		Test if 2 nodes are the same, but don't test children.
		The 2 nodes do not need to be in the same Document.

		Note: if called on a CXMLDocument, this will return false.
	*/
	virtual bool ShallowEqual( const CXMLNode* compare ) const = 0;

	/** Accept a hierchical visit the nodes in the TinyXML DOM. Every node in the 
		XML tree will be conditionally visited and the host will be called back
		via the TiCXMLVisitor interface.

		This is essentially a SAX interface for TinyXML. (Note however it doesn't re-parse
		the XML for the callbacks, so the performance of TinyXML is unchanged by using this
		interface versus any other.)

		The interface has been based on ideas from:

		- http://www.saxproject.org/
		- http://c2.com/cgi/wiki?HierarchicalVisitorPattern 

		Which are both good references for "visiting".

		An example of using Accept():
		@verbatim
		TiCXMLPrinter printer;
		tinyxmlDoc.Accept( &printer );
		const char* xmlcstr = printer.CStr();
		@endverbatim
	*/
	virtual bool Accept( CXMLVisitor* visitor ) const = 0;

	// internal
	virtual char* ParseDeep( char*, CXMLStrPair* );

protected:
	CXMLNode( CXMLDocument* );
	virtual ~CXMLNode();
	CXMLNode( const CXMLNode& );	// not supported
	void operator=( const CXMLNode& );	// not supported
	
	CXMLDocument*	document;
	CXMLNode*		parent;
	mutable CXMLStrPair	value;

	CXMLNode*		firstChild;
	CXMLNode*		lastChild;

	CXMLNode*		prev;
	CXMLNode*		next;

private:
	CXMLMemPool*		XMLMemPool;
	void Unlink( CXMLNode* child );
};


/** XML text.

	Note that a text node can have child element nodes, for example:
	@verbatim
	<root>This is <b>bold</b></root>
	@endverbatim

	A text node can have 2 ways to output the next. "normal" output 
	and CDATA. It will default to the mode it was parsed from the XML file and
	you generally want to leave it alone, but you can change the output mode with 
	SetCDATA() and query it with CDATA().
*/
class CXMLText : public CXMLNode
{
	friend class XMLBase;
	friend class CXMLDocument;
public:
	virtual bool Accept( CXMLVisitor* visitor ) const;

	virtual CXMLText*	ToText()			{ return this; }
	virtual const CXMLText*	ToText() const	{ return this; }

	/// Declare whether this should be CDATA or standard text.
	void SetCData( bool _isCData )			{ this->isCData = _isCData; }
	/// Returns true if this is a CDATA text element.
	bool CData() const						{ return isCData; }

	char* ParseDeep( char*, CXMLStrPair* endTag );
	virtual CXMLNode* ShallowClone( CXMLDocument* document ) const;
	virtual bool ShallowEqual( const CXMLNode* compare ) const;


protected:
	CXMLText( CXMLDocument* doc )	: CXMLNode( doc ), isCData( false )	{}
	virtual ~CXMLText()												{}
	CXMLText( const CXMLText& );	// not supported
	void operator=( const CXMLText& );	// not supported

private:
	bool isCData;
};


/** An XML Comment. */
class CXMLComment : public CXMLNode
{
	friend class CXMLDocument;
public:
	virtual CXMLComment*	ToComment()					{ return this; }
	virtual const CXMLComment* ToComment() const		{ return this; }

	virtual bool Accept( CXMLVisitor* visitor ) const;

	char* ParseDeep( char*, CXMLStrPair* endTag );
	virtual CXMLNode* ShallowClone( CXMLDocument* document ) const;
	virtual bool ShallowEqual( const CXMLNode* compare ) const;

protected:
	CXMLComment( CXMLDocument* doc );
	virtual ~CXMLComment();
	CXMLComment( const CXMLComment& );	// not supported
	void operator=( const CXMLComment& );	// not supported

private:
};


/** In correct XML the declaration is the first entry in the file.
	@verbatim
		<?xml version="1.0" standalone="yes"?>
	@endverbatim

	TinyXML2 will happily read or write files without a declaration,
	however.

	The text of the declaration isn't interpreted. It is parsed
	and written as a string.
*/
class CXMLDeclaration : public CXMLNode
{
	friend class CXMLDocument;
public:
	virtual CXMLDeclaration*	ToDeclaration()					{ return this; }
	virtual const CXMLDeclaration* ToDeclaration() const		{ return this; }

	virtual bool Accept( CXMLVisitor* visitor ) const;

	char* ParseDeep( char*, CXMLStrPair* endTag );
	virtual CXMLNode* ShallowClone( CXMLDocument* document ) const;
	virtual bool ShallowEqual( const CXMLNode* compare ) const;

protected:
	CXMLDeclaration( CXMLDocument* doc );
	virtual ~CXMLDeclaration();
	CXMLDeclaration( const CXMLDeclaration& );	// not supported
	void operator=( const CXMLDeclaration& );	// not supported
};


/** Any tag that tinyXml doesn't recognize is saved as an
	unknown. It is a tag of text, but should not be modified.
	It will be written back to the XML, unchanged, when the file
	is saved.

	DTD tags get thrown into TiCXMLUnknowns.
*/
class CXMLUnknown : public CXMLNode
{
	friend class CXMLDocument;
public:
	virtual CXMLUnknown*	ToUnknown()					{ return this; }
	virtual const CXMLUnknown* ToUnknown() const		{ return this; }

	virtual bool Accept( CXMLVisitor* visitor ) const;

	char* ParseDeep( char*, CXMLStrPair* endTag );
	virtual CXMLNode* ShallowClone( CXMLDocument* document ) const;
	virtual bool ShallowEqual( const CXMLNode* compare ) const;

protected:
	CXMLUnknown( CXMLDocument* doc );
	virtual ~CXMLUnknown();
	CXMLUnknown( const CXMLUnknown& );	// not supported
	void operator=( const CXMLUnknown& );	// not supported
};


enum {
	XML_NO_ERROR = 0,
	XML_SUCCESS = 0,

	XML_NO_ATTRIBUTE,
	XML_WRONG_ATTRIBUTE_TYPE,

	XML_ERROR_FILE_NOT_FOUND,
	XML_ERROR_FILE_COULD_NOT_BE_OPENED,
	XML_ERROR_ELEMENT_MISMATCH,
	XML_ERROR_PARSING_ELEMENT,
	XML_ERROR_PARSING_ATTRIBUTE,
	XML_ERROR_IDENTIFYING_TAG,
	XML_ERROR_PARSING_TEXT,
	XML_ERROR_PARSING_CDATA,
	XML_ERROR_PARSING_COMMENT,
	XML_ERROR_PARSING_DECLARATION,
	XML_ERROR_PARSING_UNKNOWN,
	XML_ERROR_EMPTY_DOCUMENT,
	XML_ERROR_MISMATCHED_ELEMENT,
	XML_ERROR_PARSING
};


/** An attribute is a name-value pair. Elements have an arbitrary
	number of attributes, each with a unique name.

	@note The attributes are not CXMLNodes. You may only query the
	Next() attribute in a list.
*/
class CXMLAttribute
{
	friend class CXMLElement;
public:
	const char* Name() const { return name.GetStr(); }			///< The name of the attribute.
	const char* Value() const { return value.GetStr(); }		///< The value of the attribute.
	const CXMLAttribute* Next() const { return next; }			///< The next attribute in the list.

	/** IntAttribute interprets the attribute as an integer, and returns the value.
	    If the value isn't an integer, 0 will be returned. There is no error checking;
		use QueryIntAttribute() if you need error checking.
	*/
	int		 IntValue() const				{ int i=0;		QueryIntValue( &i );		return i; }
	/// Query as an unsigned integer. See IntAttribute()
	unsigned UnsignedValue() const			{ unsigned i=0; QueryUnsignedValue( &i );	return i; }
	/// Query as a boolean. See IntAttribute()
	bool	 BoolValue() const				{ bool b=false; QueryBoolValue( &b );		return b; }
	/// Query as a double. See IntAttribute()
	double 	 DoubleValue() const			{ double d=0;	QueryDoubleValue( &d );		return d; }
	/// Query as a float. See IntAttribute()
	float	 FloatValue() const				{ float f=0;	QueryFloatValue( &f );		return f; }

	/** QueryIntAttribute interprets the attribute as an integer, and returns the value
		in the provided paremeter. The function will return XML_NO_ERROR on success,
		and XML_WRONG_ATTRIBUTE_TYPE if the conversion is not successful.
	*/
	int QueryIntValue( int* value ) const;
	/// See QueryIntAttribute
	int QueryUnsignedValue( unsigned int* value ) const;
	/// See QueryIntAttribute
	int QueryBoolValue( bool* value ) const;
	/// See QueryIntAttribute
	int QueryDoubleValue( double* value ) const;
	/// See QueryIntAttribute
	int QueryFloatValue( float* value ) const;

	/// Set the attribute to a string value.
	void SetAttribute( const char* value );
	/// Set the attribute to value.
	void SetAttribute( int value );
	/// Set the attribute to value.
	void SetAttribute( unsigned value );
	/// Set the attribute to value.
	void SetAttribute( bool value );
	/// Set the attribute to value.
	void SetAttribute( double value );
	/// Set the attribute to value.
	void SetAttribute( float value );

private:
	enum { BUF_SIZE = 200 };

	CXMLAttribute() : next( 0 ) {}
	virtual ~CXMLAttribute()	{}
	CXMLAttribute( const CXMLAttribute& );	// not supported
	void operator=( const CXMLAttribute& );	// not supported
	void SetName( const char* name );

	char* ParseDeep( char* p, bool processEntities );

	mutable CXMLStrPair name;
	mutable CXMLStrPair value;
	CXMLAttribute* next;
	CXMLMemPool* XMLMemPool;
};


/** The element is a container class. It has a value, the element name,
	and can contain other elements, text, comments, and unknowns.
	Elements also contain an arbitrary number of attributes.
*/
class CXMLElement : public CXMLNode
{
	friend class XMLBase;
	friend class CXMLDocument;
public:
	/// Get the name of an element (which is the Value() of the node.)
	const char* Name() const		{ return Value(); }
	/// Set the name of the element.
	void SetName( const char* str, bool staticMem=false )	{ SetValue( str, staticMem ); }

	virtual CXMLElement* ToElement()				{ return this; }
	virtual const CXMLElement* ToElement() const { return this; }
	virtual bool Accept( CXMLVisitor* visitor ) const;

	/** Given an attribute name, Attribute() returns the value
		for the attribute of that name, or null if none 
		exists. For example:

		@verbatim
		const char* value = ele->Attribute( "foo" );
		@endverbatim

		The 'value' parameter is normally null. However, if specified, 
		the attribute will only be returned if the 'name' and 'value' 
		match. This allow you to write code:

		@verbatim
		if ( ele->Attribute( "foo", "bar" ) ) callFooIsBar();
		@endverbatim

		rather than:
		@verbatim
		if ( ele->Attribute( "foo" ) ) {
			if ( strcmp( ele->Attribute( "foo" ), "bar" ) == 0 ) callFooIsBar();
		}
		@endverbatim
	*/
	const char* Attribute( const char* name, const char* value=0 ) const;

	/** Given an attribute name, IntAttribute() returns the value
		of the attribute interpreted as an integer. 0 will be
		returned if there is an error. For a method with error 
		checking, see QueryIntAttribute()
	*/
	int		 IntAttribute( const char* name ) const		{ int i=0;		QueryIntAttribute( name, &i );		return i; }
	/// See IntAttribute()
	unsigned UnsignedAttribute( const char* name ) const{ unsigned i=0; QueryUnsignedAttribute( name, &i ); return i; }
	/// See IntAttribute()
	bool	 BoolAttribute( const char* name ) const	{ bool b=false; QueryBoolAttribute( name, &b );		return b; }
	/// See IntAttribute()
	double 	 DoubleAttribute( const char* name ) const	{ double d=0;	QueryDoubleAttribute( name, &d );		return d; }
	/// See IntAttribute()
	float	 FloatAttribute( const char* name ) const	{ float f=0;	QueryFloatAttribute( name, &f );		return f; }

	/** Given an attribute name, QueryIntAttribute() returns 
		XML_NO_ERROR, XML_WRONG_ATTRIBUTE_TYPE if the conversion
		can't be performed, or XML_NO_ATTRIBUTE if the attribute
		doesn't exist. If successful, the result of the conversion
		will be written to 'value'. If not successful, nothing will
		be written to 'value'. This allows you to provide default
		value:

		@verbatim
		int value = 10;
		QueryIntAttribute( "foo", &value );		// if "foo" isn't found, value will still be 10
		@endverbatim
	*/
	int QueryIntAttribute( const char* name, int* _value ) const					{ const CXMLAttribute* a = FindAttribute( name ); if ( !a ) return XML_NO_ATTRIBUTE; return a->QueryIntValue( _value ); } 
	/// See QueryIntAttribute()
	int QueryUnsignedAttribute( const char* name, unsigned int* _value ) const	{ const CXMLAttribute* a = FindAttribute( name ); if ( !a ) return XML_NO_ATTRIBUTE; return a->QueryUnsignedValue( _value ); }
	/// See QueryIntAttribute()
	int QueryBoolAttribute( const char* name, bool* _value ) const				{ const CXMLAttribute* a = FindAttribute( name ); if ( !a ) return XML_NO_ATTRIBUTE; return a->QueryBoolValue( _value ); }
	/// See QueryIntAttribute()
	int QueryDoubleAttribute( const char* name, double* _value ) const			{ const CXMLAttribute* a = FindAttribute( name ); if ( !a ) return XML_NO_ATTRIBUTE; return a->QueryDoubleValue( _value ); }
	/// See QueryIntAttribute()
	int QueryFloatAttribute( const char* name, float* _value ) const				{ const CXMLAttribute* a = FindAttribute( name ); if ( !a ) return XML_NO_ATTRIBUTE; return a->QueryFloatValue( _value ); }

	/// Sets the named attribute to value.
	void SetAttribute( const char* name, const char* _value )	{ CXMLAttribute* a = FindOrCreateAttribute( name ); a->SetAttribute( _value ); }
	/// Sets the named attribute to value.
	void SetAttribute( const char* name, int _value )			{ CXMLAttribute* a = FindOrCreateAttribute( name ); a->SetAttribute( _value ); }
	/// Sets the named attribute to value.
	void SetAttribute( const char* name, unsigned _value )		{ CXMLAttribute* a = FindOrCreateAttribute( name ); a->SetAttribute( _value ); }
	/// Sets the named attribute to value.
	void SetAttribute( const char* name, bool _value )			{ CXMLAttribute* a = FindOrCreateAttribute( name ); a->SetAttribute( _value ); }
	/// Sets the named attribute to value.
	void SetAttribute( const char* name, double _value )			{ CXMLAttribute* a = FindOrCreateAttribute( name ); a->SetAttribute( _value ); }

	/**
		Delete an attribute.
	*/
	void DeleteAttribute( const char* name );

	/// Return the first attribute in the list.
	const CXMLAttribute* FirstAttribute() const { return rootAttribute; }
	/// Query a specific attribute in the list.
	const CXMLAttribute* FindAttribute( const char* name ) const;

	/** Convenience function for easy access to the text inside an element. Although easy
		and concise, GetText() is limited compared to getting the TiCXMLText child
		and accessing it directly.
	
		If the first child of 'this' is a TiCXMLText, the GetText()
		returns the character string of the Text node, else null is returned.

		This is a convenient method for getting the text of simple contained text:
		@verbatim
		<foo>This is text</foo>
		const char* str = fooElement->GetText();
		@endverbatim

		'str' will be a pointer to "This is text". 
		
		Note that this function can be misleading. If the element foo was created from
		this XML:
		@verbatim
		<foo><b>This is text</b></foo> 
		@endverbatim

		then the value of str would be null. The first child node isn't a text node, it is
		another element. From this XML:
		@verbatim
		<foo>This is <b>text</b></foo> 
		@endverbatim
		GetText() will return "This is ".
	*/
	const char* GetText() const;

	// internal:
	enum {
		OPEN,		// <foo>
		CLOSED,		// <foo/>
		CLOSING		// </foo>
	};
	int ClosingType() const { return closingType; }
	char* ParseDeep( char* p, CXMLStrPair* endTag );
	virtual CXMLNode* ShallowClone( CXMLDocument* document ) const;
	virtual bool ShallowEqual( const CXMLNode* compare ) const;

private:
	CXMLElement( CXMLDocument* doc );
	virtual ~CXMLElement();
	CXMLElement( const CXMLElement& );	// not supported
	void operator=( const CXMLElement& );	// not supported

	CXMLAttribute* FindAttribute( const char* name );
	CXMLAttribute* FindOrCreateAttribute( const char* name );
	//void LinkAttribute( CXMLAttribute* attrib );
	char* ParseAttributes( char* p );

	int closingType;
	// The attribute list is ordered; there is no 'lastAttribute'
	// because the list needs to be scanned for dupes before adding
	// a new attribute.
	CXMLAttribute* rootAttribute;
};


/** A document binds together all the functionality. 
	It can be saved, loaded, and printed to the screen.
	All Nodes are connected and allocated to a Document.
	If the Document is deleted, all its Nodes are also deleted.
*/
class CXMLDocument : public CXMLNode
{
	friend class CXMLElement;
public:
	/// constructor
	CXMLDocument( bool processEntities = true ); 
	~CXMLDocument();

	virtual CXMLDocument* ToDocument()				{ return this; }
	virtual const CXMLDocument* ToDocument() const	{ return this; }

	/**
		Parse an XML file from a character string.
		Returns XML_NO_ERROR (0) on success, or
		an errorID.
	*/
	int Parse( const char* xml );
	
	/**
		Load an XML file from disk.
		Returns XML_NO_ERROR (0) on success, or
		an errorID.
	*/	
	int LoadFile( const char* filename );
	
	/**
		Load an XML file from disk. You are responsible
		for providing and closing the FILE*.

		Returns XML_NO_ERROR (0) on success, or
		an errorID.
	*/	
	int LoadFile( FILE* );
	
	/**
		Save the XML file to disk.
		Returns XML_NO_ERROR (0) on success, or
		an errorID.
	*/
	int SaveFile( const char* filename );

	/**
		Save the XML file to disk.  You are responsible
		for providing and closing the FILE*.

		Returns XML_NO_ERROR (0) on success, or
		an errorID.
	*/
	int SaveFile( FILE* );

	bool ProcessEntities() const						{ return processEntities; }

	/**
		Returns true if this document has a leading Byte Order Mark of UTF8.
	*/
	bool HasBOM() const { return writeBOM; }
	/** Sets whether to write the BOM when writing the file.
	*/
	void SetBOM( bool useBOM ) { writeBOM = useBOM; }

	/** Return the root element of DOM. Equivalent to FirstChildElement().
	    To get the first node, use FirstChild().
	*/
	CXMLElement* RootElement()				{ return FirstChildElement(); }
	const CXMLElement* RootElement() const	{ return FirstChildElement(); }

	/** Print the Document. If the Printer is not provided, it will
	    print to stdout. If you provide Printer, this can print to a file:
		@verbatim
		CXMLPrinter printer( fp );
		doc.Print( &printer );
		@endverbatim

		Or you can use a printer to print to memory:
		@verbatim
		CXMLPrinter printer;
		doc->Print( &printer );
		// printer.CStr() has a const char* to the XML
		@endverbatim
	*/
	void Print( CXMLPrinter* streamer=0 );
	virtual bool Accept( CXMLVisitor* visitor ) const;

	/**
		Create a new Element associated with
		this Document. The memory for the Element
		is managed by the Document.
	*/
	CXMLElement* NewElement( const char* name );
	/**
		Create a new Comment associated with
		this Document. The memory for the Comment
		is managed by the Document.
	*/
	CXMLComment* NewComment( const char* comment );
	/**
		Create a new Text associated with
		this Document. The memory for the Text
		is managed by the Document.
	*/
	CXMLText* NewText( const char* text );
	/**
		Create a new Declaration associated with
		this Document. The memory for the object
		is managed by the Document.

		If the 'text' param is null, the standard
		declaration is used.:
		@verbatim
			<?xml version="1.0" encoding="UTF-8"?>
		@endverbatim
	*/
	CXMLDeclaration* NewDeclaration( const char* text=0 );
	/**
		Create a new Unknown associated with
		this Document. The memory for the object
		is managed by the Document.
	*/
	CXMLUnknown* NewUnknown( const char* text );

	/**
		Delete a node associated with this documented.
		It will be unlinked from the DOM.
	*/
	void DeleteNode( CXMLNode* node )	{ node->parent->DeleteChild( node ); }

	void SetError( int error, const char* str1, const char* str2 );
	
	/// Return true if there was an error parsing the document.
	bool Error() const { return errorID != XML_NO_ERROR; }
	/// Return the errorID.
	int  ErrorID() const { return errorID; }
	/// Return a possibly helpful diagnostic location or string.
	const char* GetErrorStr1() const { return errorStr1; }
	/// Return possibly helpful secondary diagnostic location or string.
	const char* GetErrorStr2() const { return errorStr2; }
	/// If there is an error, print it to stdout
	void PrintError() const;

	// internal
	char* Identify( char* p, CXMLNode** node );

	virtual CXMLNode* ShallowClone( CXMLDocument* /*document*/ ) const	{ return 0; }
	virtual bool ShallowEqual( const CXMLNode* /*compare*/ ) const	{ return false; }

private:
	CXMLDocument( const CXMLDocument& );	// not supported
	void operator=( const CXMLDocument& );	// not supported
	void InitDocument();

	bool writeBOM;
	bool processEntities;
	int errorID;
	const char* errorStr1;
	const char* errorStr2;
	char* charBuffer;

	CXMLMemPoolT< sizeof(CXMLElement) >	elementPool;
	CXMLMemPoolT< sizeof(CXMLAttribute) > attributePool;
	CXMLMemPoolT< sizeof(CXMLText) >		textPool;
	CXMLMemPoolT< sizeof(CXMLComment) >	commentPool;
};


/**
	A XMLHandle is a class that wraps a node pointer with null checks; this is
	an incredibly useful thing. Note that XMLHandle is not part of the TinyXML
	DOM structure. It is a separate utility class.

	Take an example:
	@verbatim
	<Document>
		<Element attributeA = "valueA">
			<Child attributeB = "value1" />
			<Child attributeB = "value2" />
		</Element>
	<Document>
	@endverbatim

	Assuming you want the value of "attributeB" in the 2nd "Child" element, it's very 
	easy to write a *lot* of code that looks like:

	@verbatim
	CXMLElement* root = document.FirstChildElement( "Document" );
	if ( root )
	{
		CXMLElement* element = root->FirstChildElement( "Element" );
		if ( element )
		{
			CXMLElement* child = element->FirstChildElement( "Child" );
			if ( child )
			{
				CXMLElement* child2 = child->NextSiblingElement( "Child" );
				if ( child2 )
				{
					// Finally do something useful.
	@endverbatim

	And that doesn't even cover "else" cases. XMLHandle addresses the verbosity
	of such code. A XMLHandle checks for null pointers so it is perfectly safe 
	and correct to use:

	@verbatim
	XMLHandle docHandle( &document );
	CXMLElement* child2 = docHandle.FirstChild( "Document" ).FirstChild( "Element" ).FirstChild().NextSibling().ToElement();
	if ( child2 )
	{
		// do something useful
	@endverbatim

	Which is MUCH more concise and useful.

	It is also safe to copy handles - internally they are nothing more than node pointers.
	@verbatim
	XMLHandle handleCopy = handle;
	@endverbatim

	See also XMLConstHandle, which is the same as XMLHandle, but operates on const objects.
*/
class XMLHandle
{
public:
	/// Create a handle from any node (at any depth of the tree.) This can be a null pointer.
	XMLHandle( CXMLNode* _node )												{ node = _node; }
	/// Create a handle from a node.
	XMLHandle( CXMLNode& _node )												{ node = &_node; }
	/// Copy constructor
	XMLHandle( const XMLHandle& ref )										{ node = ref.node; }
	/// Assignment
	XMLHandle operator=( const XMLHandle& ref )								{ node = ref.node; return *this; }

	/// Get the first child of this handle.
	XMLHandle FirstChild() 													{ return XMLHandle( node ? node->FirstChild() : 0 ); }
	/// Get the first child element of this handle.
	XMLHandle FirstChildElement( const char* value=0 )						{ return XMLHandle( node ? node->FirstChildElement( value ) : 0 ); }
	/// Get the last child of this handle.
	XMLHandle LastChild()													{ return XMLHandle( node ? node->LastChild() : 0 ); }
	/// Get the last child element of this handle.
	XMLHandle LastChildElement( const char* _value=0 )						{ return XMLHandle( node ? node->LastChildElement( _value ) : 0 ); }
	/// Get the previous sibling of this handle.
	XMLHandle PreviousSibling()												{ return XMLHandle( node ? node->PreviousSibling() : 0 ); }
	/// Get the previous sibling element of this handle.
	XMLHandle PreviousSiblingElement( const char* _value=0 )				{ return XMLHandle( node ? node->PreviousSiblingElement( _value ) : 0 ); }
	/// Get the next sibling of this handle.
	XMLHandle NextSibling()													{ return XMLHandle( node ? node->NextSibling() : 0 ); }		
	/// Get the next sibling element of this handle.
	XMLHandle NextSiblingElement( const char* _value=0 )					{ return XMLHandle( node ? node->NextSiblingElement( _value ) : 0 ); }

	/// Safe cast to CXMLNode. This can return null.
	CXMLNode* ToNode()							{ return node; } 
	/// Safe cast to CXMLElement. This can return null.
	CXMLElement* ToElement() 					{ return ( ( node && node->ToElement() ) ? node->ToElement() : 0 ); }
	/// Safe cast to CXMLText. This can return null.
	CXMLText* ToText() 							{ return ( ( node && node->ToText() ) ? node->ToText() : 0 ); }
	/// Safe cast to CXMLUnknown. This can return null.
	CXMLUnknown* ToUnknown() 					{ return ( ( node && node->ToUnknown() ) ? node->ToUnknown() : 0 ); }
	/// Safe cast to CXMLDeclaration. This can return null.
	CXMLDeclaration* ToDeclaration() 			{ return ( ( node && node->ToDeclaration() ) ? node->ToDeclaration() : 0 ); }

private:
	CXMLNode* node;
};


/**
	A variant of the XMLHandle class for working with const CXMLNodes and Documents. It is the
	same in all regards, except for the 'const' qualifiers. See XMLHandle for API.
*/
class XMLConstHandle
{
public:
	XMLConstHandle( const CXMLNode* _node )											{ node = _node; }
	XMLConstHandle( const CXMLNode& _node )											{ node = &_node; }
	XMLConstHandle( const XMLConstHandle& ref )										{ node = ref.node; }

	XMLConstHandle operator=( const XMLConstHandle& ref )							{ node = ref.node; return *this; }

	const XMLConstHandle FirstChild() const											{ return XMLConstHandle( node ? node->FirstChild() : 0 ); }
	const XMLConstHandle FirstChildElement( const char* value=0 ) const				{ return XMLConstHandle( node ? node->FirstChildElement( value ) : 0 ); }
	const XMLConstHandle LastChild()	const										{ return XMLConstHandle( node ? node->LastChild() : 0 ); }
	const XMLConstHandle LastChildElement( const char* _value=0 ) const				{ return XMLConstHandle( node ? node->LastChildElement( _value ) : 0 ); }
	const XMLConstHandle PreviousSibling() const									{ return XMLConstHandle( node ? node->PreviousSibling() : 0 ); }
	const XMLConstHandle PreviousSiblingElement( const char* _value=0 ) const		{ return XMLConstHandle( node ? node->PreviousSiblingElement( _value ) : 0 ); }
	const XMLConstHandle NextSibling() const										{ return XMLConstHandle( node ? node->NextSibling() : 0 ); }
	const XMLConstHandle NextSiblingElement( const char* _value=0 ) const			{ return XMLConstHandle( node ? node->NextSiblingElement( _value ) : 0 ); }


	const CXMLNode* ToNode() const				{ return node; } 
	const CXMLElement* ToElement() const			{ return ( ( node && node->ToElement() ) ? node->ToElement() : 0 ); }
	const CXMLText* ToText() const				{ return ( ( node && node->ToText() ) ? node->ToText() : 0 ); }
	const CXMLUnknown* ToUnknown() const			{ return ( ( node && node->ToUnknown() ) ? node->ToUnknown() : 0 ); }
	const CXMLDeclaration* ToDeclaration() const	{ return ( ( node && node->ToDeclaration() ) ? node->ToDeclaration() : 0 ); }

private:
	const CXMLNode* node;
};


/**
	Printing functionality. The CXMLPrinter gives you more
	options than the CXMLDocument::Print() method.

	It can:
	-# Print to memory.
	-# Print to a file you provide
	-# Print XML without a CXMLDocument.

	Print to Memory

	@verbatim
	CXMLPrinter printer;
	doc->Print( &printer );
	SomeFunctior( printer.CStr() );
	@endverbatim

	Print to a File
	
	You provide the file pointer.
	@verbatim
	CXMLPrinter printer( fp );
	doc.Print( &printer );
	@endverbatim

	Print without a CXMLDocument

	When loading, an XML parser is very useful. However, sometimes
	when saving, it just gets in the way. The code is often set up
	for streaming, and constructing the DOM is just overhead.

	The Printer supports the streaming case. The following code
	prints out a trivially simple XML file without ever creating
	an XML document.

	@verbatim
	CXMLPrinter printer( fp );
	printer.OpenElement( "foo" );
	printer.PushAttribute( "foo", "bar" );
	printer.CloseElement();
	@endverbatim
*/
class CXMLPrinter : public CXMLVisitor
{
public:
	/** Construct the printer. If the FILE* is specified,
		this will print to the FILE. Else it will print
		to memory, and the result is available in CStr()
	*/
	CXMLPrinter( FILE* file=0 );
	~CXMLPrinter()	{}

	/** If streaming, write the BOM and declaration. */
	void PushHeader( bool writeBOM, bool writeDeclaration );
	/** If streaming, start writing an element.
	    The element must be closed with CloseElement()
	*/
	void OpenElement( const char* name );
	/// If streaming, add an attribute to an open element.
	void PushAttribute( const char* name, const char* value );
	void PushAttribute( const char* name, int value );
	void PushAttribute( const char* name, unsigned value );
	void PushAttribute( const char* name, bool value );
	void PushAttribute( const char* name, double value );
	/// If streaming, close the Element.
	void CloseElement();

	/// Add a text node.
	void PushText( const char* text, bool cdata=false );
	/// Add a comment
	void PushComment( const char* comment );

	void PushDeclaration( const char* value );
	void PushUnknown( const char* value );

	virtual bool VisitEnter( const CXMLDocument& /*doc*/ );
	virtual bool VisitExit( const CXMLDocument& /*doc*/ )			{ return true; }

	virtual bool VisitEnter( const CXMLElement& element, const CXMLAttribute* attribute );
	virtual bool VisitExit( const CXMLElement& element );

	virtual bool Visit( const CXMLText& text );
	virtual bool Visit( const CXMLComment& comment );
	virtual bool Visit( const CXMLDeclaration& declaration );
	virtual bool Visit( const CXMLUnknown& unknown );

	/**
		If in print to memory mode, return a pointer to
		the XML file in memory.
	*/
	const char* CStr() const { return buffer.Mem(); }

private:
	void SealElement();
	void PrintSpace( int depth );
	void PrintString( const char*, bool restrictedEntitySet );	// prints out, after detecting entities.
	void Print( const char* format, ... );

	bool elementJustOpened;
	bool firstElement;
	FILE* fp;
	int depth;
	int textDepth;
	bool processEntities;

	enum {
		ENTITY_RANGE = 64,
		BUF_SIZE = 200
	};
	bool entityFlag[ENTITY_RANGE];
	bool restrictedEntityFlag[ENTITY_RANGE];

	CXMLDynArray< const char*, 10 > stack;
	CXMLDynArray< char, 20 > buffer, accumulator;
};


//}	// tinyxml2


#endif // TINYXML2_INCLUDED