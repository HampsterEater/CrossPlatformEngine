///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

#include "Platform.h"

namespace Engine
{
    namespace Threading
    {

		class CThread
		{

			private:
				Engine::Platform::ThreadHandle   _threadHandle;
				Engine::Containers::CString			 _name;

			public:
				CThread										 (const Engine::Containers::CString& name, Engine::Platform::THREAD_ENTRY_FUNCTION entry, void* args);
				~CThread									 ();

				void							 SetName	 (const Engine::Containers::CString& name);				
				Engine::Containers::CString		 GetName	 ();

				void							 Kill	     (s32 exitcode=1);
				void							 Resume	     ();
				void							 Pause	     ();

				bool							 Wait        (s32 timeout=0);
				bool							 IsRunning	 ();

				void							 SetPriority (Engine::Platform::Priority priority);
				Engine::Platform::Priority		 GetPriority ();

				void							 SetAffinity (u32 affinity);
				u32								 GetAffinity ();

				s32								 GetExitCode ();
				
		};

	}
}