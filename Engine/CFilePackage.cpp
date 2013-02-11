///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CFilePackage.h"
#include "CFileStream.h"
#include "CMemoryStream.h"

#include "CCRC32Encoder.h"

#include "CArray.h"
#include "CString.h"

using namespace Engine::FileSystem::Containers;

CFilePackage::CFilePackage()
{
	_path			= "";
	_writing		= false;
	_maxVolumeSize	= 0;
	_opened			= false;

	for (u32  i = 0; i < FILE_PACKAGE_MAX_VOLUMES; i++)
	{
		_volumes[i].Path = "";
		_volumes[i].Stream = NULL;
	}

	_dictionary.Clear();
}

CFilePackage::~CFilePackage()
{
	if (_opened == true)
	{
		Close();
	}
}

bool CFilePackage::Open(const Engine::Containers::CString& path, bool write, u32 maxVolumeSize)
{
	LOG_ASSERT(_opened == false);

	_path		   = path; 
	_writing	   = write;
	_maxVolumeSize = maxVolumeSize;
	_opened		   = true;

	if (write == true)
	{
		_volumes[0].Path = path;
		_volumes[0].Stream = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::FileSystem::Streams::CFileStream>(path, Engine::Platform::FILE_ACCESS_MODE_WRITE, Engine::Platform::FILE_OPEN_MODE_CREATE_ALWAYS);		
		if (_volumes[0].Stream->Open())
		{
			Initialize();
			return true;
		}
		else
		{
			_volumes[0].Path = "";
			Engine::Memory::GetDefaultAllocator()->FreeObj(&_volumes[0].Stream);
			return false;
		}
	}
	else
	{
		_volumes[0].Path = path;
		_volumes[0].Stream = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::FileSystem::Streams::CFileStream>(path, Engine::Platform::FILE_ACCESS_MODE_READ, Engine::Platform::FILE_OPEN_MODE_OPEN_ALWAYS);		
		Engine::Platform::MutexCreate(&_volumes[0].ReadMutex);

		if (_volumes[0].Stream->Open())
		{
			return LoadHeader();
		}
		else
		{
			_volumes[0].Path = "";
			Engine::Memory::GetDefaultAllocator()->FreeObj(&_volumes[0].Stream);
			return false;
		}
	}
}

void CFilePackage::Close()
{
	LOG_ASSERT(_opened == true);

	// If we are writing, then save everything.
	if (_writing == true)
	{
		SaveHeader();
	}

	// Dispose of everything.
	_opened = false;
	for (u32  i = 0; i < FILE_PACKAGE_MAX_VOLUMES; i++)
	{
		if (_volumes[i].Stream != NULL)
		{
			_volumes[i].Stream->Close();
			Engine::Memory::GetDefaultAllocator()->FreeObj(&_volumes[i].Stream);

			if (_writing = false)
			{
				Engine::Platform::MutexDelete(&_volumes[i].ReadMutex);
			}
		}

		_volumes[i].Path = "";
		_volumes[i].Stream = NULL;
	}

	// Empty the dictionary.
	_dictionary.Clear();
}
					
void CFilePackage::ReadBytes(u8* buffer, u32 volume, u64 offset, u64 length)
{
	LOG_ASSERT(_opened == true && _writing == false);
	LOG_ASSERT((u32)volume < (u32)_header.VolumeCount);
	LOG_ASSERT(_volumes[volume].Stream != NULL);

	Engine::Platform::MutexLock(&_volumes[volume].ReadMutex);

	if (_volumes[volume].Stream->Position() != offset)
		_volumes[volume].Stream->Seek(offset);

	_volumes[volume].Stream->ReadBytes(buffer, length);
	
	Engine::Platform::MutexUnlock(&_volumes[volume].ReadMutex);
}

bool CFilePackage::SaveHeader()
{
	LOG_ASSERT(_opened == true && _writing == true);
	
	Engine::Data::Hashing::CCRC32Encoder encoder;

	// Build directory.
	u32 directoryLength = (_dictionary.Size() * FILE_PACKAGE_CHUNK_BASE_SIZE);
	u32 volumeCount     = 1;

	// Work out directory size.
	for (u32 fileIndex = 0; fileIndex < _dictionary.Size(); fileIndex++)
		directoryLength += _dictionary[fileIndex].Name.Length();
	u32 volume0HeaderSize = directoryLength + FILE_PACKAGE_HEADER_SIZE;

	// Start calculating file offsets.
	u32 volumeOffset = volume0HeaderSize;
	for (u32 fileIndex = 0; fileIndex < _dictionary.Size(); fileIndex++)
	{
		Engine::Containers::CString path = "";
		u32 index = fileIndex;
		while (index != 0)
		{
			path = _dictionary[index].Name + (path != "" ? "/" + path : "");
			index = _dictionary[index].ParentIndex;
		}

		// Calculate directory information (aka none).
		if (_dictionary[fileIndex].IsFile == false)
		{
			_dictionary[fileIndex].Offset = 0;
			_dictionary[fileIndex].Length = 0;
			_dictionary[fileIndex].Volume = 0;
			_dictionary[fileIndex].CRC = 0;
		}

		// Calculate file information?
		else
		{
			// Calculate the CRC.
			Engine::FileSystem::Streams::CFileStream file(path, Engine::Platform::FILE_ACCESS_MODE_READ, Engine::Platform::FILE_OPEN_MODE_OPEN_EXISTING);
			if (!file.Open())
			{		
				LOG_ERROR("Failed to open file to serialize (%s).", path.c_str());
				Close();
				return false;
			}
			_dictionary[fileIndex].Length = (u32)file.Length();
			encoder.Initialize();
			encoder.AddStream(&file);
			file.Close();

			_dictionary[fileIndex].CRC = *(const u32*)encoder.Calculate().GetBuffer();

			// Split to next volume.
			if (volumeOffset + _dictionary[fileIndex].Length > _maxVolumeSize)
			{
				volumeOffset = 0;
				volumeCount += 1;
			}
			
			_dictionary[fileIndex].Offset = volumeOffset;
			_dictionary[fileIndex].Volume = volumeCount - 1;
			
			volumeOffset += _dictionary[fileIndex].Length;
		}
	}

	// Write everything out!
	Engine::FileSystem::Streams::CMemoryStream dictionaryStream(1024);
	dictionaryStream.Open();
	for (u32 fileIndex = 0; fileIndex < _dictionary.Size(); fileIndex++)
	{	
		CFilePackageChunk chunk = _dictionary[fileIndex];
		dictionaryStream.WriteBytes((u8*)&chunk.CRC, 4);
		dictionaryStream.WriteString(chunk.Name);
		dictionaryStream.WriteU32(chunk.NextFileIndex);
		dictionaryStream.WriteU32(chunk.NextDirectoryIndex);
		dictionaryStream.WriteU32(chunk.ParentIndex);
		dictionaryStream.WriteU8(chunk.Volume);
		dictionaryStream.WriteU32(chunk.Offset);
		dictionaryStream.WriteU32(chunk.Length);
		dictionaryStream.WriteU8(chunk.IsFile);
	}

	// CRC the dictionary.
	encoder.Initialize();
	encoder.AddBuffer(dictionaryStream.GetBuffer(), (u32)dictionaryStream.Length());
	u32 dictionaryCRC = *((const u32*)encoder.Calculate().GetBuffer());
	
	// Save header information.
	_volumes[0].Stream->WriteU32(FILE_PACKAGE_SIGNATURE);
	_volumes[0].Stream->WriteU32(FILE_PACKAGE_VERSION);
	_volumes[0].Stream->WriteBytes((u8*)&dictionaryCRC, 4);
	_volumes[0].Stream->WriteU32(directoryLength);
	LOG_ASSERT(directoryLength == dictionaryStream.Length());
	_volumes[0].Stream->WriteU8(volumeCount);
	
	// Write in the directory.
	_volumes[0].Stream->WriteBytes(dictionaryStream.GetBuffer(), dictionaryStream.Length());
	
	dictionaryStream.Close();

	// Allocate a buffer to do validation :3.
	u8* buffer = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(FILE_PACKAGE_ENCODE_CHUNK_SIZE);

	// Write in the data into each volume.
	for (u32 vol = 0; vol < volumeCount; vol++)
	{
		if (vol > 0)
		{
			_volumes[vol].Path   = _volumes[0].Path + ".v" + (vol + 1);
			_volumes[vol].Stream = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::FileSystem::Streams::CFileStream>(_volumes[vol].Path, Engine::Platform::FILE_ACCESS_MODE_WRITE, Engine::Platform::FILE_OPEN_MODE_CREATE_ALWAYS);	
			if (_volumes[vol].Stream->Open() != true)		
			{
				LOG_ERROR("Failed to load volume span of package file (%s).", (_volumes[vol].Path).c_str());		
				Close();
				return false;
			}
		}

		for (u32 fileIndex = 0; fileIndex < _dictionary.Size(); fileIndex++)
		{
			CFilePackageChunk chunk = _dictionary[fileIndex];
			if (chunk.Volume == vol && chunk.IsFile == true)
			{
				Engine::Containers::CString path = "";
				u32 index = fileIndex;
				while (index != 0)
				{
					path  = _dictionary[index].Name + (path != "" ? "/" + path : "");
					index = _dictionary[index].ParentIndex;
				}

				Engine::FileSystem::Streams::CFileStream file(path, Engine::Platform::FILE_ACCESS_MODE_READ, Engine::Platform::FILE_OPEN_MODE_OPEN_EXISTING);
				if (!file.Open())
				{		
					LOG_ERROR("Failed to open file to serialize (%s).", path.c_str());
					Close();
					return false;
				}
				
				// Write the file in chunk by chunk.
				u64 remaining = file.Length();
				while (remaining > 0)
				{
					u64 chunkSize = min(remaining, FILE_PACKAGE_ENCODE_CHUNK_SIZE);
		
					file.ReadBytes(buffer, chunkSize);
					_volumes[vol].Stream->WriteBytes(buffer, chunkSize);

					remaining -= chunkSize;
				}

				file.Close();
			}
		}
	}

	// Free the buffer.
	Engine::Memory::GetDefaultAllocator()->Free(&buffer);

	return true;
}

bool CFilePackage::LoadHeader()
{
	LOG_ASSERT(_opened == true && _writing == false);

	// Load in the header information.
	_header.Signature = _volumes[0].Stream->ReadU32();
	if (_header.Signature != FILE_PACKAGE_SIGNATURE)
	{
		LOG_ERROR("Signature of package file was invalid, failed to load.");
		return false;
	}

	_header.Version	= _volumes[0].Stream->ReadU32();
	if (_header.Version > FILE_PACKAGE_VERSION)
	{
		LOG_ERROR("Failed to load package file, file format version is higher than what is supported.");
		return false;
	}

	_volumes[0].Stream->ReadBytes((u8*)&_header.DictionaryCRC, 4);
	
	_header.DictionaryLength = _volumes[0].Stream->ReadU32();
	_header.VolumeCount      = _volumes[0].Stream->ReadU8();

	// Open all the volumes.
	for (u32 i = 1; i < (u32)_header.VolumeCount; i++)
	{
		_volumes[i].Path   = _volumes[0].Path + ".v" + (i + 1);
		_volumes[i].Stream = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::FileSystem::Streams::CFileStream>(_volumes[i].Path, Engine::Platform::FILE_ACCESS_MODE_READ, Engine::Platform::FILE_OPEN_MODE_OPEN_ALWAYS);	
		Engine::Platform::MutexCreate(&_volumes[i].ReadMutex);
		if (_volumes[i].Stream->Open() != true)		
		{
			LOG_ERROR("Failed to load volume span of package file (%s).", (_volumes[0].Path + i).c_str());		
			Close();
			return false;
		}
	}

	// Check CRC is correct.
	u64 dictionaryPos = _volumes[0].Stream->Position();
	if (!ValidateCRC(_volumes[0].Stream, dictionaryPos, _header.DictionaryLength, _header.DictionaryCRC))
	{
		LOG_ERROR("Failed to load dictionary of package file, CRC check failed.");
		Close();
		return false;
	}

	// Read in each chunk.
	_volumes[0].Stream->Seek(dictionaryPos);
	u32 endOfDictionary = (u32)_volumes[0].Stream->Position() + _header.DictionaryLength;
	for (u32 i = 0; _volumes[0].Stream->Position() < endOfDictionary; i++)
	{
		CFilePackageChunk chunk;
		_volumes[0].Stream->ReadBytes((u8*)&chunk.CRC, 4);
		chunk.Name					= _volumes[0].Stream->ReadString();
		chunk.NextFileIndex			= _volumes[0].Stream->ReadU32();
		chunk.NextDirectoryIndex	= _volumes[0].Stream->ReadU32();
		chunk.ParentIndex			= _volumes[0].Stream->ReadU32();
		chunk.Volume				= _volumes[0].Stream->ReadU8();
		chunk.Offset				= _volumes[0].Stream->ReadU32();
		chunk.Length				= _volumes[0].Stream->ReadU32();
		chunk.IsFile				= _volumes[0].Stream->ReadU8() != 0;
		_dictionary.AddToEnd(chunk);

		u32 position = (u32)_volumes[chunk.Volume].Stream->Position();
		if (chunk.IsFile == true && !ValidateCRC(_volumes[chunk.Volume].Stream, chunk.Offset, chunk.Length, chunk.CRC))
		{
			LOG_ERROR("Failed to load dictionary of package file, CRC on data chunk failed. Index:%i, Name:%s", i, chunk.Name.c_str());
			Close();
			return false;
		}
		_volumes[chunk.Volume].Stream->Seek(position);
	}

	return true;
}

bool CFilePackage::ValidateCRC(Engine::FileSystem::Streams::CStream* stream, u64 offset, u64 length, u32 crc)
{
	Engine::Data::Hashing::CCRC32Encoder encoder;
	encoder.Initialize();

	// Allocate a buffer to do validation :3.
	u8* buffer = (u8*)Engine::Memory::GetDefaultAllocator()->Alloc(FILE_PACKAGE_CRC_VALIDATION_CHUNK_SIZE);

	// Seek to start of chunk.
	stream->Seek(offset);

	// Read through and validate the file in chunks.
	u64 remaining = length;
	while (remaining > 0)
	{
		u64 chunkSize = min(remaining, FILE_PACKAGE_CRC_VALIDATION_CHUNK_SIZE);
		
		stream->ReadBytes(buffer, chunkSize);
		encoder.AddBuffer(buffer, (u32)chunkSize);

		remaining -= chunkSize;
	}

	// Free the buffer.
	Engine::Memory::GetDefaultAllocator()->Free(&buffer);

	// Check the CRC.
	u32 dictionaryCRC = *((const u32*)encoder.Calculate().GetBuffer());
	return (crc == dictionaryCRC);	
}

s32 CFilePackage::GetChunkIndex(const Engine::Containers::CString& path)
{
	LOG_ASSERT(_opened == true);

	// Search paths are in this format; /root/x/y.script

	// Normalize the path.
	Engine::Containers::CString normalPath = path;
	normalPath = normalPath.Replace('\\', '/');
	if (normalPath[0] != '/')
		normalPath = S("/") + normalPath;

	// Root path?
	if (normalPath == "/")
		return 0;

	// Split the path into segments.
	Engine::Containers::CArray<Engine::Containers::CString> segments = normalPath.Split('/', 0, true);
	segments.RemoveIndex(0); // Remove the first blank index (the root).

	// Start at the root directory.
	u32				  chunkIndex = 0;
	CFilePackageChunk chunk		 = _dictionary[0];
	for (u32 i = 0; i < segments.Size(); i++)
	{
		Engine::Containers::CString seg = segments[i].ToLower();
		bool found = false;

		// Look through directorys for a match.
		u32 index = chunk.NextDirectoryIndex;
		while (index != 0)
		{
			CFilePackageChunk c = _dictionary[index];
			if (c.Name.ToLower() == seg)
			{
				found = true;
				chunk = c;
				chunkIndex = index;
				break;
			}
			index = c.NextDirectoryIndex; 
		}

		// Look through files for a match.
		if (found == false)
		{
			index = chunk.NextFileIndex;
			while (index != 0)
			{
				CFilePackageChunk c = _dictionary[index];
				if (c.Name.ToLower() == seg)
				{
					found = true;
					chunk = c;
					chunkIndex = index;
					break;
				}
				index = c.NextFileIndex; 
			}
		}

		// Faaaail :(
		if (found == false)
		{
			return -1;
		}
	}

	return chunkIndex;
}

bool CFilePackage::Exists(const Engine::Containers::CString& path)
{
	LOG_ASSERT(_opened == true);
	return GetChunkIndex(path) != -1;
}

void CFilePackage::Clear()
{
	LOG_ASSERT(_opened == true && _writing == true);
	_dictionary.Clear();
}

void CFilePackage::AddFile(const Engine::Containers::CString& path)
{
	LOG_ASSERT(_opened == true && _writing == true);

	Engine::Containers::CString normalPath = path;
	normalPath = normalPath.Replace('\\', '/');

	// Does file already exists?
	if (Exists(normalPath))
		return;

	// Crack the path, and create each directory.
	Engine::Containers::CArray<Engine::Containers::CString> segments = normalPath.Split('/', 0, true);
	Engine::Containers::CString cracked = "/";
	s32 chunkIndex = 0; 
	for (u32 i = 0; i < segments.Size(); i++)
	{
		cracked += (i > 0 ? "/" : "") + segments[i];

		// Does it already exist?
		s32 index = GetChunkIndex(cracked);
		if (index == -1)
		{
			// Add the new chunk.
			CFilePackageChunk chunk;
			chunk.Name			     = segments[i];
			chunk.NextFileIndex		 = 0;
			chunk.NextDirectoryIndex = 0;
			chunk.ParentIndex		 = chunkIndex;

			// Directory?
			if (i != segments.Size() - 1)
			{
				chunk.IsFile = false;
				chunk.NextDirectoryIndex = _dictionary[chunkIndex].NextDirectoryIndex;
			}

			// File?
			else
			{
				chunk.IsFile = true;
				chunk.NextFileIndex = _dictionary[chunkIndex].NextFileIndex;
			}

			_dictionary.AddToEnd(chunk);

			u32 newIndex = _dictionary.Size() - 1;
			_dictionary[chunkIndex].NextDirectoryIndex = newIndex;

			chunkIndex = newIndex;
		}
		else
		{
			chunkIndex = index;
		}
	}
}

void CFilePackage::AddDirectory(const Engine::Containers::CString& path, bool recursive)
{
	LOG_ASSERT(_opened == true && _writing == true);

	Engine::Platform::DirHandle handle;
	if (!Engine::Platform::DirOpen(&handle, path))
		return;

	while (true)
	{
		Engine::Containers::CString sub = Engine::Platform::DirNext(&handle);
		if (sub == "") 
			break;

		// Ignore any SVN entries.
		if (sub == ".svn")
			continue;

		sub = path + "/" + sub;

		if (Engine::Platform::PathIsDirectory(sub))
		{
			if (recursive == true)
				AddDirectory(sub, recursive);
		}
		else
		{
			AddFile(sub);
		}
	}

	Engine::Platform::DirClose(&handle);
}

void CFilePackage::Initialize()
{	
	// Create the root chunk.
	CFilePackageChunk chunk;
	chunk.Name			     = "";
	chunk.NextFileIndex		 = 0;
	chunk.NextDirectoryIndex = 0;
	chunk.IsFile			 = false;
	chunk.ParentIndex		 = 0;
	_dictionary.AddToEnd(chunk);
}

Engine::Containers::CArray<Engine::Containers::CString>	CFilePackage::ListFiles(const Engine::Containers::CString& path)
{
	LOG_ASSERT(_opened == true);

	Engine::Containers::CString normalPath = path;
	normalPath = normalPath.Replace('\\', '/');

	Engine::Containers::CArray<Engine::Containers::CString>	files;
	s32 chunkIndex = GetChunkIndex(normalPath);
	if (chunkIndex >= 0)
	{
		CFilePackageChunk& chunk = _dictionary[chunkIndex];

		u32 index = chunk.NextFileIndex;
		while (index != 0)
		{
			CFilePackageChunk& c = _dictionary[index];
			files.AddToEnd(c.Name);
			index = c.NextFileIndex; 
		}
	}

	return files;
}

Engine::Containers::CArray<Engine::Containers::CString>	CFilePackage::ListDirectories(const Engine::Containers::CString& path)
{
	LOG_ASSERT(_opened == true);

	Engine::Containers::CString normalPath = path;
	normalPath = normalPath.Replace('\\', '/');

	Engine::Containers::CArray<Engine::Containers::CString>	files;
	s32 chunkIndex = GetChunkIndex(normalPath);
	if (chunkIndex >= 0)
	{
		CFilePackageChunk& chunk = _dictionary[chunkIndex];

		u32 index = chunk.NextDirectoryIndex;
		while (index != 0)
		{
			CFilePackageChunk& c = _dictionary[index];
			files.AddToEnd(c.Name);
			index = c.NextDirectoryIndex; 
		}
	}

	return files;
}
