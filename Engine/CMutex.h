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
		class CConditionVariable;

		class CMutex
		{
			private:
				Engine::Platform::MutexHandle _mutexHandle;

			public:
				CMutex();
				~CMutex();

				void Lock();
				void Unlock();
				bool TryLock();

			// Not a fan of friend classes but this stop us having to expose the platform
			// interface through some form of GetHandle() function. 			
			friend class CConditionVariable;

		};

	}
}