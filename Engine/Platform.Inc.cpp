///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "Platform.Inc.h"

// Include platform specific implementation.
#ifdef OS_WIN
    #include "Win32.cpp"

//#elif OS_LINUX
 //   #include "Linux.cpp"

//#elif OS_MAC
//    #include "MacOS.cpp"

//#elif OS_IOS
//    #include "iOS.cpp"

#else
    #error (Attempt to include platform.h on unknown or unsupported platform!)

#endif