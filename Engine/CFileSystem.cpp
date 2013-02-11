///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CFileSystem.h"
#include "Platform.h"
#include "CFileStream.h"
#include "CFilePackageStream.h"

using namespace Engine::FileSystem;

CFileSystem::~CFileSystem()
{
	ClearPackages();

	_directoryCache.Clear();
	_attributes.Clear();
	_packages.Clear();

	_diskFilesAllowed = true;
}

bool CFileSystem::SetAssetFolder(const Engine::Containers::CString& path)
{
	// Removed: Dosen't have to exist, as we may be using pak files.
	//if (!Engine::Platform::PathExists(path))
	//	return false;

	_assetFolder = path.TrimEnd("/");

	return true;
}

bool CFileSystem::SetSaveFolder(const Engine::Containers::CString& path)
{
	if (!Engine::Platform::PathExists(path))
		return false;

	_saveFolder = path.TrimEnd("/");

	return true;
}

void CFileSystem::SetDiskFilesAllowed(bool allowDiskFiles)
{
	_diskFilesAllowed = allowDiskFiles;
}

void CFileSystem::SetAttributes(const Engine::Containers::CArray<Engine::Containers::CString>& attributes)
{
	_attributes = attributes;
	for (u32 i = 0; i < _attributes.Size(); i++)
		_attributes[i] = _attributes[i].ToLower();
}

bool CFileSystem::AddPackage(const Engine::Containers::CString& url)
{
	Engine::FileSystem::Containers::CFilePackage* package = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::FileSystem::Containers::CFilePackage>();
	if (!package->Open(url))
	{
		Engine::Memory::GetDefaultAllocator()->FreeObj(&package);
		return false;
	}
	return true;
}

void CFileSystem::ClearPackages()
{
	for (u32 i = 0; i < _packages.Size(); i++)
	{
		_packages[i]->Close();
		Engine::Memory::GetDefaultAllocator()->FreeObj(&_packages[i]);
	}
	_packages.Clear();
}

CFileSystemCachedDirectoryList CFileSystem::GetDirectoryListing(const Engine::Containers::CString& url)
{
	Engine::Containers::CArray<Engine::Containers::CString> allFiles;
	Engine::Containers::CArray<Engine::Containers::CString> allFilesPaths;
	Engine::Containers::CArray<Containers::CFilePackage*>	allFilesPackages;

	CFileSystemCachedDirectoryList listing;
	listing.Path = url;

	// Get the list of files in packages.
	for (s32 i = (s32)_packages.Size() - 1; i >= 0; i--)
	{
		Containers::CFilePackage* package = _packages[i];
		Engine::Containers::CArray<Engine::Containers::CString>& files = package->ListFiles(url);
		for (u32 j = 0; j < files.Size(); j++)
		{
			allFilesPaths.AddToEnd(url + "/" + files[j]);
			allFiles.AddToEnd(files[j]);
			allFilesPackages.AddToEnd(package);
		}
	}

	// Get the list of files on disk.
	if (_diskFilesAllowed == true)
	{
		Engine::Containers::CString onDiskFolder = Engine::Platform::PathNormalize(_assetFolder + "/" + url);
		Engine::Platform::DirHandle handle;

		if (Engine::Platform::PathIsDirectory(onDiskFolder))
		{
			Engine::Platform::DirOpen(&handle, onDiskFolder);
			while (true)
			{
				Engine::Containers::CString diskFile = Engine::Platform::DirNext(&handle);
				if (diskFile == "")
					break;

				Engine::Containers::CString diskFileFull = Engine::Platform::PathNormalize(onDiskFolder + "/" + diskFile);

				if (!Engine::Platform::PathIsDirectory(diskFileFull))
				{
					allFilesPaths.AddToEnd(diskFileFull);
					allFiles.AddToEnd(diskFile);
					allFilesPackages.AddToEnd(NULL);
				}
			}
			Engine::Platform::DirClose(&handle);
		}
	}

	// Split files into name/attributes/type.
	for (u32 i = 0; i < allFiles.Size(); i++)
	{
		Engine::Containers::CArray<Engine::Containers::CString> split = allFiles[i].Split('.');

		CFileSystemCachedFile file;
		file.Filename	 = split[0].ToLower();
		file.Type		 = split.Size() > 1 ? split[split.Size() - 1].ToLower() : "";
		file.Package	 = allFilesPackages[i];
		file.RawFilename = allFiles[i];
		file.FullPath    = allFilesPaths[i];
		for (u32 j = 1; j < split.Size() - 1; j++)
		{
			file.Attributes.AddToEnd(split[j].ToLower());
		}
		listing.Files.AddToEnd(file);
	}

	return listing;
}

CFileSystemCachedFile* CFileSystem::ExpandPathAttributes(const Engine::Containers::CString& url)
{
	Engine::Containers::CString root = url.ToLower();
	Engine::Containers::CString filename;
	Engine::Containers::CString filetype;

	// Get only the last directory name (we don't use the platform function for this as we know 
	// the format and the platform version will normalize the string).
    root = root.Replace("\\", "/").Replace("//", "/");
    s32 slash = root.LastIndexOf("/");
    if (slash < 0)
    {
		root = "/" + root;
		slash = 0;
	}

	// Get the lowercase filename (without extensions or any other garbage).
    filename = root.SubString(slash + 1).ToLower();
	s32 period = filename.IndexOf(".");
	if (period >= 0)
	{
		filename = filename.SubString(0, period);
	}

	// Extract type as well.
    filetype = root.SubString(slash + 1).ToLower();
	period = root.LastIndexOf(".");
	if (period >= 0)
	{
		filetype = root.SubString(period + 1);
	}
	else
	{
		filetype = "";
	}

    root = root.SubString(0, slash);

	u32 rootHashCode = root.ToHashCode();

	// Reload the directory listing.
	if (!_directoryCache.Contains(rootHashCode))
	{
		_directoryCache.Insert(rootHashCode, GetDirectoryListing(root));
	}

	// Grab file listing.
	CFileSystemCachedDirectoryList& fileList = _directoryCache[rootHashCode];

	// We look for attributes based on priority, eg if we have these attributes;
	//	 cooked
	//	 xbox360
	// we will look for these files when trying to load test.script.
	//	 test.cooked.xbox360.script
	//	 test.cooked.script
	//	 test.xbox360.script
	//	 test.script

	// Start from beginning of attributes and ignore the one we are least
	// intrested in each time.
	CFileSystemCachedFile* matchedFile = NULL;
	s32 maxMatches = -1;
	s32 leftOverAttributes = 999999;

	for (s32 startIndex = 0; startIndex < (s32)_attributes.Size(); startIndex++)
	{
		for (s32 endIndex = (s32)_attributes.Size(); endIndex >= startIndex; endIndex--)
		{
			s32 attributeSize = (endIndex - startIndex);

			// Look for file with these attributes.
			for (s32 fileIndex = 0; fileIndex < (s32)fileList.Files.Size(); fileIndex++)
			{
				CFileSystemCachedFile* thisFile = &fileList.Files[fileIndex];
				if (thisFile->Filename != filename)
					continue;
				if (thisFile->Type != filetype)
					continue;
				if ((s32)thisFile->Attributes.Size() < attributeSize)
					continue;

				// Does this file contain all the attributes we are looking for.
				s32 matchCount = 0;
				for (s32 i = startIndex; i < endIndex; i++)
				{
					if (thisFile->Attributes.Contains(_attributes[i]))
					{
						matchCount++;
					}
				}

				if (matchCount > maxMatches)
				{	
					maxMatches = matchCount;
					matchedFile = thisFile;
					leftOverAttributes = thisFile->Attributes.Size() - maxMatches;
					if (maxMatches == _attributes.Size() && _attributes.Size() == matchedFile->Attributes.Size())
					{
						goto exitLoop;
					}
				}
				else if (matchCount == maxMatches)
				{
					s32 newLeftOver = thisFile->Attributes.Size() - maxMatches;
					if (newLeftOver < leftOverAttributes)
					{
						leftOverAttributes = newLeftOver;
						matchedFile = thisFile;
					}
				}
			}
		}
	}
exitLoop: // Not really the best, but speedier than a load of nested breaks.

	return matchedFile;
}

Engine::FileSystem::Streams::CStream* CFileSystem::CreateWriteStream(const Engine::Containers::CString& url)
{
	Engine::Containers::CString real_url = Engine::Platform::PathNormalize(_saveFolder + "/" + url.Trim("\\/"));
	Engine::Containers::CString dir_url  = Engine::Platform::PathDirectoryName(real_url);
	
	// Make sure the directory for this file exists before we try and open it!
	if (!Engine::Platform::PathIsDirectory(dir_url))
		Engine::Platform::DirCreate(dir_url, true);

	return  Engine::Memory::GetDefaultAllocator()->NewObj<Engine::FileSystem::Streams::CFileStream>(real_url, Engine::Platform::FILE_ACCESS_MODE_WRITE, Engine::Platform::FILE_OPEN_MODE_CREATE_ALWAYS);
}

Engine::FileSystem::Streams::CStream* CFileSystem::CreateReadStream(const Engine::Containers::CString& url)
{
	CFileSystemCachedFile* file = ExpandPathAttributes(url);
	if (file == NULL)
		return NULL;
	else if (file->Package != NULL)
		return Engine::Memory::GetDefaultAllocator()->NewObj<FileSystem::Containers::CFilePackageStream>(file->Package, file->FullPath);
	else
		return Engine::Memory::GetDefaultAllocator()->NewObj<Engine::FileSystem::Streams::CFileStream>(file->FullPath, Engine::Platform::FILE_ACCESS_MODE_READ, Engine::Platform::FILE_OPEN_MODE_OPEN_EXISTING);
}

void CFileSystem::DestroyStream(Engine::FileSystem::Streams::CStream* stream)
{
	Engine::Memory::GetDefaultAllocator()->FreeObj(&stream);
}

bool CFileSystem::CanAccess(const Engine::Containers::CString& url)
{
	CFileSystemCachedFile* attr = ExpandPathAttributes(url);
	return (attr != NULL);
}

/*			
u32 CFileSystem::HotReloadGroup(const Engine::Containers::CString& group)
{
	return 0;
}

u32 CFileSystem::HotReloadFile(const Engine::Containers::CString& url)
{
	return 0;
}

u32 CFileSystem::HotReloadAll()
{
	HotReloadEvent evt;
	evt.ReloadType = HOT_RELOAD_ALL;
	evt.Source = this;

	_eventManager->PostEvent(evt);

	return 0;
}
*/