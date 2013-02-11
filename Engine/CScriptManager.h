///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

#include "Platform.h"

#include "Memory.h"
#include "CAllocator.h"
#include "CProxyAllocator.h"

#include "CArray.h"

namespace Engine
{
	namespace FileSystem
	{
		class CFileSystem;
	}
	namespace Scripting
    {
		class CScriptCompileContext;
		class CScriptVirtualMachine;
		class CScriptExecutionContext;

		// Allocators!
		extern Engine::Memory::Allocators::CProxyAllocator* g_script_allocator;
		void InitScriptAllocator();
		void FreeScriptAllocator();
		inline Engine::Memory::Allocators::CProxyAllocator* GetScriptAllocator() { return Engine::Scripting::g_script_allocator; }

		// The script manager is the main interface to the scripting language, none
		// of the other classes should be used directly.
		class CScriptManager
		{
			private:
				CScriptVirtualMachine*								_vm;
				Engine::FileSystem::CFileSystem*					_fileSystem;
				Engine::Containers::CArray<CScriptCompileContext*>	_compileContexts;

			public:
				CScriptManager									(Engine::FileSystem::CFileSystem* fileSystem);
				~CScriptManager									();

				const CScriptVirtualMachine*	GetVM			();
				void							Run				(f32 timeslice = 0.0f);

				CScriptCompileContext*			CompileString	(const Engine::Containers::CString& str, const Engine::Containers::CString& file="<string>");
				CScriptCompileContext*			CompileFile		(const Engine::Containers::CString& path);
				
				CScriptExecutionContext*		Load			(CScriptCompileContext* compile_context);
				void							Unload			(CScriptExecutionContext* context);
		};

	}
}