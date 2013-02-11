///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

namespace Engine
{
    namespace Networking
    {
		class CSocketAddress
		{
			public:
				u32 Host;
				u32 Port;

				CSocketAddress()
				{
				}
				CSocketAddress(u32 h, u32 p) 
				{
					Host = h;
					Port = p;
				}
		};

		class CSocket
		{
			private:
				Engine::Platform::SocketHandle      _socket;
				bool								_opened;

			public:
								CSocket				(Engine::Platform::SocketProtocol protocol);
								CSocket				(Engine::Platform::SocketHandle handle);			
								CSocket				() { }
								~CSocket			();

				bool			Close				();
				bool			Listen				(u32 backlog=0);
				bool			Bind				(CSocketAddress address);
				bool			Connect				(CSocketAddress address, u32 timeout=0);				
				bool			Accept				(CSocket* socket, u32 timeout=0);
				
				bool			IsConnected			();
				u32				BytesAvailable		();

				CSocketAddress	LocalAddress		();
				CSocketAddress	RemoteAddress		();

				void			SetOption			(Engine::Platform::SocketOption option, s32 value);
				s32				GetOption			(Engine::Platform::SocketOption option);

				u32				Send				(const u8* data, u32 length);
				u32				Receive				(u8* data, u32 recvlength);

				u32				SendTo				(const u8* data, u32 length, CSocketAddress address);
				u32				ReceiveFrom			(u8* data, u32 recvlength, CSocketAddress* address);

		};
	}
}