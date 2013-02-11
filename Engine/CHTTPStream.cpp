///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CHTTPStream.h"
#include "CLog.h"
#include "Memory.h"
#include "CArray.h"

using namespace Engine::FileSystem::Streams;

CHTTPStream::CHTTPStream(const Engine::Containers::CString& url)
{
	Engine::Containers::CString real = url;
	
	// Strip off http:// part.
	s32 index = real.IndexOf("//");
	if (index >= 0)
		real = real.SubString(index + 2);

	// Crack host and request path.
	index = real.IndexOf("/");
	if (index >= 0)
	{
		_hostname = real.SubString(0, index);
		_requestPath = real.SubString(index);
	}
	else
	{
		_hostname = real;
		_requestPath = "/";
	}

	// Split hostname and port.
	index = _hostname.IndexOf(":");
	if (index >= 0)
	{
		_port = _hostname.SubString(index + 1).ToInt();
		_hostname = _hostname.SubString(0, index);
	}
	else
	{
		_port = 80;
	}

	// Set values.
	_requestBody		= NULL;
	_requestBodyLength	= 0;
	_requestMethod		= "GET";
	_requestHeaders		= "";
	_opened				= false;
	_responseHeaderCount = 0;
	_position			 = 0;
	_length				 = 0;
}

CHTTPStream::~CHTTPStream()
{
	if (_opened != 0)
		Close();
}

void CHTTPStream::SetRequestMethod(const Engine::Containers::CString& method)
{
	LOG_ASSERT_MSG(_opened == false, "Attempt to set request variables of http stream after it has been opened.");
	_requestMethod = method;
}

void CHTTPStream::SetRequestBody(u8* data, u32 length)
{
	LOG_ASSERT_MSG(_opened == false, "Attempt to set request variables of http stream after it has been opened.");
	_requestBody = data;
	_requestBodyLength = length;
}

void CHTTPStream::AddHeader(const Engine::Containers::CString& name, const Engine::Containers::CString& value)
{
	LOG_ASSERT_MSG(_opened == false, "Attempt to set request variables of http stream after it has been opened.");
	_requestHeaders += name + ": " + value + "\n";
}

u32	CHTTPStream::GetResponseStatus()
{
	return _responseStatusCode;
}

Engine::Containers::CString CHTTPStream::GetResponseHeader(const Engine::Containers::CString& name)
{
	Engine::Containers::CString lower = name;
	lower = lower.ToLower();

	for (u32 i = 0; i < _responseHeaderCount; i++)
	{
		if (lower == _responseHeaderNames[_responseHeaderCount].ToLower())
		{
			return _responseHeaderValues[_responseHeaderCount];
		}
	}

	return "";
}

bool CHTTPStream::Open()
{
	LOG_ASSERT_MSG(!_opened, "Attempt to open http stream when its already open.");

	// Resolve hostname.
	Engine::Containers::CArray<u32> ips = Engine::Networking::DNS::LookupHost(_hostname);
	if (ips.Size() <= 0)
		return false;

	// Create a new socket.
	Engine::Networking::CSocketAddress address(ips[0], _port);
	_socket = Engine::Memory::GetDefaultAllocator()->NewObj<Engine::Networking::CSocket>(Engine::Platform::SOCKET_PROTOCOL_TCP);

	// Connect to host.
	if (!_socket->Connect(address))
		return false;

	// Write request.
	Engine::Containers::CString data = "";

	// Send request start.
	data = _requestMethod + " " + _requestPath + " HTTP/1.0\n";
	data += "Host: "+_hostname+"\n";
	data += "Connection: close\n";

	// Send request headers.
	if (_requestHeaders != "")
	{
		data += _requestHeaders;
	}

	// Send content size.
	if (_requestBody != NULL)
	{
		data += S("Content-Length: ") + S(_requestBodyLength) + "\n";
	}

	// Send end of request.
	data += "\n";
	_socket->Send(data.c_str(), data.Length());

	// Send body of request.
	if (_requestBody != NULL)
	{
		_socket->Send(_requestBody, _requestBodyLength);
	}

	// Just to stop the end-of-stream tripping while we read. Hack.
	_length = -1;

	// Read the status return.
	Engine::Containers::CString statusline = ReadLine();

	u32 index1 = statusline.IndexOf(' ');
	u32 index2 = statusline.IndexOf(' ', index1 + 1);

	_responseStatusCode = statusline.SubString(index1 + 1, index2 - index1 - 1).ToInt();

	// Read in header response.
	while (!AtEnd())
	{
		Engine::Containers::CString line = ReadLine();
		if (line == "" || line == "\n" || line == "\r")
		{
			break;
		}
		else
		{
			if (_responseHeaderCount < MAX_HTTP_RESPONSE_HEADER_COUNT)
			{
				s32 index = line.IndexOf(':');
				if (index <= 0)
				{
					continue;
				}

				Engine::Containers::CString name = line.SubString(0, index).Trim();
				Engine::Containers::CString value = line.SubString(index + 1).Trim();

				_responseHeaderNames [_responseHeaderCount] = name;
				_responseHeaderValues[_responseHeaderCount] = value;

				if (name.ToLower() == "content-length")
				{
					_length = value.ToInt();
				}

				_responseHeaderCount++;
			}
		}
	}

	_position = 0;

	_opened = true;
	return true;
}

void CHTTPStream::Close()
{
	LOG_ASSERT_MSG(_opened == false, "Attempt to close http stream when its already closed.");
	_opened = false;

	_socket->Close();
	Engine::Memory::GetDefaultAllocator()->FreeObj(&_socket);
	_socket = NULL;
}

void CHTTPStream::Seek(u64 position)
{
	LOG_ASSERT_MSG(false, "HTTP streams do not support seeking.");
}

void CHTTPStream::Flush()
{
	// Nothing to do.
}

u64 CHTTPStream::Position()
{
	return _position;
}

u64 CHTTPStream::Length()
{
	return _length;
}

bool CHTTPStream::AtEnd()
{
	return (_position >= _length && _length > 0 && _socket->BytesAvailable() <= 0);
}

void CHTTPStream::ReadBytes(u8* buffer, u64 length)
{
	_socket->Receive(buffer, (u32)length);
	_position += (u32)length;
}

void CHTTPStream::WriteBytes(const u8* buffer, u64 length)
{
	LOG_ASSERT_MSG(false, "HTTP streams do not support writing.");
}