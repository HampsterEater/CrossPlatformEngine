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

		class CThreadLocalData
		{
			private:
				Engine::Platform::ThreadLocalDataHandle _tldHandle;

			public:
				CThreadLocalData();
				CThreadLocalData(void* value);
				~CThreadLocalData();
				
				CThreadLocalData & operator=(void* &rhs);
				void* operator()();

				void  Set(void* ptr);
				void* Get();
		};

	}
}