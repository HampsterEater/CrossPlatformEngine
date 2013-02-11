///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

namespace Engine
{
    namespace Threading
    {
		class CAsyncTask
		{
		public:			
			void Run() = 0;
		};

		class CAsyncJob
		{
			public:
				CConditionVariable();
				~CConditionVariable();


				void IsRunning();

				void AddTask();
				void Wait();

				void Results();
		};

	}
}