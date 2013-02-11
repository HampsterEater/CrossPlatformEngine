///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// Global variables wooo.
// ----------------------------------------------------------------------------
s32				 g_platform_argc			= 0;
u8**			 g_platform_argv			= NULL;

CRITICAL_SECTION g_dbghelp_critical_section;
u32			     g_main_thread_id			= 0;
HANDLE			 g_main_thread_handle		= NULL;

jmp_buf			 g_error_recovery_longjmp;

// ----------------------------------------------------------------------------
// Helper functions for windows only. Should not be accessed outside this file.
// ----------------------------------------------------------------------------
FILETIME UnixTimeToFileTime(u32 unixTime)
{
    LONGLONG ll;
    FILETIME ft;

    ll = Int32x32To64(unixTime, 10000000) + 116444736000000000LL;
    ft.dwLowDateTime = (DWORD)ll;
    ft.dwHighDateTime = ll >> 32;

    return ft;
}

u32 FileTimeToUnixTime(FILETIME fileTime)
{
    LARGE_INTEGER date, adjust;
    date.HighPart = fileTime.dwHighDateTime;
    date.LowPart = fileTime.dwLowDateTime;

    // Convert milliseconds to nanoseconds.
    adjust.QuadPart = 11644473600000LL * 10000;

    // Subtract difference between 1970 and 1601.
    date.QuadPart -= adjust.QuadPart;

    // Return the number of seconds!
    return (u32)(date.QuadPart / 10000000);
}

// This lovely struct + function are used to set the name of a thread
// in the visual studio debugger. Only works in visual studio!
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // must be 0x1000
   LPCSTR szName; // pointer to name (in user addr space)
   DWORD dwThreadID; // thread ID (-1=caller thread)
   DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;
void Win32SetVSThreadName(DWORD dwThreadID, const u8* szThreadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

Engine::Containers::CString FormatSystemError(DWORD error)
{
    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    Engine::Containers::CString result = Engine::Containers::CString((u8*)lpMsgBuf);

    LocalFree(lpMsgBuf);

    result = result.Trim();

    return result;
}

u16 Win32CaptureStackBackTraceSlow(u16 frameOffset, u16 maxFrames, void* frames, CONTEXT* overrideContext, HANDLE thread)
{
	// Don't really want the frame for this function!
	frameOffset += 1;

	// If thread is not given, use current.
	if (thread == NULL)
	{
		thread = GetCurrentThread();
	}

	// Capture context for this thread.
	CONTEXT context;
	ZeroMemory(&context, sizeof(CONTEXT));
	
	// 32bit has no function call to get the current
	// threads context, so its asm trickery time :3
	#ifdef ARCH_BIT_32

		if (overrideContext != NULL)
		{
			memcpy(&context, overrideContext, sizeof(CONTEXT));
		}
		else
		{
			context.ContextFlags = CONTEXT_CONTROL;	
			__asm
			{
			Label:
				mov [context.Ebp], ebp;
				mov [context.Esp], esp;
				mov eax, [Label];
				mov [context.Eip], eax;
			}
		}

	// 64bit does though, w00t.
	#else
	
		if (overrideContext != NULL)
		{
			memcpy(&context, overrideContext, sizeof(CONTEXT));
		}
		else
		{
			RtlCaptureContext(&context);
		}

	#endif
				
	// Build the initial stack frame.
	STACKFRAME64 stackFrame;
	DWORD		 machineType;
	ZeroMemory(&stackFrame, sizeof(STACKFRAME64));

	#ifdef X86
		machineType                 = IMAGE_FILE_MACHINE_I386;
		stackFrame.AddrPC.Offset    = context.Eip;
		stackFrame.AddrPC.Mode      = AddrModeFlat;
		stackFrame.AddrFrame.Offset = context.Ebp;
		stackFrame.AddrFrame.Mode   = AddrModeFlat;
		stackFrame.AddrStack.Offset = context.Esp;
		stackFrame.AddrStack.Mode   = AddrModeFlat;
	#elif AMD64
		machineType                 = IMAGE_FILE_MACHINE_AMD64;
		stackFrame.AddrPC.Offset    = context.Rip;
		stackFrame.AddrPC.Mode      = AddrModeFlat;
		stackFrame.AddrFrame.Offset = context.Rsp;
		stackFrame.AddrFrame.Mode   = AddrModeFlat;
		stackFrame.AddrStack.Offset = context.Rsp;
		stackFrame.AddrStack.Mode   = AddrModeFlat;
	#elif ITANIUM64
		machineType                 = IMAGE_FILE_MACHINE_IA64;
		stackFrame.AddrPC.Offset    = context.StIIP;
		stackFrame.AddrPC.Mode      = AddrModeFlat;
		stackFrame.AddrFrame.Offset = context.IntSp;
		stackFrame.AddrFrame.Mode   = AddrModeFlat;
		stackFrame.AddrBStore.Offset= context.RsBSP;
		stackFrame.AddrBStore.Mode  = AddrModeFlat;
		stackFrame.AddrStack.Offset = context.IntSp;
		stackFrame.AddrStack.Mode   = AddrModeFlat;
	#else
		return 0;

	#endif

	EnterCriticalSection(&g_dbghelp_critical_section);

	// Stack trace!
	u32 offset = 0;
	u32 framesRetrieved = 0;
	while (framesRetrieved < maxFrames)
	{
		if (!StackWalk64(machineType, GetCurrentProcess(), thread, &stackFrame, 
							machineType == IMAGE_FILE_MACHINE_I386 ? NULL : &context, 
							NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
		{
			// Failure :(
			break;
		}

		if (++offset <= frameOffset)
		{
			continue;
		}
		
		if (stackFrame.AddrPC.Offset != 0)
		{
#ifdef ARCH_BIT_64
			*((u64*)frames + framesRetrieved) = stackFrame.AddrPC.Offset;
#else
			*((u32*)frames + framesRetrieved) = (u32)stackFrame.AddrPC.Offset;
#endif
			framesRetrieved++;
		}
		else
		{
			// Reached the base of the stack.
			break;
		}
	}

	LeaveCriticalSection(&g_dbghelp_critical_section);

	return framesRetrieved;
}

// ----------------------------------------------------------------------------
// Invoked when a fault occurs in the applications (uncaught exception, signal,
// early abort, or so forth).
// ----------------------------------------------------------------------------
void PlatformFault(struct _EXCEPTION_POINTERS *exceptionInfo=NULL)
{
    Engine::Containers::CString dumpdir = Engine::Platform::PathGetWorkingDir() + "\\dumps";
    Engine::Containers::CString dumppath = dumpdir + "\\00000000.dmp";

    LOG_ERROR("~~~~~~~~~~~~ UNHANDLED EXCEPTION OCCURRED ~~~~~~~~~~~");
    LOG_ERROR("PlatformFault() invoked ...");

    // Make sure dump file exists.
    if (!Engine::Platform::PathIsDirectory(dumpdir))
    {
        LOG_ERROR("Attempt to create dump folder ...");
        LOG_ERROR(S("Path: ") + dumpdir);

        if (!Engine::Platform::DirCreate(dumpdir, true))
        {
            LOG_ERROR("Failed to create dump folder. Aborting.");
            exit(0);
        }
    }

    // Find somewhere to dump the file.
    u32 index = 0;
    do
    {
        dumppath = dumpdir + "\\" + Engine::Containers::CString(index).PadStart(8, '0') + ".dmp";
        index++;

    } while (Engine::Platform::PathIsFile(dumppath));

    // Dump the file!
    LOG_ERROR("Attempt to create dump file ...");
    LOG_ERROR(S("Path: ") + dumppath);

    HANDLE fileHandle = CreateFile(dumppath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == NULL)
    {
        LOG_ERROR("Failed to create open dump file. Aborting.");
        exit(0);
    }

    // Duuuuuuuuump.
    HANDLE process = GetCurrentProcess();

    MINIDUMP_EXCEPTION_INFORMATION exceptionParam;
    exceptionParam.ThreadId          = GetCurrentThreadId();
    exceptionParam.ExceptionPointers = exceptionInfo;
    exceptionParam.ClientPointers    = FALSE;
	
	EnterCriticalSection(&g_dbghelp_critical_section);

    BOOL result = MiniDumpWriteDump(process,
                                    GetCurrentProcessId(),
                                    fileHandle,
                                    MiniDumpWithFullMemory,
                                    exceptionInfo == NULL ? NULL : &exceptionParam,
                                    NULL,
                                    NULL);
	
	LeaveCriticalSection(&g_dbghelp_critical_section);

    // Close the dump file handle.
    CloseHandle(fileHandle);

    // Check a file was created.
    if (result == FALSE)
    {
        u32 ec = GetLastError();
        LOG_ERROR(S("Failed to create dump file, GetLastError()=%i (%s)"), ec, FormatSystemError(ec).c_str());
    }
    else
    {
        LOG_ERROR("Success!");
    }

	// Dump stack trace.
	LOG_ERROR("");
	LOG_ERROR("Call Stack:");
	void* frames[MAX_STACK_FRAMES];
	u16 frameCount = Win32CaptureStackBackTraceSlow(0, MAX_STACK_FRAMES, frames, exceptionInfo->ContextRecord, GetCurrentThread());
	for (u16 i = 0; i < frameCount; i++)
	{
		#ifdef ARCH_BIT_64
			u64 addr = (u64)frames[i];
		#else
			u32 addr = (u64)frames[i];
		#endif

		Engine::Platform::StackFrame frame = Engine::Platform::DebugResolveAddressToStackFrame(addr);				
		LOG_ERROR(S("\t[%i] %s (%i): %s").Format(i, frame.file, frame.line, frame.name)); 
	}
	LOG_ERROR("");

	// Bail the fuck out.
    // We use this to long jump back to the point after PlatformMain is called
    // so we can deinitialize everything.
	if (GetCurrentThreadId() == g_main_thread_id)
	{
	    LOG_ERROR("Attempting recovery through longjmp ...");
		longjmp(g_error_recovery_longjmp, 1);
	}
}

// ----------------------------------------------------------------------------
// Used with SetExceptionHandler, recieves any uncaught exceptions and
// dumps out debugging information about them before bailing out.
// ----------------------------------------------------------------------------
LONG WINAPI ProgramExceptionHandler(struct _EXCEPTION_POINTERS *exceptionInfo)
{
    PlatformFault(exceptionInfo);
    return EXCEPTION_EXECUTE_HANDLER;
}

// ----------------------------------------------------------------------------
// This is windows entry pos32. It's only purpose is to setup error handling
// and call the platform independent entry pos32.
// ----------------------------------------------------------------------------
s32 main(s32 argc, u8* argv[])
{
	s32 exitcode = 0;
	g_platform_argc = argc;
	g_platform_argv = argv;
	
	// Disable any pita popups that occassionally occur when checking drive file
	// sizes, errors in libraries, etc.
	SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOALIGNMENTFAULTEXCEPT|SEM_NOOPENFILEERRORBOX|SEM_NOGPFAULTERRORBOX);

	// Initialize the globals (needs to be done early, allocators use these).
	InitializeCriticalSection(&g_dbghelp_critical_section);
	g_main_thread_id = GetCurrentThreadId();
	g_main_thread_handle = GetCurrentThread();

	// Symbols need to be initialized if we want debug stack traces and such
	// when the allocators are initialized.
	SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
	if (!SymInitialize(GetCurrentProcess(), NULL, TRUE))
	{    
		// :(. Well we can survive without symbols.		
	}

    // README: Always setup global memory pools first, attempting to do
    // anything else will cause things to go batshit insane :).
    Engine::Memory::InitDefaultAllocator(DEFAULT_ALLOCATOR_START_MEMORY, DEFAULT_ALLOCATOR_MAX_MEMORY, DEFAULT_ALLOCATOR_MEMORY_CHUNK_SIZE);
	Engine::Containers::InitStringAllocator();
	Engine::Containers::InitListAllocator();
	Engine::Containers::InitArrayAllocator();
	Engine::Containers::InitHashTableAllocator();
	Engine::Scripting::InitScriptAllocator();

	{
		// Change working directory to the directory the executable is in.
		Engine::Platform::PathSetWorkingDir(Engine::Platform::PathDirectoryName(argv[0]));

		// Setup logging.
		Engine::Debug::CLog::Initialize();
	#if RELEASE
		Engine::Debug::CLog::SetLogLevel(ERROR_LEVEL_ERROR);
	#else
		Engine::Debug::CLog::SetLogLevel(ERROR_LEVEL_DEBUG);
	#endif

		// Enable higher performance GetTickCount()
		timeBeginPeriod(1);

		// Seed random number generator.
		srand(GetTickCount());

		// Initialize networking.
		if (!Engine::Platform::SocketSysBegin())
			LOG_CRITICAL("Failed to initialize socket sub-system!");

		// Initialize external libraries.
		LOG_ASSERT_MSG(Engine::External::InitializeLibs(), "Failed to initialize external libraries!");

		// Call platform main!
		SetUnhandledExceptionFilter(ProgramExceptionHandler);

		// We use setjmp to recover from any errors.
		exitcode = setjmp(g_error_recovery_longjmp);
		if (exitcode == 0)
		{
			exitcode = PlatformMain();
		}

		if (exitcode != 0)
			LOG_ERROR("Platform-generic entry point exited unsuccessfully (exitcode=%i).", exitcode);
		
		// Denitialize networking.
		if (!Engine::Platform::SocketSysFinish())
			LOG_ERROR("Failed to initialize socket sub system!");

		// Disable higher performance GetTickCount()
		timeEndPeriod(1);
		
		// Initialize external libraries.
		LOG_ASSERT_MSG(Engine::External::DeinitializeLibs(), "Failed to deinitialize external libraries!");

		LOG_INFO("Shutting down log and disposing of memory ...");

		// Close logging.
		Engine::Debug::CLog::Flush();
		Engine::Debug::CLog::Deinitialize();

	}

    // Goodbye memory, it was nice knowing ye.
	Engine::Scripting::FreeScriptAllocator();
	Engine::Containers::FreeHashTableAllocator();
	Engine::Containers::FreeArrayAllocator();
	Engine::Containers::FreeListAllocator();
	Engine::Containers::FreeStringAllocator();
    Engine::Memory::FreeDefaultAllocator();

    return exitcode;
}
