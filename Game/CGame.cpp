///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "CGame.h"

#include "..\Engine\CArray.h"

#include "..\Engine\CTaskManager.h"
#include "..\Engine\CDebugPrintTaskJob.h"

#include "..\Engine\CXMLFile.h"

#include "..\Engine\CFileSystem.h"
#include "..\Engine\CFileStream.h"

#include "..\Engine\CScriptCompileContext.h"
#include "..\Engine\CScriptLexer.h"
#include "..\Engine\CScriptParser.h"
#include "..\Engine\CScriptGenerator.h"
#include "..\Engine\CScriptVirtualMachine.h"

#include "..\Engine\CCRC32Encoder.h"

#include "Version.h"

using namespace Game::Core;
using namespace Engine::Core::Tasks;
using namespace Engine::Core::Tasks::Jobs;
using namespace Engine::Scripting;

CGame::CGame(const u8* game_title, const u8* game_title_short) : CGameEngine(game_title, game_title_short)
{
	// Nothing to do here really, we are just calling the base constructor.
}

void CGame::PrintHeader()
{
	LOG_INFO(S(_gameTitle));
#if DEBUG
	LOG_INFO(S("Version %s DEBUG (Built %s/%s/%s %s:%s)").Format(AutoVersion::FULLVERSION_STRING, AutoVersion::DAY, AutoVersion::MONTH, AutoVersion::YEAR, AutoVersion::HOUR, AutoVersion::MINUTE));
#else
	LOG_INFO(S("Version %s RELEASE (Built %s/%s/%s %s:%s)").Format(AutoVersion::FULLVERSION_STRING, AutoVersion::DAY, AutoVersion::MONTH, AutoVersion::YEAR, AutoVersion::HOUR, AutoVersion::MINUTE));
#endif
	LOG_INFO("----------------------------------------------------");
}

void CGame::Update()
{
	CScriptCompileContext* context = GetScriptManager()->CompileFile("/test.script");
	if (context != NULL)
	{
		if (context->GetErrorCount() == 0)
		{
			CScriptExecutionContext* execution_context = GetScriptManager()->Load(context);
			GetScriptManager()->Run();
		}
		else
		{
			for (u32 i = 0; i < context->GetErrorCount(); i++)
			{
				Engine::Containers::CString error = context->FormatError(context->GetError(i));
				printf("\n%s\n", error.c_str());
			}
		}
	}
	

	//Engine::FileSystem::Streams::CFileStream file("Assets/test.script");
	//if (file.Open())
	//{
	//	GetScriptManager()->Compile(file->);
	//
	//}


	/*
	Engine::FileSystem::Streams::CStream* s = GetFileSystem()->CreateReadStream("lib/io.script");
	if (s->Open())
	{
		s->Close();
	}

	s = GetFileSystem()->CreateWriteStream("save/save0.dat");
	if (s->Open())
	{
		s->Close();
	}


	Engine::FileSystem::Streams::CFileStream file("Assets/test.script");
	if (file.Open())
	{
		// Load script into context.
		Engine::Containers::CString source = file.ReadToEnd();
		Engine::Scripting::CScriptCompileContext context(source);
		file.Close();

		f32 timer = (f32)Engine::Platform::GetMillisecs();

		// Lex that shit.
		Engine::Scripting::CScriptLexer lexer;
		if (lexer.Analyze(&context))
		{
			Engine::Scripting::CScriptParser parser;
			if (parser.Analyze(&context))
			{
				Engine::Scripting::CScriptGenerator generator;
				if (generator.Analyze(&context))
				{
					Engine::FileSystem::Streams::CFileStream output("Assets/test.cooked.script");
					if (output.Open())
					{
						context.Save(&output);
						output.Close();
					}

					
					Engine::FileSystem::Streams::CFileStream input("Assets/test.cooked.script", Engine::Platform::FILE_ACCESS_MODE_READ, Engine::Platform::FILE_OPEN_MODE_OPEN_ALWAYS);
					if (output.Open())
					{
						if (context.Load(&output))
						{
							Engine::Scripting::CScriptExecutionContext executionContext(&context);

							Engine::Scripting::CScriptVirtualMachine machine;
						//	machine.SetModulePath("Assets\\Lib");	// Needs to use pak files ;_;
							machine.LoadNativeLibrary();
							machine.AddContext(&executionContext);
							
							f32 gltimer = (f32)Engine::Platform::GetMillisecs();
							while (true)
							{
								//f64 t = Engine::Platform::GetMillisecs();
								executionContext.CallEvent("OnTick", 0, false, false);

								machine.Run();			
								//printf("TOOK %f\n", Engine::Platform::GetMillisecs() - t);				
							}
							f32 glelapsed = (f32)Engine::Platform::GetMillisecs() - gltimer;

							printf("\nSuccessfully Completed (Global executed in %.2f ms)\n", glelapsed);
						}
						output.Close();
					}
				}
			}
		}
		
		// Print out errors/warnings/info
		if (context.GetErrorCount() > 0)
		{
			for (u32 i = 0; i < context.GetErrorCount(); i++)
			{
				Engine::Containers::CString error = context.FormatError(context.GetError(i));
				printf("\n%s\n", error.c_str());
			}
		}

		f32 elapsed = (f32)Engine::Platform::GetMillisecs() - timer;
		printf("Elapsed Time: %f\n", elapsed);
	}
 	Exit(0);
	*/

	Exit(0);
}

void CGame::Render()
{

}

bool CGame::Initialize()
{
	/*
	Engine::Containers::CString rootFolder       = Engine::Platform::PathGetWorkingDir() + "/" GAME_ASSET_ROOT_FOLDER "/";
	Engine::Containers::CString saveFolder       = Engine::Platform::PathGetSpecialDir(Engine::Platform::SPECIAL_DIR_APPDATA) + "/" + GAME_NAME_SHORT + "/" + GAME_ASSET_SAVE_FOLDER + "/";
	Engine::Containers::CString dataFile         = Engine::Platform::PathGetWorkingDir() + "/" GAME_ASSET_DATA_FILE;
	Engine::Containers::CString configFile       = Engine::Platform::PathGetWorkingDir() + "/" GAME_CONFIG_FILE;
	Engine::Containers::CString profileFile      = Engine::Platform::PathGetSpecialDir(Engine::Platform::SPECIAL_DIR_APPDATA) + "/" + GAME_NAME_SHORT + "/" + GAME_PROFILE_FILE;

	// Root the file system to our working folder + /Assets/.
	// When loading files you can skip the root folder path and just use /Images/etc/whatever
	// This should be the only folder read from.
	LOG_INFO("Setting file system root to: %s", rootFolder.c_str());
	if (!GetFileSystem()->SetAssetFolder(rootFolder))
	{
		LOG_ERROR("Failed to root to expected '%s' assets folder, aborting.", rootFolder.c_str());
		return false;
	}

	// Change the save directory to the users application data folder.
	// This should be the only folder ever written to.
	LOG_INFO("Setting save root to: %s", saveFolder.c_str());
	if (!GetFileSystem()->SetSaveFolder(saveFolder))
	{
		LOG_ERROR("Failed to root to expected '%s' save folder, aborting.", saveFolder.c_str());
		return false;
	}

	// If there is a data file available, then lets crack that bitch open :3.
	if (Engine::Platform::PathIsFile(dataFile))
	{
		if (!GetFileSystem()->AddPackage(dataFile))
		{
			LOG_ERROR("Failed to load asset data file from '%s', aborting.", dataFile.c_str());
			return false;
		}
	}
	else
	{
#ifdef GAME_ASSET_FORCE_DATA_FILE
		LOG_ERROR("Failed to load asset data file from '%s', file is missing, aborting.", dataFile.c_str());
		return false;
#endif
	}
	
	// Load the configuration file.
	LOG_INFO("Loading configuration file: %s", configFile.c_str());
	if (Engine::Platform::PathIsFile(configFile))
	{

	}
	else
	{
		LOG_ERROR("Failed to load configuration file from '%s', file is missing, aborting.", configFile.c_str());
		return false;
	}

	// Load profile configuration file if it exists.
	LOG_INFO("Loading profile file: %s", configFile.c_str());
	if (Engine::Platform::PathIsFile(configFile))
	{

	}

	// Load command line arguments, override settings in 
	// configuration file.
	// TODO

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
	attributes.AddToEnd("english");
	attributes.AddToEnd("cooked");
#ifdef DEBUG
	attributes.AddToEnd("debug");
#else
	attributes.AddToEnd("release");
#endif
	GetFileSystem()->SetAttributes(attributes);
	"name.ps3.english.cooked.debg.pak"
	*/
	return true;
}

bool CGame::Deinitialize()
{
	return true;
}