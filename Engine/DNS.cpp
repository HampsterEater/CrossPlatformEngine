///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "DNS.h"
#include "CArray.h"

using namespace Engine::Networking::DNS;

Engine::Containers::CString Engine::Networking::DNS::IPToString(u32 ip)
{
	return Engine::Platform::DnsIPToString(ip);
}

u32	Engine::Networking::DNS::IPFromString(Engine::Containers::CString ip)
{
	return Engine::Platform::DnsStringToIP(ip);
}
				
Engine::Containers::CArray<u32> Engine::Networking::DNS::LookupHost(Engine::Containers::CString host)
{
	return Engine::Platform::DnsLookupHost(host);
}

Engine::Containers::CString Engine::Networking::DNS::LookupIP(u32 ip)
{
	return Engine::Platform::DnsLookupIP(ip);
}
