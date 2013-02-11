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

		class CSemaphore
		{
			private:
				Engine::Platform::SemaphoreHandle _semaphoreHandle;

			public:
				CSemaphore();
				~CSemaphore();

				void Wait(u32 timeout=0);
				void Post();
		};

	}
}