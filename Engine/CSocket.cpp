///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CSocket.h"
#include "CLog.h"

using namespace Engine::Networking;

CSocket::CSocket(Engine::Platform::SocketProtocol protocol)
{
	_opened = true;
	Engine::Platform::SocketOpen(&_socket, protocol);
}

CSocket::CSocket(Engine::Platform::SocketHandle handle)
{
	_socket = handle;
	_opened = true;
}

CSocket::~CSocket()
{
	if (_opened == true)
	{
		Engine::Platform::SocketClose(&_socket);
		_opened = false;
	}
}

bool CSocket::Close()
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");

	_opened = false;
	return Engine::Platform::SocketClose(&_socket);
}

bool CSocket::Listen(u32 backlog)
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");

	return Engine::Platform::SocketListen(&_socket, backlog);
}

bool CSocket::Bind(CSocketAddress address)
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");

	return Engine::Platform::SocketBind(&_socket, address.Host, address.Port);
}

bool CSocket::Connect(CSocketAddress address, u32 timeout)
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");

	return Engine::Platform::SocketConnect(&_socket, address.Host, address.Port, timeout);
}

bool CSocket::Accept(CSocket* socket, u32 timeout)
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");

	Engine::Platform::SocketHandle recieveSocket;

	bool ret = Engine::Platform::SocketAccept(&_socket, &recieveSocket, timeout);
	if (ret == false)
		return false;
	else
		*socket = CSocket(recieveSocket);
		return true;
}

bool CSocket::IsConnected()
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");
	return Engine::Platform::SocketConnected(&_socket);
}

u32	CSocket::BytesAvailable()
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");
	return Engine::Platform::SocketBytesAvail(&_socket);
}

CSocketAddress CSocket::LocalAddress()
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");
	
	CSocketAddress addr;
	Engine::Platform::SocketLocalAddress(&_socket, &addr.Host, &addr.Port);

	return addr;
}

CSocketAddress CSocket::RemoteAddress()
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");
	
	CSocketAddress addr;
	Engine::Platform::SocketRemoteAddress(&_socket, &addr.Host, &addr.Port);

	return addr;
}

void CSocket::SetOption(Engine::Platform::SocketOption option, s32 value)
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");
	Engine::Platform::SocketSetOption(&_socket, option, value);
}

s32	CSocket::GetOption(Engine::Platform::SocketOption option)
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");
	return Engine::Platform::SocketGetOption(&_socket, option);
}

u32 CSocket::Send(const u8* data, u32 length)
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");

	u32 transfered = 0;
	while (transfered < length && IsConnected())
	{
		u32 bytes = Engine::Platform::SocketSend(&_socket, data + transfered, length - transfered);
		transfered += bytes;
	}

	return transfered;
}

u32 CSocket::Receive(u8* data, u32 length)
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");

	u32 transfered = 0;
	while (transfered < length && IsConnected())
	{
		u32 bytes = Engine::Platform::SocketRecieve(&_socket, data + transfered, length - transfered);
		transfered += bytes;
	}

	return transfered;
}

u32 CSocket::SendTo(const u8* data, u32 length, CSocketAddress address)
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");

	u32 transfered = 0;
	while (transfered < length && IsConnected())
	{
		u32 bytes = Engine::Platform::SocketSendTo(&_socket, data + transfered, length - transfered, address.Host, address.Port);
		transfered += bytes;
	}

	return transfered;
}

u32 CSocket::ReceiveFrom(u8* data, u32 length, CSocketAddress* address)
{
	LOG_ASSERT_MSG(_opened == true, "Operation can not be performed on closed socket.");

	u32 transfered = 0;
	while (transfered < length && IsConnected())
	{
		u32 bytes = Engine::Platform::SocketRecieveFrom(&_socket, data + transfered, length - transfered, &address->Host, &address->Port);
		transfered += bytes;
	}

	return transfered;
}
