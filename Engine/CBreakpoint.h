///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CString.h"
#include "Platform.h"

namespace Engine
{
    namespace Debug
    {

        // This class is responsible for programmatically defining breakpoints.
		// This is not always available on all platforms! Initialize will return
		// false if the breakpoint could not be created.
        class CBreakpoint
        {
			private:
				Engine::Platform::BreakpointHandle _handle;
				bool _initialized;

			public:
				CBreakpoint		();
				~CBreakpoint	();

				void Force		();

				bool Initialize	(void* ptr=NULL, u32 size=4, Engine::Platform::BreakpointMode flags=Engine::Platform::BREAKPOINT_READWRITE);
				void Dispose	();
		};

    }
}

