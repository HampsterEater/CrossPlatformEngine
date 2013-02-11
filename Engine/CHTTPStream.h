///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CStream.h"
#include "CSocket.h"
#include "DNS.h"

namespace Engine
{
    namespace FileSystem
    {
		namespace Streams
		{

			#define MAX_HTTP_RESPONSE_HEADER_COUNT 32

			class CHTTPStream : public CStream
			{
				private:
					Engine::Networking::CSocket* _socket;

					Engine::Containers::CString	_hostname;
					u32						_port;
					Engine::Containers::CString	_requestPath;

					Engine::Containers::CString	_requestMethod;
					u8*						_requestBody;
					u32						_requestBodyLength;
					Engine::Containers::CString	_requestHeaders;

					Engine::Containers::CString	_responseHeaderNames[MAX_HTTP_RESPONSE_HEADER_COUNT];
					Engine::Containers::CString	_responseHeaderValues[MAX_HTTP_RESPONSE_HEADER_COUNT];
					u32						_responseHeaderCount;
					u32						_responseStatusCode;

					u32						_opened;

					s32						_position;
					s32						_length;

				public:
					CHTTPStream					(const Engine::Containers::CString& url);
					~CHTTPStream				();

					void SetRequestMethod		(const Engine::Containers::CString& method);
					void SetRequestBody			(u8* data, u32 length);

					void AddHeader				(const Engine::Containers::CString& name, const Engine::Containers::CString& value);
					//void AddAttachment			();

					u32					   GetResponseStatus		();
					Engine::Containers::CString GetResponseHeader		(const Engine::Containers::CString& name);

					bool Open					();
					void Close					();
					void Seek					(u64 position);
					void Flush					();
					u64  Position				();
					u64  Length					();
					bool AtEnd					();
					void ReadBytes				(u8* buffer, u64 length);
					void WriteBytes				(const u8* buffer, u64 length);

			};
		}
	}
}