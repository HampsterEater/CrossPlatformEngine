///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CStream.h"
#include "CSocket.h"

namespace Engine
{
    namespace FileSystem
    {
		namespace Streams
		{
			class CSocketStream : public CStream
			{
				private:
					Engine::Networking::CSocket* _socket;
					bool					     _opened;
					bool					 	 _autoClose;

				public:
					
					CSocketStream(Engine::Networking::CSocket* socket, bool autoclose=false);
					~CSocketStream();

					bool Open		();
					void Close		();
					void Seek		(u64 position);
					void Flush		();
					u64  Position	();
					u64  Length		();
					bool AtEnd		();
					void ReadBytes	(u8* buffer, u64 length);
					void WriteBytes	(const u8* buffer, u64 length);

			};
		}
	}
}