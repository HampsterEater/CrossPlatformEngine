///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CLog.h"

#include "CTaskJob.h"

namespace Engine
{
	namespace Core
	{
		namespace Tasks
		{
			namespace Jobs
			{
			
				// This is just a debug task, it prints a message!
				class CDebugPrintTaskJob : public CTaskJob
				{
					private:
						Engine::Containers::CString	_msg;

					public:
					
						CDebugPrintTaskJob(const Engine::Containers::CString& msg)
						{
							_msg	  = msg;
						}

						virtual void Run()
						{
							LOG_DEBUG(_msg);
						}

				};

			}
		}
	}
}