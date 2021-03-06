///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright � 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////	
// Generated by generate_code_glue.py
// 28-08-2012 00:40
///////////////////////////////////////////////////////////////////////////////	
// WARNING: Do not modify this file in any way! It is automatically generated 
//			as part of the build process it should never have to be manually
//			modified.
///////////////////////////////////////////////////////////////////////////////	
#pragma once

#include "CString.h"
#include "CScriptVirtualMachine.h"

#include "ScriptFunctions_System.h"

namespace Engine
{
	namespace Scripting
	{
		class CScriptExecutionContext;
		
		namespace Native
		{
			namespace Glue
			{
				///////////////////////////////////////////////////////////////////////////////	
				// Our pretty glue functions!
				///////////////////////////////////////////////////////////////////////////////	
void ScriptGlue_0_Print(CScriptExecutionContext* context);


				///////////////////////////////////////////////////////////////////////////////	
				// Registration functions to be called from inside the codebase.
				///////////////////////////////////////////////////////////////////////////////	
				void RegisterScriptFunctions(CScriptVirtualMachine* context);
			}
		}
	}
}
