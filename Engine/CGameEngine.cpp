///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CGameEngine.h"
#include "CLog.h"
#include "Memory.h"
#include "CTaskManager.h"
#include "Version.h"

#include "Config.h"

#include <stdio.h>

using namespace Engine::Core;

CGameEngine::CGameEngine(const u8* game_title, const u8* game_title_short)
{
	_exited			= false;
	_exitCode		= 0;
	_taskManager	= NULL;
	_fileSystem		= NULL;
	_scriptManager	= NULL;

	_gameTitle		= game_title;
	_gameTitleShort	= game_title_short;
}

CGameEngine::~CGameEngine()
{
}

s32 CGameEngine::RenderEntryPoint(void* meta)
{
	CGameEngine* engine = (CGameEngine*)meta;
	while (!engine->_exited)
	{
		engine->BaseRender();
	}
	return 0;
}

Tasks::CTaskManager* CGameEngine::GetTaskManager()
{
	return _taskManager;
}

Engine::FileSystem::CFileSystem* CGameEngine::GetFileSystem()
{
	return _fileSystem;
}

Engine::Scripting::CScriptManager* CGameEngine::GetScriptManager()
{
	return _scriptManager;
}

void CGameEngine::MainLoop()
{
	// Begin all threads.
	_taskManager->ResumeWorkers();
	_renderThread->Resume();

	// Run the update thread in this here main thread, until
	// we are requested to quit the game.
	while (!_exited)
	{
		BaseUpdate();
	}

	// Pause all threads.
	_renderThread->Pause();
	_taskManager->PauseWorkers();
}

void CGameEngine::BasePrintHeader()
{
	LOG_INFO("----------------------------------------------------");
	LOG_INFO("Icarus Game Engine");
#if DEBUG
	LOG_INFO(S("Version %s DEBUG (Built %s/%s/%s %s:%s)").Format(AutoVersion::FULLVERSION_STRING, AutoVersion::DAY, AutoVersion::MONTH, AutoVersion::YEAR, AutoVersion::HOUR, AutoVersion::MINUTE));
#else
	LOG_INFO(S("Version %s RELEASE (Built %s/%s/%s %s:%s)").Format(AutoVersion::FULLVERSION_STRING, AutoVersion::DAY, AutoVersion::MONTH, AutoVersion::YEAR, AutoVersion::HOUR, AutoVersion::MINUTE));
#endif
	LOG_INFO("----------------------------------------------------");

	LOG_INFO(Engine::Platform::GetPlatformString());
	LOG_INFO(S("Processors: ") + Engine::Platform::GetProcessorCount());

	// Command arguments.
	LOG_INFO(S("Working Directory: ")+Engine::Platform::PathGetWorkingDir());
	LOG_INFO(S("Executable: ")+Engine::Platform::GetLaunchExePath());
	Engine::Containers::CArray<Engine::Containers::CString> arguments = Engine::Platform::GetLaunchArguments();
	if (arguments.Size() != 0)
	{
		LOG_INFO("Arguments: ");
		for (u32 i = 0; i < arguments.Size(); i++)
			LOG_INFO(S("  [%i] = %s").Format(i, arguments[i].c_str()));
	}
	else
	{
		LOG_INFO("Arguments: None");
	}

	// Drive stats.
	Engine::Containers::CArray<Engine::Containers::CString> drives = Engine::Platform::PathGetRoots();

	LOG_INFO(S("FileSystem:"));
	LOG_INFO(S("  Drive").PadEnd(20, ' ') + S("Total").PadEnd(10, ' ') + S("Used").PadEnd(10, ' ') + S("Free").PadEnd(10, ' '));
	for (u32 i = 0; i < drives.Size(); i++)
	{
		Engine::Containers::CString drive = drives[i];
		u64 total = (Engine::Platform::PathGetRootTotalSpace(drive)/1024)/1024;
		u64 used  = (Engine::Platform::PathGetRootUsedSpace(drive)/1024)/1024;
		u64 free  = (Engine::Platform::PathGetRootFreeSpace(drive)/1024)/1024;

		LOG_INFO(S("  " + drive + "\\").PadEnd(20, ' ') + (S(total) + "MB").PadEnd(10, ' ') + (S(used) + "MB").PadEnd(10, ' ') + (S(free) + "MB").PadEnd(10, ' '));
	}

	// General stats.		
	LOG_INFO(S("Memory:"));
	LOG_INFO(S("  Type").PadEnd(20, ' ') + S("Total").PadEnd(10, ' ') + S("Used").PadEnd(10, ' ') + S("Free").PadEnd(10, ' '));
		
	u64 physTotal = (Engine::Platform::GetTotalMemory(Engine::Platform::MEMORY_TYPE_PHYSICAL) / 1024) / 1024;
	u64 physUsed  = (Engine::Platform::GetUsedMemory(Engine::Platform::MEMORY_TYPE_PHYSICAL) / 1024) / 1024;
	u64 physFree  = (Engine::Platform::GetFreeMemory(Engine::Platform::MEMORY_TYPE_PHYSICAL) / 1024) / 1024;
	LOG_INFO(S("  Physical").PadEnd(20, ' ') + S(S(physTotal) + "MB").PadEnd(10, ' ') + S(S(physUsed) + "MB").PadEnd(10, ' ') + S(S(physFree) + "MB").PadEnd(10, ' '));
		
	u64 virtTotal = (Engine::Platform::GetTotalMemory(Engine::Platform::MEMORY_TYPE_VIRTUAL) / 1024) / 1024;
	u64 virtUsed  = (Engine::Platform::GetUsedMemory(Engine::Platform::MEMORY_TYPE_VIRTUAL) / 1024) / 1024;
	u64 virtFree  = (Engine::Platform::GetFreeMemory(Engine::Platform::MEMORY_TYPE_VIRTUAL) / 1024) / 1024;
	LOG_INFO(S("  Virtual") .PadEnd(20, ' ') + S(S(virtTotal) + "MB").PadEnd(10, ' ') + S(S(virtUsed) + "MB").PadEnd(10, ' ') + S(S(virtFree) + "MB").PadEnd(10, ' '));
		
	u64 pageTotal = (Engine::Platform::GetTotalMemory(Engine::Platform::MEMORY_TYPE_PAGING) / 1024) / 1024;
	u64 pageUsed  = (Engine::Platform::GetUsedMemory(Engine::Platform::MEMORY_TYPE_PAGING) / 1024) / 1024;
	u64 pageFree  = (Engine::Platform::GetFreeMemory(Engine::Platform::MEMORY_TYPE_PAGING) / 1024) / 1024;
	LOG_INFO(S("  Paging")  .PadEnd(20, ' ') + S(S(pageTotal) + "MB").PadEnd(10, ' ') + S(S(pageUsed) + "MB").PadEnd(10, ' ') + S(S(pageFree) + "MB").PadEnd(10, ' '));
		
	LOG_INFO("----------------------------------------------------");

}

void CGameEngine::BaseRender()
{
	Render();
}

void CGameEngine::BaseUpdate()
{
	Update();
}

bool CGameEngine::SetupFileSystem()
{
	// Work out locations of all appropriate folders.
	Engine::Containers::CString rootFolder       = Engine::Platform::PathJoin(Engine::Platform::PathGetWorkingDir(), ENGINE_ASSET_ROOT_FOLDER);
	Engine::Containers::CString saveFolder       = Engine::Platform::PathJoin(Engine::Platform::PathJoin(Engine::Platform::PathGetSpecialDir(Engine::Platform::SPECIAL_DIR_APPDATA), _gameTitleShort), ENGINE_ASSET_SAVE_FOLDER);
	Engine::Containers::CString dataFile         = Engine::Platform::PathJoin(Engine::Platform::PathGetWorkingDir(), ENGINE_ASSET_DATA_FILE);
	Engine::Containers::CString configFile       = Engine::Platform::PathJoin(Engine::Platform::PathGetWorkingDir(), ENGINE_CONFIG_FILE);
	Engine::Containers::CString profileFile      = Engine::Platform::PathJoin(Engine::Platform::PathJoin(Engine::Platform::PathGetSpecialDir(Engine::Platform::SPECIAL_DIR_APPDATA), _gameTitleShort), ENGINE_PROFILE_FILE);
	
	// Root the file system to our working folder + /Assets/.
	// When loading files you can skip the root folder path and just use /Images/etc/whatever
	// This should be the only folder read from.
	LOG_INFO("Setting file system root to: %s", rootFolder.c_str());
	if (!_fileSystem->SetAssetFolder(rootFolder))
	{
		LOG_ERROR("Failed to root to expected '%s' assets folder, aborting.", rootFolder.c_str());
		return false;
	}

	// Change the save directory to the users application data folder.
	// This should be the only folder ever written to.
	LOG_INFO("Setting save root to: %s", saveFolder.c_str());
	if (!Engine::Platform::PathIsDirectory(saveFolder))
	{
		Engine::Platform::DirCreate(saveFolder, true);
	}
	if (!_fileSystem->SetSaveFolder(saveFolder))
	{
		LOG_ERROR("Failed to root to expected '%s' save folder, aborting.", saveFolder.c_str());
		return false;
	}

	// Can we load files from disk or do they have to be in container files?
#ifdef GAME_ASSET_FORCE_DATA_FILE
	_fileSystem->SetDiskFilesAllowed(false);
#else
	_fileSystem->SetDiskFilesAllowed(true);
#endif

	// If there is a data file available, then lets crack that bitch open :3.
	LOG_INFO("Looking for data file at: %s", dataFile.c_str());
	if (Engine::Platform::PathIsFile(dataFile))
	{
		if (!_fileSystem->AddPackage(dataFile))
		{
			LOG_ERROR("Failed to load asset data file from '%s', aborting.", dataFile.c_str());
			return false;
		}
		else
		{
			LOG_INFO("Loaded data file from: %s", dataFile.c_str());
		}
	}
	else
	{
#ifdef GAME_ASSET_FORCE_DATA_FILE
		LOG_ERROR("Failed to load asset data file from '%s', file is missing, aborting.", dataFile.c_str());
		return false;
#else	
		LOG_INFO("Failed to find data file, falling back to assets on disk.");
#endif
	}

	// Add any patch files that exist.
	u32 patch_index = 0;
	Engine::Containers::CString patchFile;
	while (true)
	{
		patchFile = dataFile + ".patch" + (patch_index++);
		LOG_INFO("Looking for patch file at: %s", patchFile.c_str());

		if (Engine::Platform::PathIsFile(patchFile))
		{
			if (!_fileSystem->AddPackage(patchFile))
			{
				LOG_ERROR("Failed to load asset data file patch from '%s', aborting.", patchFile.c_str());
				return false;
			}
			else
			{
				LOG_ERROR("Loaded patch file from: %s", patchFile.c_str());
			}
		}
		else
		{
			LOG_INFO("Failed to find patch file, finished looking for patchs.");
			break;
		}
	}

	// TODO: Load configuration files so we can do the below correctly.

	// Work out what attributes should be used when loading files.
	// Should be in priority:
	//		Platform		- Platform-specific resources should always take priority
	//		Language		- Selected language then takes priority.
	//		English			- If selected language is not available try and find one in english.
	//		Cooked			- Prefer a cooked file over a non-cooked file.
	//		Debug/Release	- Prefer a debug / release file depending on compile settings.
	Engine::Containers::CArray<Engine::Containers::CString> attributes;
	attributes.AddToEnd(Engine::Platform::GetPlatformShortName());
	attributes.AddToEnd("language");
	attributes.AddToEnd("cooked");
#ifdef DEBUG
	attributes.AddToEnd("debug");
#else
	attributes.AddToEnd("release");
#endif
	_fileSystem->SetAttributes(attributes);
	//"name.ps3.english.cooked.debg.pak"

	return true;
}

bool CGameEngine::BaseInitialize()
{
	// Create our file-system.
	_fileSystem = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::FileSystem::CFileSystem>();
	if (!SetupFileSystem())
		return false;

	// Create script manager.
	_scriptManager = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::Scripting::CScriptManager>(_fileSystem);

	// Create our task manager.
	_taskManager = Engine::Memory::GetDefaultAllocator()->NewObj<Tasks::CTaskManager>();

	// Create render thread.
	_renderThread = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::Threading::CThread>("Render Thread", RenderEntryPoint, this);

	// Create the task managers workers.
	_taskManager->CreateWorkers(max((s32)Engine::Platform::GetProcessorCount() - 2, 0));

	// Initialize the game.
	return Initialize();
}

bool CGameEngine::BaseDeinitialize()
{
	// Deinitialize the game.
	if (!Deinitialize())
		return false;

	// We need to resume all the threads so we can kill them (lolwut I know).	
	_renderThread->Resume();
	_taskManager->ResumeWorkers();

	// Kill all task managers.
	_taskManager->DestoryWorkers();

	// Destroy render thread.
	while (_renderThread->IsRunning())
		Engine::Platform::Wait(1);
	Engine::Memory::GetDefaultAllocator()->FreeObj(&_renderThread);

	// Destroy our task manager!
	Engine::Memory::GetDefaultAllocator()->FreeObj(&_taskManager);		
	
	// Dispose of the script manager.
	Engine::Memory::GetDefaultAllocator()->FreeObj(&_scriptManager);

	// Destroy our file system!
	Engine::Memory::GetDefaultAllocator()->FreeObj(&_fileSystem);		

	return true;
}

s32 CGameEngine::Run()
{
	// Print general information about the platform we are currently on.
	BasePrintHeader();
	PrintHeader();

	// Initialize the game engine!
	if (!BaseInitialize())
	{
		LOG_ERROR("Failed to initialize game engine, aborting ...");
		return -1;
	}

	// Start running the game engine.
	MainLoop();

	// Deinitialize the game engine.
	if (!BaseDeinitialize())
	{
		LOG_ERROR("Failed to deinitialize game engine, failing silently.");
		return -1;
	}

	return _exitCode;
}

void CGameEngine::Exit(s32 code)
{
	_exited = true;
	_exitCode = code;
}