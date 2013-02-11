///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

namespace Engine
{
	namespace Core
	{
		namespace Tasks
		{
			namespace Jobs
			{

				// This class is derived to implement actual
				// work for a task to do. Simply derive your copy and implement
				// the run method.
				class CTaskJob
				{
					private:

					public:
					
						virtual void Run() = 0;

				};

			}
		}
	}
}