///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CString.h"
#include "CArray.h"
#include "CHashTable.h"
#include "CList.h"

#include "Platform.h"

#include "Memory.h"
#include "CAllocator.h"
#include "CProxyAllocator.h"

#include "CScriptSymbol.h"
#include "CScriptFunctionSymbol.h"
#include "CScriptVariableSymbol.h"
#include "CScriptCompileContext.h"
#include "CScriptInstruction.h"

#include "CScriptObject.h"

using namespace Engine::Scripting::Symbols;
using namespace Engine::Scripting::Instructions;
using namespace Engine::Scripting::Objects;

namespace Engine
{
    namespace Scripting
    {
		namespace Objects
		{
			class CScriptContextIteratorObject;
		}

		// Prototypes
		class CScriptVirtualMachine;
		class CScriptNativeFunction;

		// How many instructions between each time we should check our timeslice.
		#define SCRIPT_VM_TIMESLICE_CHECK_INTERVAL	50
		#define SCRIPT_VM_GC_INTERVAL				1000	// Generation 0 is done every time we run this many instructions, generation 1 runs every this*10 instructions, generation 2 is this*100 etc.
		#define SCRIPT_MAX_GC_GENERATIONS			3

		// Defines the value currently being stored by a script value.
		enum ScriptValueType
		{
			SCRIPT_VALUE_TYPE_INT,
			SCRIPT_VALUE_TYPE_FLOAT,
			SCRIPT_VALUE_TYPE_OBJECT,	
			SCRIPT_VALUE_TYPE_SYMBOL,			// Holds a symbol reference.
			SCRIPT_VALUE_TYPE_FUNCTION,			// Holds the entry point of a function.
			SCRIPT_VALUE_TYPE_NATIVE_FUNCTION,	// Holds a native function reference.
			SCRIPT_VALUE_TYPE_NULL,		
		};
		u8 const* const ScriptValueType_String[] =
		{
			// Primitive types.
			"int",
			"float",

			// Strings, contexts, lists, dicts, modules, etc
			"object",		
			
			// Holds a symbol reference. No point turning this into an object
			// seeing as we don't need garbage collection, and the script
			// can never access these directly (values using these are usually internal to the compiler).
			"symbol",		

			// Holds the entry point of a function.	No point turning this into an object,
			// as we need no garbage collection and it can just be stored in the intliteral field.
			"function",

			// Holds a native function definition.
			"native",

			// Special "null" data type.
			"null"
		};

		// A script value struct stores the current state
		// of a typeless value within a script (variable, parameters, etc).
		class CScriptValue
		{
		public:
			ScriptValueType Type;
			union
			{
				s32											IntValue;			// Also stores integer.
				f32											FloatValue;

				CScriptSymbol*								Symbol;				
				Engine::Scripting::Objects::CScriptObject*	Object;				// List/Dict/Object/Context

				CScriptNativeFunction*						NativeFunction;
			};
			//Engine::Containers::CString	StringValue; // String is not allowed in union :( Silly constructors.
		};

		// A call stack entry stores a single function call
		// thats currently in the callstack of an execution
		// context.
		class CScriptCallContext
		{
		private:

		public:
			Symbols::CScriptFunctionSymbol*			Symbol;
			u32										PC;
			u32										LocalCount;
			CScriptValue*							Locals;
			CScriptValue							Registers[SCRIPT_TOTAL_REGISTERS];
			Objects::CScriptContextIteratorObject*	GeneratorIterator;

			void Dispose();

			CScriptCallContext()
			{
				GeneratorIterator = NULL;

				Registers[SCRIPT_CONST_REGISTER_ZERO].Type = SCRIPT_VALUE_TYPE_INT;
				Registers[SCRIPT_CONST_REGISTER_ZERO].IntValue = 0;
				Registers[SCRIPT_CONST_REGISTER_ONE].Type = SCRIPT_VALUE_TYPE_INT;
				Registers[SCRIPT_CONST_REGISTER_ONE].IntValue = 1;
				Registers[SCRIPT_CONST_REGISTER_CMP].Type = SCRIPT_VALUE_TYPE_INT;
				Registers[SCRIPT_CONST_REGISTER_CMP].IntValue = 0;

				Locals = NULL;

				Symbol = NULL;
			}

		};

		// An execution context stores the state of a single
		// currently executing script.
		class CScriptExecutionContext
		{
		private:
			CScriptCompileContext*					_context;
			Instructions::CScriptInstruction**		_instructions;
			Symbols::CScriptSymbol**				_symbols;

			CScriptVirtualMachine*					_virtualMachine;

			// Global variables / scope stuff.
			bool									_globalScopeRun;
			CScriptValue*							_globals;
			CScriptValue*							_functionTable;

			// Instruction tracking / call stack.
			Containers::CArray<CScriptCallContext>	_callStack;
			CScriptCallContext*						_currentContext;
			Containers::CArray<CScriptValue>		_parameterStack;

			// GC Stack.
			CScriptObject*							_gcObjectPool[SCRIPT_MAX_GC_GENERATIONS];
			bool									_gcObjectPoolDirty[SCRIPT_MAX_GC_GENERATIONS];
			u32										_gcLastRun;

			// Statistics.
			u32										_instructionsExecuted;
			f32										_instructionTimer;

			// Native call information.
			u32										_nativeFunctionParameterCount;
			Engine::Containers::CString				_nativeFunctionIdentifier;

			// State based information.
			CScriptStateSymbol*																_state;
			bool																			_globalScopeSymbolHashTableCreated;
			Engine::Containers::CHashTable<CScriptSymbol*>									_globalScopeHashTable;
			Engine::Containers::CHashTable<Engine::Containers::CHashTable<CScriptSymbol*>*>	_stateScopeHashTable;

			// Data type bits and pieces.
			FORCE_INLINE Engine::Containers::CString	GetDataTypeName		(const CScriptValue& value);

			// Executs the next instruction in the script.
			FORCE_INLINE  bool	Execute										();

			// Coercion functions.
			FORCE_INLINE  s32							CompareValues		(CScriptValue& src, CScriptValue& dest);
			FORCE_INLINE  s32							CoerceToInt			(const CScriptValue& value);
			FORCE_INLINE  f32							CoerceToFloat		(const CScriptValue& value);
			FORCE_INLINE  Engine::Containers::CString   CoerceToString		(const CScriptValue& value);
			FORCE_INLINE  CScriptObject*				CoerceToObject		(const CScriptValue& value);
			FORCE_INLINE  void							ImplicitCast		(CScriptValue& src, CScriptValue& dest);

			FORCE_INLINE  void							AssignTo			(CScriptValue& dest, CScriptValue& to, bool binary=false);

			FORCE_INLINE  void							PushCallContext		(CScriptCallContext& context);
			FORCE_INLINE  void							PopCallContext		();

			FORCE_INLINE void							GCExecute			();
			FORCE_INLINE void							GCCollectGeneration	(u32 generation);

			// Invokes a script symbol.
			bool										InvokeFunction		(Symbols::CScriptFunctionSymbol* symbol, u32 paramCount=0);

			// Symbol hash table rebuilding.
			void										RebuildGlobalScopeSymbolHashTable	();
			void										RebuildStateScopeSymbolHashTable	(Symbols::CScriptStateSymbol* symbol);

		public:
			
			CScriptExecutionContext		(CScriptCompileContext* context);
			~CScriptExecutionContext	();

			// These are made public for the use of object classes.
			s32							CoerceValueToInt		(const CScriptValue& value);
			f32							CoerceValueToFloat		(const CScriptValue& value);
			Engine::Containers::CString CoerceValueToString		(const CScriptValue& value);
			CScriptObject*				CoerceValueToObject		(const CScriptValue& value);

			s32							CompareScriptValues		(CScriptValue& src, CScriptValue& dest);
			
			// Should not really be public, but here to allow objects to modify the internal gc
			// state.
			void	GCAdd				(CScriptObject* object, u32 generation=0);
			void	GCRemove			(CScriptObject* object);
			void	GCCollect			();

			// Runs the context as much as possible within the timeslice given.
			void	Run					(f32 timeslice = 0);
			void	RunGlobalScope		();

			// For script->native calling.
			u32							GetParameterCount		();
			
			CScriptValue				GetParameter			(u32 index);
			s32							GetIntParameter			(u32 index);
			f32							GetFloatParameter		(u32 index);
			Engine::Containers::CString	GetStringParameter		(u32 index);
			CScriptObject*				GetObjectParameter		(u32 index);
			
			CScriptValue				GetReturnValue			();
			s32							GetReturnIntValue		();
			f32							GetReturnFloatValue		();
			Engine::Containers::CString	GetReturnStringValue	();
			CScriptObject*				GetReturnObjectValue	();

			void						SetReturnValue			(const CScriptValue& param);
			void						SetReturnIntValue		(s32 param);
			void						SetReturnFloatValue		(f32 param);
			void						SetReturnStringValue	(const Engine::Containers::CString& str);
			void						SetReturnObjectValue	(CScriptObject* obj);

			// For native->script calling.
			void						PassStringParameter		(const Engine::Containers::CString& param);
			void						PassIntParameter		(s32 param);
			void						PassFloatParameter		(f32 param);
			void						PassObjectParameter		(CScriptObject* obj);
			void						PassParameter			(const CScriptValue& param);

			bool						CallFunction			(const Engine::Containers::CString& name, u32 parameterCount=0);
			bool						CallFunction			(Symbols::CScriptFunctionSymbol* symbol, u32 parameterCount=0);

			// Event invokation.
			bool						CallEvent				(const Engine::Containers::CString& name, u32 parameterCount=0, bool async=true, bool stackable=true);
			bool						CallEvent				(Symbols::CScriptFunctionSymbol* symbol, u32 parameterCount=0, bool async=true, bool stackable=true);

			// Symbol retrieval.
			Symbols::CScriptFunctionSymbol*	GetFunctionSymbol	(const Engine::Containers::CString& name);
			Symbols::CScriptVariableSymbol*	GetVariableSymbol	(const Engine::Containers::CString& name);
			void							ChangeState			(Symbols::CScriptStateSymbol* symbol);

			// Getting values.
			/*
			CScriptSymbol*				GetSymbol				(const Engine::Containers::CString& name, u32 types=0, u32 excluding=0);

			Engine::Containers::CString	GetGlobalStringVariable	(const Engine::Containers::CString& name);
			s32							GetGlobalIntVariable	(const Engine::Containers::CString& name);
			f32							GetGlobalFloatVariable	(const Engine::Containers::CString& name);
			CScriptObject*				GetGlobalObjectVariable	(const Engine::Containers::CString& name);
			CScriptValue				GetGlobalVariable		(const Engine::Containers::CString& name);

			Engine::Containers::CString	GetGlobalStringVariable	(Symbols::CScriptVariableSymbol* symbol);
			s32							GetGlobalIntVariable	(Symbols::CScriptVariableSymbol* symbol);
			f32							GetGlobalFloatVariable	(Symbols::CScriptVariableSymbol* symbol);
			CScriptObject*				GetGlobalObjectVariable	(Symbols::CScriptVariableSymbol* symbol);
			CScriptValue				GetGlobalVariable		(Symbols::CScriptVariableSymbol* symbol);

			void						SetGlobalStringVariable	(const Engine::Containers::CString& name, const Engine::Containers::CString& val);
			void						SetGlobalIntVariable	(const Engine::Containers::CString& name, s32 param);
			void						SetGlobalFloatVariable	(const Engine::Containers::CString& name, f32 param);
			void						SetGlobalObjectVariable	(const Engine::Containers::CString& name, CScriptObject* obj);
			void						SetGlobalVariable		(const Engine::Containers::CString& name, const CScriptValue& param);

			void						SetGlobalStringVariable	(Symbols::CScriptVariableSymbol* symbol, const Engine::Containers::CString& val);
			void						SetGlobalIntVariable	(Symbols::CScriptVariableSymbol* symbol, s32 param);
			void						SetGlobalFloatVariable	(Symbols::CScriptVariableSymbol* symbol, f32 param);
			void						SetGlobalObjectVariable	(Symbols::CScriptVariableSymbol* symbol, CScriptObject* obj);
			void						SetGlobalVariable		(Symbols::CScriptVariableSymbol* symbol, const CScriptValue& param);
			*/

			// Error types.
			void	Error					(const Engine::Containers::CString& str, Instructions::CScriptInstruction* instruction=NULL);
			void	InvalidOp				(const Engine::Containers::CString& op, const CScriptValue& value, Instructions::CScriptInstruction* instruction=NULL);
			void	InvalidIndex			(const CScriptValue& obj, s32 index, Instructions::CScriptInstruction* instruction=NULL);
			void	InvalidIndex			(const CScriptValue& obj, const CScriptValue& key, Instructions::CScriptInstruction* instruction=NULL);
			void	ImmutableError			(const CScriptValue& obj, Instructions::CScriptInstruction* instruction=NULL);
			void	ImmutableError			(const Engine::Containers::CString& str, Instructions::CScriptInstruction* instruction=NULL);
			void	DuplicateIndex			(const CScriptValue& obj, const CScriptValue& key, Instructions::CScriptInstruction* instruction=NULL);
			void	InvalidCast				(const CScriptValue& obj, const Engine::Containers::CString& type, Instructions::CScriptInstruction* instruction=NULL);
			void	InvalidParameterCount	(u32 expectedParamCount, Instructions::CScriptInstruction* instruction=NULL);
			void	UniterableObject		(const CScriptValue& obj, Instructions::CScriptInstruction* instruction=NULL);

			friend class CScriptVirtualMachine;
			friend class CScriptContextIteratorObject;
		};
		
		// Prototype of a native function.
		typedef void (*ScriptNativeFunctionPrototype)(CScriptExecutionContext*);

		// Used to register a function available to scripts. This should be created
		// statically. Or better use the code glue generator to do all of this for you.
		class CScriptNativeFunction
		{
		public:

			const u8*					  Name;
			ScriptNativeFunctionPrototype FunctionPtr;

			CScriptNativeFunction(const u8* name, ScriptNativeFunctionPrototype funcPtr);

		};

		// The virtual machine class looks after executing all the 
		// different script processes currently being run by the game.
		class CScriptVirtualMachine
		{
		private:

			Engine::Containers::CArray<CScriptExecutionContext*>	_contexts;

			Engine::Containers::CHashTable<CScriptNativeFunction*>	_nativeFunctions;

		public:

			CScriptVirtualMachine			();
			~CScriptVirtualMachine			();

			// Runs all executions contexts with a share of the given timeslice.
			void	Run						(f32 timeslice = 0);

			// Add/Remove execution contexts.
			void														AddContext				(CScriptExecutionContext* context);
			void														RemoveContext			(CScriptExecutionContext* context);
			const Engine::Containers::CArray<CScriptExecutionContext*>&	GetContexts				();

			// Module loading.
		//	void	SetModulePath			(const Engine::Containers::CString& path);
		//	void	LoadModule				(const Engine::Containers::CString& name);

			// Register native bindings.
			void					RegisterNativeFunction	(const u8* name, ScriptNativeFunctionPrototype funcPtr);
			CScriptNativeFunction*	FindNativeFunction		(const Engine::Containers::CString& name);

			void					LoadNativeLibrary		();
		};

	}
}