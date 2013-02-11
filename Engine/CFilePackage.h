///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CString.h"
#include "CStream.h"

#include "CArray.h"

namespace Engine
{
    namespace FileSystem
    {
		namespace Containers
		{
			class CFilePackageStream;

			#define FILE_PACKAGE_MAX_VOLUMES				255
			#define FILE_PACKAGE_SIGNATURE					0x4B504349	 // ICPK (Icarus Pak)
			#define FILE_PACKAGE_CRC_VALIDATION_CHUNK_SIZE	1024 * 1024 * 1
			#define FILE_PACKAGE_ENCODE_CHUNK_SIZE			1024 * 1024 * 1
			#define FILE_PACKAGE_VERSION					1
			#define FILE_PACKAGE_CHUNK_BASE_SIZE			(4 + 1 + 4 + 4 + 1 + 4 + 4 + 4 + 4)
			#define FILE_PACKAGE_HEADER_SIZE				(4 + 4 + 4 + 4 + 1)
			#define FILE_PACKAGE_DEFAULT_MAX_SIZE			1024 * 1024 * 64

			// Header information about a file package.
			struct CFilePackageHeader
			{
				u32 Signature;
				u32 Version;
				u32 DictionaryCRC;
				u32 DictionaryLength;
				u8  VolumeCount;
			};

			// Stores information on a data chunk inside a file package.
			// This is basically the actual 'file' in the file package.
			struct CFilePackageChunk
			{
				u32 CRC;

				u8  Volume;
				u32 Offset;
				u32 Length;

				bool IsFile;

				Engine::Containers::CString Name;
			
				u32 NextFileIndex;
				u32 NextDirectoryIndex;

				u32 ParentIndex;
			};

			// Stores information on an individual volume in the file package.
			// Packages can span multiple volumes with a finite size.
			struct CFilePackageVolume
			{
				Engine::FileSystem::Streams::CStream* Stream;
				Engine::Platform::MutexHandle ReadMutex;

				Engine::Containers::CString Path;
			};

			// Typical pak-file class. Allows multiple files to be compressed
			// ane encrypted together into one large packaged file.
			class CFilePackage 
			{
				friend class CFilePackageStream;

				private:
					Engine::Containers::CString						_path;
					bool											_writing;
					u32												_maxVolumeSize;
					bool											_opened;

					CFilePackageHeader								_header;
					CFilePackageVolume								_volumes[FILE_PACKAGE_MAX_VOLUMES];
					Engine::Containers::CArray<CFilePackageChunk>	_dictionary;

					// Loads or saves the directory header.
					void Initialize			();
					bool LoadHeader			();
					bool SaveHeader			();
					bool ValidateCRC		(Engine::FileSystem::Streams::CStream* stream, u64 offset, u64 length, u32 crc);
					s32  GetChunkIndex		(const Engine::Containers::CString& path);

					void ReadBytes			(u8* buffer, u32 volume, u64 offset, u64 length);

				public:

					CFilePackage		();
					~CFilePackage		();
					
					// Opens and closes the stream to this packages on-disk file.
					bool Open			(const Engine::Containers::CString& path, bool write=false, u32 maxVolumeSize=FILE_PACKAGE_DEFAULT_MAX_SIZE);
					void Close			();

					// Modification options.
					bool Exists				(const Engine::Containers::CString& path);
					void Clear				();
					void AddFile			(const Engine::Containers::CString& path);
					void AddDirectory		(const Engine::Containers::CString& path, bool recursive=true);

					// General pokey around settings :3.
					Engine::Containers::CArray<Engine::Containers::CString>	ListFiles(const Engine::Containers::CString& path);
					Engine::Containers::CArray<Engine::Containers::CString>	ListDirectories(const Engine::Containers::CString& path);

			};

		}
	}
}