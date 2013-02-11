///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "Platform.h"
#include "CScriptManager.h"

// Include platform specific implementation.
#ifdef OS_WIN
    #include "Win32.cpp"

//#elif OS_LINUX
 //   #include "Linux.cpp"

//#elif OS_MAC
//    #include "MacOS.cpp"

//#elif OS_IOS
//    #include "iOS.cpp"

//#elif OS_XBOX360
//    #include "Xbox360.cpp"

#else
    #error (Attempt to include platform.h on unknown or unsupported platform!)

#endif