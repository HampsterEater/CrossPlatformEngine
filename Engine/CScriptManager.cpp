///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CScriptManager.h"
#include "CScriptCompileContext.h"
#include "CScriptLexer.h"
#include "CScriptParser.h"
#include "CScriptGenerator.h"
#include "CScriptVirtualMachine.h"
#include "CFileSystem.h"

using namespace Engine::Scripting;

Engine::Memory::Allocators::CProxyAllocator* Engine::Scripting::g_script_allocator = NULL;

void Engine::Scripting::InitScriptAllocator()
{
    Engine::Memory::Allocators::CAllocator* alloc = Engine::Memory::GetDefaultAllocator();
    Engine::Scripting::g_script_allocator = alloc->NewObj<Engine::Memory::Allocators::CProxyAllocator>("Script Allocator", alloc);
}

void Engine::Scripting::FreeScriptAllocator()
{
    Engine::Memory::GetDefaultAllocator()->FreeObj(&Engine::Scripting::g_script_allocator);
    Engine::Scripting::g_script_allocator = NULL;
}

CScriptManager::CScriptManager(Engine::FileSystem::CFileSystem* fileSystem)
{
	_fileSystem = fileSystem;

	_vm = GetScriptAllocator()->NewObj<CScriptVirtualMachine>();
	_vm->LoadNativeLibrary();
}

CScriptManager::~CScriptManager()
{
	Engine::Containers::CArray<CScriptExecutionContext*> contexts = _vm->GetContexts();

	// Unload any scripts that haven't already been unloaded.
	for (u32 i = 0; i < contexts.Size(); i++)
	{
		Unload(contexts[i]);
	}

	// Destroy all compile contexts we are holding.
	for (u32 i = 0; i < _compileContexts.Size(); i++)
	{
		CScriptCompileContext* context = _compileContexts[i];
		GetScriptAllocator()->FreeObj(&context);
		_compileContexts.Remove(context);
	}

	// Destroy the VM.
	GetScriptAllocator()->FreeObj(&_vm);
}

const CScriptVirtualMachine* CScriptManager::GetVM()
{
	return _vm;
}

void CScriptManager::Run(f32 timeslice)
{
	_vm->Run(timeslice);
}

CScriptCompileContext* CScriptManager::CompileString(const Engine::Containers::CString& source, const Engine::Containers::CString& file)
{
	CScriptCompileContext*	context = GetScriptAllocator()->NewObj<CScriptCompileContext>(source, file);
	CScriptLexer			lexer;
	CScriptParser			parser;
	CScriptGenerator		generator;

	_compileContexts.AddToEnd(context);

	if (lexer.Analyze(context))
	{
		if (parser.Analyze(context))
		{
			generator.Analyze(context);			
		}
	}

	return context;
}

CScriptCompileContext* CScriptManager::CompileFile(const Engine::Containers::CString& path)
{
	CScriptCompileContext* context = NULL;
	Engine::FileSystem::Streams::CStream* s = _fileSystem->CreateReadStream(path);
	if (s != NULL)
	{
		if (s->Open())
		{
			context = CompileString(s->ReadToEnd(), path);			
			s->Close();
		}
	
		_fileSystem->DestroyStream(s);
	}
	return context;
}				

CScriptExecutionContext* CScriptManager::Load(CScriptCompileContext* compile_context)
{
	LOG_ASSERT_MSG(compile_context->GetErrorCount(SCRIPT_ERROR_FATAL) <= 0, "Attempt to load compile context that contains compile errors.");
	
	CScriptExecutionContext* executionContext = GetScriptAllocator()->NewObj<CScriptExecutionContext>(compile_context);

	_vm->AddContext(executionContext);

	return executionContext;
}

void CScriptManager::Unload(CScriptExecutionContext* context)
{
	_vm->RemoveContext(context);
	GetScriptAllocator()->FreeObj(&context);
}