///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

namespace Engine
{
    namespace Misc
    {

		class CCommandLineParser
		{
			private:

			public:
				CCommandLineParser	();

				void Parse			(const Engine::Containers::CString& str);

				void PrintHelp		();

				void GetString		();
				void GetInt			();
				void GetBool		();

		};

	}
}