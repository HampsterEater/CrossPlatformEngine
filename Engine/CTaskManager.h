///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CList.h"

#include "CTaskJob.h"

namespace Engine
{
	namespace Threading
	{
		class CThread;
		class CConditionVariable;
		class CMutex;
	}
	namespace Core
	{
		namespace Tasks
		{
			namespace Jobs
			{
				class CTaskJob;
			}

			// Maximum number of workers that the task manager can 
			// register at any one time.
			#define TASK_MANAGER_MAX_WORKERS	32
			#define TASK_MANAGER_MAX_TASKS		256
			#define TASK_MANAGER_MAX_TASK_ID	1000000
			
			// Unique ID for the task, equates basically to an index in the task array.
			typedef s32 TaskID;

			// Contains information about a task being run within
			// the engine within the thread pool.
			struct CTask
			{
				TaskID			ID;
				Jobs::CTaskJob* Work;
				u32				TaskRemaining;
				TaskID			Parent;
				TaskID			Dependency;	

				void Reset();
			};

			// This is the base class that all games are inherited from.
			// It deals with all the fun underlying code used to get the game up and running.
			// To derive a game form this, just create your own class inherited from it
			// and override the virtual methods.
			class CTaskManager
			{
				private:

					TaskID								_nextTaskID;
					CTask								_tasks[TASK_MANAGER_MAX_TASKS];

					Engine::Containers::CList<CTask*>	_taskQueue;

					Engine::Threading::CConditionVariable*	_workerTaskConVar;
					Engine::Threading::CMutex*				_workerTaskListMutex;
					Engine::Threading::CMutex*				_workerTaskMutex;
					Engine::Threading::CThread*				_workerThreads[TASK_MANAGER_MAX_WORKERS];

					bool									_workersExiting;

					static s32 WorkerThreadEntryPoint	(void* meta);

					// Used internally by workers to syncronize themselves.
					CTask*	WorkerWaitForTask			(u32 timeout=0);
					void	WorkerPostNewTask			(CTask* task);
					void	WorkerTaskCompleted			(CTask* task);

					CTask*	GetAvailableTask			();
					CTask*	GetTaskByID					(TaskID id);

				public:

					CTaskManager			();
					~CTaskManager			();

					// Worker pool.
					void CreateWorkers		(u32 count);
					void DestoryWorkers		();
					void PauseWorkers		();
					void ResumeWorkers		();

					// Task management.
					TaskID		AddTask		(Jobs::CTaskJob* work, TaskID parent = -1);
					void		DependsOn	(TaskID work, TaskID on);
					void		WaitFor		(TaskID work);
					void		QueueTask	(TaskID work);

			};

		}

	}
}