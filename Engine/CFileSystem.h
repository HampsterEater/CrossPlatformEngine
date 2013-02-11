///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"
#include "CArray.h"
#include "CString.h"
#include "CHashTable.h"
#include "CFilePackage.h"

namespace Engine
{
    namespace FileSystem
    {

		// Files are stored primarily within asset file packages, though can also be loaded off the disk in debug mode.
		// Paths to files are always relative to the asset folder (unless you are writing in which case they are relative to the save folder),
		// paths are case-sensitive and are seperated with forward slashes. The format of file names are as specified;
		//
		//	<filename>.<attributes>.<type>
		//
		//	eg.
		//
		//	testfile.debug.xbox360.script
		//
		//	The filename differentiates between files as usual, and the type (where the extension would normally be) determines what the file
		//  contains (these are full names, not shortened extensions, eg use .script not .scr or similar, use .texture not .png, etc). 
		//
		//  The attributes are where the fun comes in, each build of the game specifies the attributes it prefers for a file (eg, .xbox, .english, etc),
		//	these allow the game to choose different assets depending on the platform/language-selected and so on.
		//
		//	eg. if the following attributes are available
		//		cooked
		//		xbox360
		//
		//	And we look for;
		//		test.script
		//
		//  The file system will look for the following files in order;
		//		test.cooked.xbox360.script
		//		test.cooked.script
		//		test.xbox360.script
		//		test.script
		//
		//	Order of attributes does not matter, it will look for all.

		/*
		// The one thing to remember about the file system is everything should be designed to be hot-reloadable. 
		// eg. if you create a texture you should always register a callback that will reload it if neccessary.
		// This is primarily used to make sure changes that developers make are instantly reflected in the game engine
		// decreasing development time and increasing productivity.
		//
		// Hot reloading is done by deriving your asset class from IHotReloadable. You register for a callback by invoking
		// RegisterHotReload and deregister by calling UnregisterHotReload. When hot reloads are required the HotReload
		// virutal method will be invoked with the parameters specified. If you fail to de-register all hotreloads the 
		// IHotReloadable destructor will do it for you.
		*/

		/*
		// Any class that uses hot-reloadable assets derive from this class. This is primarily used to keep
		// the compiler happy when taking member function pointers to use with hot reloading.
		class IHotReloadable
		{
		public:
			void HookHotReloadURL		(CFileSystem* fileSystem, const Engine::Containers::CString& url);
			void HookHotReloadGroup		(CFileSystem* fileSystem, const Engine::Containers::CString& group);
			void UnhookHotReload		();

			virtual void HotReload		() = 0;

			~IHotReloadable				();
		};

		EVENT_HOT_RELOAD_URL
		EVENT_HOT_RELOAD_GROUP
		EVENT_APP_CLOSE

		CEventManager
		{
			void PostEvent(u32 event_id, evt);
		}

		CEvent
		{
			GetEventID();
			GetSource();
			// .. getdata
		}

		IEventListener
		{
			void		HookEvent		(CEventManager* manager, u32 event_id);
			void		UnhookEvent		(CEventManager* manager, u32 event_id);
			virtual void ProcessEvent	(CEvent& evt);
		}
		*/

		// Stores a cached list of files in a directory.
		class CFileSystemCachedFile
		{
		public:
			Engine::Containers::CString								Filename;
			Engine::Containers::CString								RawFilename;
			Engine::Containers::CString								FullPath;
			Engine::Containers::CString								Type;
			Engine::Containers::CArray<Engine::Containers::CString>	Attributes;
			Engine::FileSystem::Containers::CFilePackage*			Package;
		};

		class CFileSystemCachedDirectoryList
		{
		public:
			Engine::Containers::CString							Path;
			Engine::Containers::CArray<CFileSystemCachedFile>	Files;
		};

		// The file system class. The local file system should only ever be accessed
		// through this, you shouldn't create file streams directly!
		// All files loaded into the system should be hot reloadable!
		class CFileSystem
		{
			private:

				Engine::Containers::CHashTable<CFileSystemCachedDirectoryList>				_directoryCache;
				Engine::Containers::CArray<Engine::FileSystem::Containers::CFilePackage*>	_packages;
				Engine::Containers::CArray<Engine::Containers::CString>						_attributes;
			
				Engine::Containers::CString	_assetFolder;
				Engine::Containers::CString	_saveFolder;

				bool _diskFilesAllowed;

				// Path manipulation junk.
				CFileSystemCachedDirectoryList	GetDirectoryListing	(const Engine::Containers::CString& url);
				CFileSystemCachedFile*			ExpandPathAttributes(const Engine::Containers::CString& url);

			public:

				~CFileSystem			();

				// Sets the folders that reading/writing are done to.
				bool SetAssetFolder		(const Engine::Containers::CString& path);
				bool SetSaveFolder		(const Engine::Containers::CString& path);
				
				// Sets or gets the attributes used to load files.
				void SetAttributes		(const Engine::Containers::CArray<Engine::Containers::CString>& attributes);
				void SetDiskFilesAllowed(bool allowDiskFiles);

				// Modifies the packages used to load streams before the local
				// filesystem is accessed.
				bool AddPackage			(const Engine::Containers::CString& url);
				void ClearPackages		();

				// Opens a stream to a file on the local system, and registers the hot-reload callback
				// to the given resource group.
				Engine::FileSystem::Streams::CStream* CreateWriteStream	(const Engine::Containers::CString& url);
				Engine::FileSystem::Streams::CStream* CreateReadStream	(const Engine::Containers::CString& url);
				void								  DestroyStream		(Engine::FileSystem::Streams::CStream* stream);
				bool								  CanAccess			(const Engine::Containers::CString& url);
				
				// Invokes the hot reload callback for either
				// a group or file registered when opening a stream.
				u32 HotReloadGroup		(const Engine::Containers::CString& group);
				u32 HotReloadFile		(const Engine::Containers::CString& url);
				u32 HotReloadAll		();
		};

	}
}