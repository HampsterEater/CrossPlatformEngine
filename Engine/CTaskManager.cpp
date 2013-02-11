///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CLog.h"
#include "Memory.h"
#include "CTaskManager.h"
#include "CThread.h"
#include "CConditionVariable.h"
#include "CMutex.h"

#include <stdio.h>

using namespace Engine::Core::Tasks;

void CTask::Reset()
{
	ID				= -1;
	Work			= NULL;
	TaskRemaining	= 0;
	Parent			= 0;
	Dependency		= -1;
}

s32 CTaskManager::WorkerThreadEntryPoint(void* meta)
{
	CTaskManager* manager = (CTaskManager*)meta;

	while (!manager->_workersExiting)
	{
		CTask* task = manager->WorkerWaitForTask();
		if (task != NULL)
		{
			try
			{
				task->Work->Run();
			}
			catch (...)
			{
				LOG_ASSERT_MSG(false, "Captured unhandled exception within worker thread.");
			}
			manager->WorkerTaskCompleted(task);
		}
	}

	return 0;
}

CTask* CTaskManager::GetTaskByID(TaskID id)
{
	if (id == -1)
		return NULL;

	for (u32 i = 0; i < TASK_MANAGER_MAX_TASKS; i++)
	{
		if (_tasks[i].ID == id)
		{
			return &_tasks[i];
		}
	}
	return NULL;
}

CTaskManager::CTaskManager()
{	
	for (u32 i = 0; i < TASK_MANAGER_MAX_WORKERS; i++)
	{
		_workerThreads[i] = NULL;
	}
	for (u32 i = 0; i < TASK_MANAGER_MAX_TASKS; i++)
	{
		_tasks[i].Reset();
	}

	_nextTaskID = 0;

	_workerTaskConVar       = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::Threading::CConditionVariable>();
	_workerTaskListMutex    = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::Threading::CMutex>();
	_workerTaskMutex        = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::Threading::CMutex>();
}

CTaskManager::~CTaskManager()
{
	DestoryWorkers();
	
	if (_workerTaskConVar != NULL)
	{
		Engine::Memory::GetDefaultAllocator()->FreeObj(&_workerTaskConVar);
	}
	if (_workerTaskListMutex != NULL)
	{
		Engine::Memory::GetDefaultAllocator()->FreeObj(&_workerTaskListMutex);
	}
	if (_workerTaskMutex != NULL)
	{
		Engine::Memory::GetDefaultAllocator()->FreeObj(&_workerTaskMutex);
	}
}

CTask* CTaskManager::WorkerWaitForTask(u32 timeout)
{
	CTask* task = GetAvailableTask();
	if (task != NULL)
		return task;

	_workerTaskMutex->Lock();
	if (_workerTaskConVar->Wait(_workerTaskMutex, timeout))
	{
		task = GetAvailableTask();
	}
	_workerTaskMutex->Unlock();

	return task;
}

CTask* CTaskManager::GetAvailableTask()
{
	CTask* task = NULL;

	if (_workersExiting == true)
		return task;

	_workerTaskListMutex->Lock();
	for (Engine::Containers::CListNode<CTask*>* node = _taskQueue.Start(); node != NULL; node = _taskQueue.Next(node))
	{
		if (node->Value->TaskRemaining == 1 && 
			GetTaskByID(node->Value->Dependency) == NULL)
		{
			_taskQueue.Remove(node->Value);
			task = node->Value;
			break;
		}
	}
	_workerTaskListMutex->Unlock();

	return task;
}

void CTaskManager::WorkerTaskCompleted(CTask* task)
{
	task->TaskRemaining--;
	task->ID = -1;
	
	CTask* parent = GetTaskByID(task->Parent);
	if (parent != NULL)
		parent->TaskRemaining--;
	
	_workerTaskMutex->Lock();
	_workerTaskConVar->Broadcast();
	_workerTaskMutex->Unlock();
}

void CTaskManager::WorkerPostNewTask(CTask* task)
{
	_workerTaskListMutex->Lock();
	_taskQueue.AddToEnd(task);
	_workerTaskListMutex->Unlock();
	
	_workerTaskMutex->Lock();
	_workerTaskConVar->Broadcast();
	_workerTaskMutex->Unlock();
}

void CTaskManager::CreateWorkers(u32 count)
{
	_workersExiting = false;
	count = min(count, TASK_MANAGER_MAX_WORKERS);
	for (u32 i = 0; i < count; i++)
	{
		LOG_ASSERT(_workerThreads[i] == NULL);
		_workerThreads[i] = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::Threading::CThread>(S("Worker Thread ") + i, WorkerThreadEntryPoint, this);
	}
}

void CTaskManager::DestoryWorkers()
{
	// Signal all workers to exit.
	_workersExiting = true;

	_workerTaskMutex->Lock();
	_workerTaskConVar->Broadcast();
	_workerTaskMutex->Unlock();
	
	// Wait for workers to stop.
	bool running = true;
	while (running == true)
	{
		// Anything still running?
		running = false;
		for (u32 i = 0; i < TASK_MANAGER_MAX_WORKERS; i++)
		{
			if (_workerThreads[i] != NULL && _workerThreads[i]->IsRunning())
			{
				running = true;
			}
		}

		Engine::Platform::Wait(0);
	}

	// Dispose of all workers.
	for (u32 i = 0; i < TASK_MANAGER_MAX_WORKERS; i++)
	{
		if (_workerThreads[i] != NULL)
		{
			Engine::Memory::GetDefaultAllocator()->FreeObj(&_workerThreads[i]);
		}
	}
}

void CTaskManager::PauseWorkers()
{
	for (u32 i = 0; i < TASK_MANAGER_MAX_WORKERS; i++)
	{
		if (_workerThreads[i] != NULL)
		{
			_workerThreads[i]->Pause();
		}
	}
}

void CTaskManager::ResumeWorkers()
{
	for (u32 i = 0; i < TASK_MANAGER_MAX_WORKERS; i++)
	{
		if (_workerThreads[i] != NULL)
		{
			_workerThreads[i]->Resume();
		}
	}

	// Give up our timeslice so the threads have some time to run and
	// progress to the sync point.
	Engine::Platform::Wait(0); 
}

TaskID CTaskManager::AddTask(Jobs::CTaskJob* work, TaskID parent)
{
	CTask* taskParent = GetTaskByID(parent);
	if (taskParent != NULL)
	{
		taskParent->TaskRemaining++;
	}

	for (u32 i = 0; i < TASK_MANAGER_MAX_TASKS; i++)
	{
		if (_tasks[i].TaskRemaining <= 0)
		{
			_tasks[i].Reset();
			_tasks[i].ID			= _nextTaskID++;
			_tasks[i].Parent		= parent;
			_tasks[i].TaskRemaining = 1;
			_tasks[i].Work			= work;

			// Wrap the task ID around, or we will end
			// up wrapping around and going into the negatives.
			if (_nextTaskID > TASK_MANAGER_MAX_TASK_ID)
			{
				_nextTaskID = 0;
			}

			return _tasks[i].ID;
		}
	}

	LOG_ASSERT_MSG(false, "Failed to add task into task queue, queue has overflowed!");
	return 0;
}

void CTaskManager::DependsOn(TaskID work, TaskID on)
{
	CTask* task = GetTaskByID(work);
	if (task != NULL)
	{
		task->Dependency = on;
	}
}

void CTaskManager::WaitFor(TaskID work)
{
	while (true)
	{
		// Task completed yet?
		CTask* task = GetTaskByID(work);
		if (task == NULL)
			return;

		// Run some tasks while we are waiting.
		CTask* runtask = WorkerWaitForTask(1);
		if (runtask != NULL)
		{
			try
			{
				runtask->Work->Run();
			}
			catch (...)
			{
				LOG_ASSERT_MSG(false, "Captured unhandled exception within worker thread.");
			}
			WorkerTaskCompleted(runtask);
		}
	}
}

void CTaskManager::QueueTask(TaskID work)
{
	CTask* task = GetTaskByID(work);
	if (task != NULL)
	{
		WorkerPostNewTask(task);
	}
}
