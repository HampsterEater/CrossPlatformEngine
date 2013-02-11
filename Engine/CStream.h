///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"
#include "CArray.h"

namespace Engine
{
    namespace FileSystem
    {
		namespace Streams
		{
			class CStream
			{
				protected:
					CStream() { } // Disable instantiation. 

				public:
					virtual ~CStream() { } 

					virtual bool Open		() = 0;
					virtual void Close		() = 0;
					virtual void Seek		(u64 position) = 0;
					virtual void Flush		() = 0;
					virtual u64  Position	() = 0;
					virtual u64  Length		() = 0;
					virtual bool AtEnd		() = 0;
					virtual void ReadBytes	(u8* buffer, u64 length) = 0;
					virtual void WriteBytes	(const u8* buffer, u64 length) = 0;

					template <typename T> T		ReadPOD			();
										u64		ReadU64			 ();
										s64		ReadS64			 ();
										u32		ReadU32			 ();
										s32		ReadS32			 ();
										u16		ReadU16			 ();
										s16		ReadS16			 ();
										u8		ReadU8			 ();
										s8		ReadS8			 ();
										f64		ReadF64			 ();
										f32		ReadF32			 ();

					Engine::Containers::CString		ReadLine	 ();
					Engine::Containers::CString		ReadToEnd	 ();
					Engine::Containers::CString		ReadText	 (u32 len);
					Engine::Containers::CString		ReadString	 ();

					template <typename T> void	WritePOD	 (T data);
										  void  WriteU64	 (u64 data);
										  void  WriteS64	 (s64 data);
										  void  WriteU32	 (u32 data);
										  void  WriteS32	 (s32 data);
										  void  WriteU16	 (u16 data);
										  void  WriteS16	 (s16 data);
										  void  WriteU8		 (u8 data);
										  void  WriteS8		 (s8 data);
										  void  WriteF64	 (f64 data);
										  void  WriteF32	 (f32 data);
					
					void						WriteLine	 (Engine::Containers::CString s);
					void						WriteText	 (Engine::Containers::CString s);
					void						WriteString	 (Engine::Containers::CString s);
					
					/*
					void						WriteUInt64	(u64 value);
					void						WriteInt64	(s64 value);
					void						WriteUInt32	(u32 value);
					void						WriteInt32	(s32 value);
					void						WriteUInt16	(u16 value);
					void						WriteInt16	(s16 value);
					void						WriteUInt8	(u8  value);
					void						WriteInt8	(s8  value);
					void						WriteFloat32	(f32 value);
					void						WriteFloat64	(f64 value);
					*/
			};

		}
	}
}