///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

#include "Platform.h"

#include "CMutex.h"

namespace Engine
{
    namespace Threading
    {

		class CConditionVariable
		{
			private:
				Engine::Platform::ConditionVarHandle _conVarHandle;

			public:
				CConditionVariable();
				~CConditionVariable();

				void Broadcast();
				void Signal();
				bool Wait(Engine::Threading::CMutex* mutex, u32 timeout=0);
		};

	}
}