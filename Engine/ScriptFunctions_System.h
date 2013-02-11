///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CString.h"
#include "CArray.h"

namespace Engine
{
    namespace Scripting
    {
		class CScriptExecutionContext;

		namespace Native
		{
			namespace System
			{
				
				// [Script]
				void Print(const Engine::Containers::CString& str);
				
			}
		}
	}
}