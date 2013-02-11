///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#include "ScriptFunctions_System.h"
#include "CScriptVirtualMachine.h"

using namespace Engine::Scripting;
using namespace Engine::Scripting::Native::System;

void Engine::Scripting::Native::System::Print(const Engine::Containers::CString& str)
{
	LOG_INFO("Script: " + str);
}