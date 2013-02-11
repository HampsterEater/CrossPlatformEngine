///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CString.h"

// Different logging levels.
#define ERROR_LEVEL_DEBUG       0
#define ERROR_LEVEL_INFO        1
#define ERROR_LEVEL_WARNING     2
#define ERROR_LEVEL_ERROR       3
#define ERROR_LEVEL_CRITICAL    4

// Handy macros for logging with file line index.
// It's buttfugging ugly, but pretty speedy when dumping out
// large amounts of debugging crap.
#define LOG_DEBUG(...)        Engine::Debug::CLog::Write(ERROR_LEVEL_ERROR,    __FILE__, __LINE__, __CURRENT_FUNCTION__, __VA_ARGS__)
#define LOG_INFO(...)         Engine::Debug::CLog::Write(ERROR_LEVEL_INFO,     __FILE__, __LINE__, __CURRENT_FUNCTION__, __VA_ARGS__)
#define LOG_WARNING(...)      Engine::Debug::CLog::Write(ERROR_LEVEL_WARNING,  __FILE__, __LINE__, __CURRENT_FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(...)        Engine::Debug::CLog::Write(ERROR_LEVEL_ERROR,    __FILE__, __LINE__, __CURRENT_FUNCTION__, __VA_ARGS__)
#define LOG_CRITICAL(...)     Engine::Debug::CLog::Write(ERROR_LEVEL_CRITICAL, __FILE__, __LINE__, __CURRENT_FUNCTION__, __VA_ARGS__)

// Asserts!
#define LOG_ASSERT_MSG(cond, msg)				if (!(cond)) LOG_CRITICAL("LOG_ASSERT(" #cond ") failed. " msg)
#define LOG_ASSERT_MSG_ARGS(cond, msg, ...)		if (!(cond)) LOG_CRITICAL("LOG_ASSERT(" #cond ") failed. " msg, __VA_ARGS__)
#define LOG_ASSERT(cond)						if (!(cond)) LOG_CRITICAL("LOG_ASSERT(" #cond ") failed")

// Special assert, this bypasses all the engine code to aborted straight out
// to the C runtime. This is used only by the memory allocation code, primarily
// because a normal assert might cause more memory allocation, leading to stack overflows.
// *****************************************************************
// * If this ever fails, something really really bad has happened. *
// *****************************************************************
#define LOG_ASSERT_FAST(cond)					if (!(cond)) { printf("LOG_ASSERT_FAST(" #cond ") failed. Entering infinite loop ...\n"); Engine::Platform::DebugBreak(); while (true) { Sleep(1); } }

//#define LOG_ASSERT_MSG(cond, msg, ...)      if (!(cond)) LOG_CRITICAL("LOG_ASSERT(" #cond ") failed. " ## msg, __VA_ARGS__)
//#define LOG_ASSERT(cond)					 if (!(cond)) LOG_CRITICAL("LOG_ASSERT(" #cond ") failed")


namespace Engine
{
    namespace Debug
    {

        // This class is responsible for storing and emitting information
        // to an output stream.
        class CLog
        {
          private:
            // Properties!
            //static const u32               MAX_BUFFER_SIZE = 2048;
           // static Engine::Containers::CString  m_buffer;

            // Private methods!
            static void WriteToOutput(Engine::Containers::CString msg);
			
            static u32  LogLevel;
			static bool Initialized;
			static u64  StartTime;
            static const char* LOG_NAMES[];

          public:

            //static void SetOutput(CStream* stream);
            //static void CloseOutput();
            //static void DumpCrashLog(bool exit);

            static void SetLogLevel		(u32 level);
            static u32  GetLogLevel		();

			static void Initialize		();
			static void Deinitialize	();

            static void Flush			();
            static void Write			(u32 logLevel, const u8* file, u32 line, const u8* function, Engine::Containers::CString main, ...);//Engine::Containers::CString msg);
        };

    }
}

