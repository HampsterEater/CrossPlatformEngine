///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"
#include "CThread.h"

#include "CFileSystem.h"
#include "CScriptManager.h"

namespace Engine
{
	namespace FileSystem
	{
		class CFileSystem;
	}
	namespace Core
	{
		namespace Tasks
		{
			class CTaskManager;
		}
	
		// This is the base class that all games are inherited from.
		// It deals with all the fun underlying code used to get the game up and running.
		// To derive a game form this, just create your own class inherited from it
		// and override the virtual methods.
		class CGameEngine
		{
			protected:

				// Game titel information.
				const u8*									_gameTitle;
				const u8*									_gameTitleShort;

			private:

				// State information.
				bool										_exited;
				s32											_exitCode;
				
				// Task management.
				Engine::Threading::CThread*					_renderThread;
				Tasks::CTaskManager*						_taskManager;

				// File-system management.
				Engine::FileSystem::CFileSystem*			_fileSystem;

				// Scripting.
				Engine::Scripting::CScriptManager*			_scriptManager;

				// Private initialization methods.
				bool SetupFileSystem					();

				void BasePrintHeader					();
				void BaseRender							();
				void BaseUpdate							();
				bool BaseInitialize						();
				bool BaseDeinitialize					();
				void MainLoop							();
				static s32 RenderEntryPoint				(void* meta);

			protected:
				
				virtual void PrintHeader				() = 0;
				virtual void Update						() = 0;
				virtual void Render						() = 0;
				virtual bool Initialize					() = 0;
				virtual bool Deinitialize				() = 0;

			public:

				CGameEngine														(const u8* game_title="Unnamed Game", const u8* game_title_short="UG");
				~CGameEngine													();

				Tasks::CTaskManager*						GetTaskManager		();
				Engine::FileSystem::CFileSystem*			GetFileSystem		();
				//CConsole*									GetConsole			();
				//CArgumentParser*							GetArguments		();
				Engine::Scripting::CScriptManager*			GetScriptManager	();

				s32											Run					();
				void										Exit				(s32 code);

		};

	}
}