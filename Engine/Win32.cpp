///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "Win32.h"

// Dependencies
#include <windows.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <dbghelp.h>
#include <stdarg.h>
#include <Shlobj.h>

#include "CLog.h"
#include "Version.h"
#include "Memory.h"
#include "External.h"

#include "CList.h"
#include "CArray.h"
#include "CHashTable.h"

// Platform implementation 
// (caution: order is important! This is including source, not headers!).
#include "Win32_Entry.cpp"
#include "Win32_System.cpp"
#include "Win32_Debug.cpp"
#include "Win32_FileSystem.cpp"
#include "Win32_Sockets.cpp"
#include "Win32_Threads.cpp"


