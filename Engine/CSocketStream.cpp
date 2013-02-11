///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CSocketStream.h"
#include "CLog.h"

using namespace Engine::FileSystem::Streams;

CSocketStream::CSocketStream(Engine::Networking::CSocket* socket, bool autoclose)
{
	_socket = socket;
	_opened = false;
	_autoClose = autoclose;
}

CSocketStream::~CSocketStream()
{
	if (_opened == true)
		Close();
}

bool CSocketStream::Open()
{
	LOG_ASSERT_MSG(_opened == false,	  "Attempt to open already opened socket stream.");
	LOG_ASSERT_MSG(_socket->IsConnected(), "Attempt to open already open socket stream on closed socket.");
	_opened = true;
	return _opened;
}

void CSocketStream::Close()
{
	LOG_ASSERT_MSG(_opened == true, "Attempt to open closed socket stream.");
	_socket->Close();
	_opened = false;
}

void CSocketStream::Seek(u64 position)
{
	LOG_ASSERT_MSG(false, "Socket streams do not support seeking.");
}

void CSocketStream::Flush()
{
	// Sockets don't need flushing.
}

u64 CSocketStream::Position()
{
	LOG_ASSERT_MSG(false, "Socket streams do not support getting a position.");
	return 0;
}

u64 CSocketStream::Length()
{
	LOG_ASSERT_MSG(false, "Socket streams do not support getting then length.");
	return 0;
}

bool CSocketStream::AtEnd()
{
	return !_socket->IsConnected();
}

void CSocketStream::ReadBytes(u8* buffer, u64 length)
{
	LOG_ASSERT_MSG(_opened == true, "Attempt to access closed socket stream.");
	LOG_ASSERT_MSG(_socket->IsConnected(), "Attempt to access disconnected socket stream.");

	_socket->Receive(buffer, (u32)length);
}

void CSocketStream::WriteBytes(const u8* buffer, u64 length)
{
	LOG_ASSERT_MSG(_opened == true, "Attempt to access closed socket stream.");
	LOG_ASSERT_MSG(_socket->IsConnected(), "Attempt to access disconnected socket stream.");

	_socket->Send(buffer, (u32)length);
}