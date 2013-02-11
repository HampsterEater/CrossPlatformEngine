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
		namespace DNS
		{

			Engine::Containers::CString	IPToString		(u32 ip);
			u32						IPFromString	(Engine::Containers::CString ip);
				
			Engine::Containers::CArray<u32>	LookupHost	(Engine::Containers::CString host);
			Engine::Containers::CString		LookupIP	(u32 ip);
		
		}
	}
}