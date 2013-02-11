///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

namespace Engine
{
    namespace Platform
    {

		bool SocketSysBegin()
		{
			WSADATA wsaData = {0};
			return (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
		}

		bool SocketSysFinish()
		{
			return (WSACleanup() == 0);
		}

		bool SocketOpen(SocketHandle* handle, SocketProtocol protocol)
		{ 
			s32 win32type = 0;
			s32 win32protocol = 0;

			switch (protocol)
			{
				case SOCKET_PROTOCOL_TCP:
					win32type = SOCK_STREAM;
					win32protocol = IPPROTO_TCP;
					break;
				
				case SOCKET_PROTOCOL_UDP:
					win32type = SOCK_DGRAM;
					win32protocol = IPPROTO_UDP;
					break;

				default:
					LOG_ERROR("Unsupported protocol requested when opening socket!");
					return false;
			}

			handle->_socketHandle = socket(AF_INET, win32type, win32protocol);
			if (handle->_socketHandle == INVALID_SOCKET)
			{ 
				u32 ec = WSAGetLastError();
                LOG_ERROR(S("Failed to open new socket! WSAGetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));
				return false;
			}

			// Turn non-blocking on.
			u_long iMode = 1;
			ioctlsocket(handle->_socketHandle, FIONBIO, &iMode);

			return true;
		}

		bool SocketClose(SocketHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");
			
			if (closesocket(handle->_socketHandle) != 0)
			{ 
				u32 ec = WSAGetLastError();
                LOG_ERROR(S("Failed to close socket! WSAGetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));
				return false;
			}			

			handle->_socketHandle = INVALID_SOCKET;
			return true;
		}

		bool SocketBind(SocketHandle* handle, u32 host, u32 port)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");
			
			sockaddr_in addr;
			addr.sin_family		 = AF_INET;
			addr.sin_addr.s_addr = host;
			addr.sin_port		 = htons(port);

			if (bind(handle->_socketHandle, (SOCKADDR*)&addr, sizeof(addr)) != 0)
			{ 
				u32 ec = WSAGetLastError();
                LOG_ERROR(S("Failed to bind socket! WSAGetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));
				return false;
			}	

			return true;		
		}

		bool SocketListen(SocketHandle* handle, s32 backlog)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");

			if (backlog == 0)
				backlog = SOMAXCONN;

			if (listen(handle->_socketHandle, backlog) != 0)
			{ 
				u32 ec = WSAGetLastError();
                LOG_ERROR(S("Failed to listen on socket! WSAGetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));
				return false;
			}	

			return true;
		}
		
		bool SocketConnect(SocketHandle* handle, u32 host, u32 port, u32 timeout)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");
			
			sockaddr_in addr;
			memset(&addr, 0, sizeof(addr));

			addr.sin_family		 = AF_INET;
			addr.sin_addr.s_addr = host;
			addr.sin_port		 = htons(port);

			// Connect!
			s32 ret = connect(handle->_socketHandle, (SOCKADDR*)&addr, sizeof(addr));
			if (ret != 0)
			{
				u32 ec = WSAGetLastError();
				if (ec != WSAEWOULDBLOCK)
				{
	                LOG_ERROR(S("Failed to connect on socket! WSAGetLastError()=%i (%s)").Format(ec, FormatSystemError(ec).c_str()));
					return false;
				}
			}

			// Wait for connection.
			timeval timer;
			timer.tv_sec = timeout / 1000;
			timer.tv_usec = (timeout % 1000) * 1000;

			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(handle->_socketHandle, &fds);
			s32 result = select(handle->_socketHandle + 1, NULL, &fds, NULL, timeout == 0 ? NULL : &timer);

			// Success!
			if (result == 1)
			{
				u8  error;
				s32 len = sizeof(error);
				getsockopt(handle->_socketHandle, SOL_SOCKET, SO_ERROR, &error, &len);
					
				if (error == 0)
					return true;
				else
					return false;
			}

			// Failed.
			else
			{
				return false;
			}

			return false;
		}
		
		bool SocketConnected(SocketHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");
			
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(handle->_socketHandle, &fds);
			u32 ret = select(0, NULL, &fds, &fds, NULL);
			
			return (ret != 0 || SocketBytesAvail(handle) != 0);
		}

		u32 SocketBytesAvail(SocketHandle* handle)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");
			
			u_long bytes = 0;
			if (ioctlsocket(handle->_socketHandle, FIONREAD, &bytes) < 0)
				return 0;
			return bytes;
		}
		
		bool SocketAccept(SocketHandle* handle, SocketHandle* recieveSocket, u32 timeout)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");

			// Wait for connection.
			timeval timer;
			timer.tv_sec = timeout / 1000;
			timer.tv_usec = (timeout % 1000) * 1000;

			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(handle->_socketHandle, &fds);
			s32 result = select(handle->_socketHandle + 1, &fds, NULL, NULL, &timer);

			// Success, we have something to accept.
			if (result != 1)
			{
				return false;
			}

			// Accept connection.
			sockaddr  addr;
			s32		  addr_size = sizeof(addr);

			SOCKET connected = accept(handle->_socketHandle, &addr, &addr_size);
			if (connected == INVALID_SOCKET)
			{
				return false;
			}
			else
			{
				recieveSocket->_socketHandle = connected;
				return true;
			}
		}

		void SocketLocalAddress(SocketHandle* handle, u32* host, u32* port)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");

			sockaddr_in addr;
			s32			addr_size = sizeof(addr);

			if (getsockname(handle->_socketHandle, (sockaddr*)&addr, &addr_size) < 0)
			{
				*host = 0;
				*port = 0;
			}
			else
			{
				*host = addr.sin_addr.S_un.S_addr;
				*port = ntohs(addr.sin_port);
			}
		}

		void SocketRemoteAddress(SocketHandle* handle, u32* host, u32* port)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");

			sockaddr_in addr;
			s32			addr_size = sizeof(addr);

			if (getpeername(handle->_socketHandle, (sockaddr*)&addr, &addr_size) < 0)
			{
				*host = 0;
				*port = 0;
			}
			else
			{
				*host = addr.sin_addr.S_un.S_addr;
				*port = ntohs(addr.sin_port);
			}
		}

		u32 SocketSend(SocketHandle* handle, const u8* buffer, u32 length)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");

			u32 s = send(handle->_socketHandle, buffer, length, 0);
			if (s == SOCKET_ERROR)
				return 0;
			else
				return s;
		}

		u32 SocketRecieve(SocketHandle* handle, u8* buffer, u32 length)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");
			
			u32 r = recv(handle->_socketHandle, buffer, length, 0);
			if (r == SOCKET_ERROR)
				return 0;
			else
				return r;
		}		
		
		u32 SocketSendTo(SocketHandle* handle, const u8* buffer, u32 length, u32 host, u32 port)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");
			
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			addr.sin_addr.s_addr = host;

			u32 r = sendto(handle->_socketHandle, buffer, length, 0, (SOCKADDR *)&addr, sizeof(addr));
			if (r == SOCKET_ERROR)
				return 0;

			return r;
		}

		u32 SocketRecieveFrom(SocketHandle* handle, u8* buffer, u32 recvlength, u32* host, u32* port)
		{
            LOG_ASSERT_MSG(handle != NULL && handle->_socketHandle != INVALID_SOCKET, "Socket handle passed was NULL.");
			
			sockaddr_in addr;
			s32	addrlen = sizeof(addr);

			u32 r = recvfrom(handle->_socketHandle, buffer, recvlength, 0, (SOCKADDR *)&addr, &addrlen);
			if (r == SOCKET_ERROR)
				return 0;

			*host = addr.sin_addr.S_un.S_addr;
			*port = addr.sin_port;

			return r;
		}

		void SocketSetOption(SocketHandle* handle, SocketOption option, s32 value)
		{
			switch (option)
			{
				case SOCKET_OPTION_NODELAY: 
					setsockopt(handle->_socketHandle, IPPROTO_TCP, TCP_NODELAY, (char*)&value, 4);
					break;

				case SOCKET_OPTION_BROADCAST:
					setsockopt(handle->_socketHandle, SOL_SOCKET, SO_BROADCAST, (char*)&value, 4);
					break;

				case SOCKET_OPTION_RECIEVE_BUFFER: 
					setsockopt(handle->_socketHandle, SOL_SOCKET, SO_RCVBUF, (char*)&value, 4);
					break;

				case SOCKET_OPTION_SEND_BUFFER:
					setsockopt(handle->_socketHandle, SOL_SOCKET, SO_SNDBUF, (char*)&value, 4);
					break;

				case SOCKET_OPTION_RECIEVE_TIMEOUT:
					setsockopt(handle->_socketHandle, SOL_SOCKET, SO_RCVTIMEO, (char*)&value, 4);
					break;

				case SOCKET_OPTION_SEND_TIMEOUT:
					setsockopt(handle->_socketHandle, SOL_SOCKET, SO_SNDTIMEO, (char*)&value, 4);
					break;
			}
		}

		s32 SocketGetOption(SocketHandle* handle, SocketOption option)
		{
			s32 val		= 0;
			s32 vallen	= 4;

			switch (option)
			{
				case SOCKET_OPTION_NODELAY: 
					getsockopt(handle->_socketHandle, IPPROTO_TCP, TCP_NODELAY, (char*)&val, &vallen);
					break;

				case SOCKET_OPTION_BROADCAST:
					getsockopt(handle->_socketHandle, SOL_SOCKET, SO_BROADCAST, (char*)&val, &vallen);
					break;

				case SOCKET_OPTION_RECIEVE_BUFFER: 
					getsockopt(handle->_socketHandle, SOL_SOCKET, SO_RCVBUF, (char*)&val, &vallen);
					break;

				case SOCKET_OPTION_SEND_BUFFER:
					getsockopt(handle->_socketHandle, SOL_SOCKET, SO_SNDBUF, (char*)&val, &vallen);
					break;

				case SOCKET_OPTION_RECIEVE_TIMEOUT:
					getsockopt(handle->_socketHandle, SOL_SOCKET, SO_RCVTIMEO, (char*)&val, &vallen);
					break;

				case SOCKET_OPTION_SEND_TIMEOUT:
					getsockopt(handle->_socketHandle, SOL_SOCKET, SO_SNDTIMEO, (char*)&val, &vallen);
					break;
			}

			return val;
		}

		u32	DnsStringToIP(Engine::Containers::CString ip)
		{
			return inet_addr(ip.c_str());
		}

		Engine::Containers::CString DnsIPToString(u32 ip)
		{
			in_addr addr;
			addr.S_un.S_addr = ip;
			return inet_ntoa(addr);
		}

		Engine::Containers::CArray<u32> DnsLookupHost(Engine::Containers::CString host)
		{
			Engine::Containers::CArray<u32> ips;
			
			struct hostent* ent = gethostbyname(host.c_str());
			if (ent == NULL || ent->h_length != 4)
				return false;

			char** addrlist = ent->h_addr_list;
			while (addrlist != NULL)
			{
				u32* addr = (u32*)*addrlist;
				if (addr == NULL)
					break;

				ips.AddToEnd(*addr);

				addrlist++;
			}

			return ips;
		}

		Engine::Containers::CString DnsLookupIP(u32 ip)
		{
			u_long addr = ip;
			struct hostent* ent = gethostbyaddr((char*)&addr, 4, AF_INET);

			if (ent == NULL)
				return DnsIPToString(ip);
			else
				return ent->h_name;
		}

	}
}