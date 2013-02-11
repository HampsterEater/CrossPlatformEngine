///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CLog.h"
#include <stdio.h>
#include <stdarg.h>
#include "Platform.h"

#include "CArray.h"

// Static variable definitions!
//Engine::Containers::CString Engine::Debug::CLog::m_buffer;
u32                     Engine::Debug::CLog::LogLevel;
bool					Engine::Debug::CLog::Initialized;
u64						Engine::Debug::CLog::StartTime;

const char* Engine::Debug::CLog::LOG_NAMES[] = {
    "DEBG",
    "INFO",
    "WARN",
    "ERRO",
    "CRIT"
};

void Engine::Debug::CLog::Write(u32 logLevel, const u8* file, u32 line, const u8* function, Engine::Containers::CString main, ...)
{
    if (LogLevel > logLevel)
        return;
	if (Initialized == false)
		return;
	
	Engine::Containers::CString msg = main;

    // Format with arguments!
    va_list ap;
    va_start(ap, main);
    msg = Engine::Containers::CString::FormatStringVA(msg, ap);
    va_end(ap);

	// Multiple lines?
	if (msg.IndexOf('\n') > 0)
	{
		Engine::Containers::CArray<Engine::Containers::CString> split = msg.Split('\n');
		for (u32 i = 0; i < split.Size(); i++)
			Write(logLevel, file, line, function, split[i]);
		return;
	}

	// Work out elapsed time since log started.
	f64 elapsed_seconds = (f32)(Engine::Platform::GetTicks() - StartTime) / 1000.0;

    // Moar formatting
	Engine::Containers::CString fileInfo = (Engine::Platform::PathFileName(file) + ":" + S(line)).PadEnd(25, ' ');
	msg = S("%s %s %.2f %s\n").Format(Engine::Debug::CLog::LOG_NAMES[logLevel], fileInfo.c_str(),  elapsed_seconds, msg);

 //   Engine::Containers::CString fileInfo = (Engine::Platform::PathFileName(file) + ":" + S(line));
	//msg = S(Engine::Debug::CLog::LOG_NAMES[logLevel]) + fileInfo.PadEnd(17, ' ') + " " + S(function).PadEnd(14, ' ') + " | " + msg + "\n";
	//msg = S(Engine::Debug::CLog::LOG_NAMES[logLevel]) + fileInfo + " " + function + " " + msg + "\n";
//	msg = S(Engine::Debug::CLog::LOG_NAMES[logLevel]) + fileInfo + " " + elapsedTime + " " + msg + "\n";

    //m_buffer += msg;
    //if (m_buffer.Length() > MAX_BUFFER_SIZE)
    //{
    //    WriteToOutput(m_buffer);
    //    m_buffer = "";

/*        s32 amount                      = m_buffer.Length() - MAX_BUFFER_SIZE;
        Engine::Containers::CString start   = m_buffer.SubString(0, amount);
        m_buffer                        = m_buffer.SubString(amount + 1);

        WriteToOutput(start);*/
   // }
	WriteToOutput(msg);

    // Always flush output on errors, we may not get another chance!
    if (logLevel >= ERROR_LEVEL_ERROR)
    {
        Flush();
    }

    // Critical? Fail fast.
    if (logLevel >= ERROR_LEVEL_CRITICAL)
    {
        Engine::Platform::Abort();
    }
}

void Engine::Debug::CLog::WriteToOutput(Engine::Containers::CString msg)
{
    Engine::Platform::StdOutWrite(msg.c_str());
}

void Engine::Debug::CLog::Initialize()
{
	Initialized = true;
	StartTime = Engine::Platform::GetTicks();
}

void Engine::Debug::CLog::Deinitialize()
{
	Initialized = false;
}

void Engine::Debug::CLog::SetLogLevel(u32 level)
{
   LogLevel = level;
}

u32 Engine::Debug::CLog::GetLogLevel()
{
    return LogLevel;
}

void Engine::Debug::CLog::Flush()
{
    //if (m_buffer != "")
   // {
   //     WriteToOutput(m_buffer);
   //     m_buffer = "";
   // }
}
