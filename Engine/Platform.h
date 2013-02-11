///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

// ----------------------------------------------------------------------------
// This file basically does 2 things, first it defines the interface
// that all platforms must expose. Then at the bottom it includes the
// actual definition of the platform we are compiling on.
// ----------------------------------------------------------------------------

#include "Conditionals.h"
#include "CString.h"

// This is a special functions. This is what the user defines instead of main.
// the actual main function is defined platform-specific.
extern s32 PlatformMain();

namespace Engine
{
	namespace Containers
	{
		//class CString;
		template <typename T> class CArray;
		template <typename T> class CList;
	}

    namespace Platform
    {
        // ----------------------------------------------------------------------------
        // Structures!
        // ----------------------------------------------------------------------------
        typedef enum
        {
            FILE_ACCESS_MODE_READ        = 1,
            FILE_ACCESS_MODE_WRITE       = 2,
            FILE_ACCESS_MODE_READ_WRITE  = FILE_ACCESS_MODE_READ | FILE_ACCESS_MODE_WRITE
        } FileAccessMode;

        typedef enum
        {
           FILE_OPEN_MODE_CREATE_ALWAYS     = 1,
           FILE_OPEN_MODE_CREATE_NEW        = 2,
           FILE_OPEN_MODE_OPEN_ALWAYS       = 3,
           FILE_OPEN_MODE_OPEN_EXISTING     = 4,
           FILE_OPEN_MODE_TRUNCATE          = 5,
        } FileOpenMode; 

        typedef enum
        {
            PATH_FLAG_READ_ONLY        = 1,
        } IOPathFlags;

		typedef enum 
		{
			MEMORY_TYPE_PHYSICAL		= 1,
			MEMORY_TYPE_VIRTUAL			= 2,
			MEMORY_TYPE_PAGING			= 3
		} MemoryType;

		typedef enum
		{
			PRIORITY_LOWEST				= -2,
			PRIORITY_LOW				= -1,
			PRIORITY_NORMAL				=  0,
			PRIORITY_HIGH				=  1,
			PRIORITY_HIGHEST			=  2
		} Priority;
		
		typedef enum
		{
			SOCKET_OPTION_NODELAY			= 1,
			SOCKET_OPTION_BROADCAST			= 2,
			SOCKET_OPTION_RECIEVE_BUFFER	= 3,
			SOCKET_OPTION_SEND_BUFFER		= 4,
			SOCKET_OPTION_RECIEVE_TIMEOUT	= 5,
			SOCKET_OPTION_SEND_TIMEOUT		= 6
		} SocketOption;

		typedef enum
		{
			SOCKET_PROTOCOL_TCP				= 1,
			SOCKET_PROTOCOL_UDP				= 2
		} SocketProtocol;

		typedef enum
		{
			SPECIAL_DIR_DESKTOP,
			SPECIAL_DIR_APPDATA,
		} SpecialDirectory;

		typedef enum
		{
			BREAKPOINT_READ		  = 1,
			BREAKPOINT_WRITE	  = 2,
			BREAKPOINT_READWRITE  = 3
		} BreakpointMode; 
		
        // ----------------------------------------------------------------------------
        // These structures hold platform specific data
        // hence they are declared per-platform.
        // ----------------------------------------------------------------------------
        struct FileHandle;
        struct DirHandle;

		struct ThreadHandle;
		struct ThreadLocalDataHandle;
		struct MutexHandle;
		struct SemaphoreHandle;
		struct ConditionVarHandle;

		struct SocketHandle;

		struct BreakpointHandle;

		struct EnvironmentVariable
		{
			Engine::Containers::CString name;
			Engine::Containers::CString value;
		};

		// ----------------------------------------------------------------------------
        // Threading junk.
        // ----------------------------------------------------------------------------
        typedef s32 (*THREAD_ENTRY_FUNCTION)(void* meta);

		// ----------------------------------------------------------------------------
        // Debugging junk.
        // ----------------------------------------------------------------------------
		#define MAX_STACK_FRAMES	32
		#define MAX_SYMBOL_LENGTH	255

		struct StackFrame
		{
			u64 address;
			u8 name[MAX_SYMBOL_LENGTH];
			u8 file[MAX_SYMBOL_LENGTH];
			u32 line;
		};

		struct StackTrace
		{
			u32	frameCount;
			u64	frames[MAX_STACK_FRAMES];
		};

        // ----------------------------------------------------------------------------
        // File IO.
        // ----------------------------------------------------------------------------
        bool    FileOpen  (FileHandle* file, const Engine::Containers::CString& path, FileAccessMode accessMode=FILE_ACCESS_MODE_READ_WRITE, FileOpenMode openMode=FILE_OPEN_MODE_OPEN_ALWAYS);
        void    FileClose (FileHandle* file);
        bool    FileRead  (FileHandle* file, u8* buffer, u64 size);
        bool    FileWrite (FileHandle* file, const u8* buffer, u64 size);
        u64     FilePos   (FileHandle* file);
        u64     FileLen   (FileHandle* file);
        bool    FileSeek  (FileHandle* file, u64 offset);
        bool    FileEOF   (FileHandle* file);
        void    FileFlush (FileHandle* file);

        // ----------------------------------------------------------------------------
        // Directory manipulation.
        // ----------------------------------------------------------------------------
        bool                    DirOpen   (DirHandle* dir, const Engine::Containers::CString& path);
        Engine::Containers::CString  DirNext   (DirHandle* dir);
        void                    DirClose  (DirHandle* dir);

        // ----------------------------------------------------------------------------
        // Path manipulation.
        // ----------------------------------------------------------------------------
        Engine::Containers::CString PathNormalize       (const Engine::Containers::CString& path);
        Engine::Containers::CString PathJoin            (const Engine::Containers::CString& a, const Engine::Containers::CString& b);
        Engine::Containers::CString PathFileName        (const Engine::Containers::CString& path);
        Engine::Containers::CString PathDirectoryName   (const Engine::Containers::CString& path);
        Engine::Containers::CString PathExtension       (const Engine::Containers::CString& path);

        bool                    PathExists          (const Engine::Containers::CString& path);
        bool                    PathIsDirectory     (const Engine::Containers::CString& path);
        bool                    PathIsFile          (const Engine::Containers::CString& path);
        bool                    PathIsRoot          (const Engine::Containers::CString& path);
        bool                    PathIsRelative      (const Engine::Containers::CString& path);
        bool                    PathIsAbsolute      (const Engine::Containers::CString& path);

        Engine::Containers::CString  PathGetWorkingDir   ();
        bool                    PathSetWorkingDir   (const Engine::Containers::CString& b);

        bool                    PathSetTime         (const Engine::Containers::CString& path, u32  access, u32  modified, u32  creation);
        bool                    PathGetTime         (const Engine::Containers::CString& path, u32& access, u32& modified, u32& creation);
        bool                    PathSetFlags        (const Engine::Containers::CString& path, IOPathFlags mode);
        IOPathFlags             PathGetFlags        (const Engine::Containers::CString& path);
        bool                    PathSetMode         (const Engine::Containers::CString& path, u32 mode);
        u32                     PathGetMode         (const Engine::Containers::CString& path);

        Engine::Containers::CArray<Engine::Containers::CString> PathGetRoots();

        u64                     PathGetRootFreeSpace    (const Engine::Containers::CString& path);
        u64                     PathGetRootUsedSpace    (const Engine::Containers::CString& path);
        u64                     PathGetRootTotalSpace   (const Engine::Containers::CString& path);

		Engine::Containers::CString	PathGetSpecialDir		(SpecialDirectory id);

        // ----------------------------------------------------------------------------
        // File manipulation.
        // ----------------------------------------------------------------------------
        bool                    FileDelete          (const Engine::Containers::CString& path);
        bool                    FileRename          (const Engine::Containers::CString& src, const Engine::Containers::CString& dst);
        bool                    FileCopy            (const Engine::Containers::CString& src, const Engine::Containers::CString& dst);
        bool                    FileCreate          (const Engine::Containers::CString& path);
        u64                     FileSize            (const Engine::Containers::CString& path);

        // ----------------------------------------------------------------------------
        // Directory manipulation.
        // ----------------------------------------------------------------------------
        bool                    DirDelete           (const Engine::Containers::CString& path, bool recursive=false);
        bool                    DirCreate           (const Engine::Containers::CString& path, bool recursive=false);
        bool                    DirCopy             (const Engine::Containers::CString& src, const Engine::Containers::CString& dst, bool recursive=false);

        // ----------------------------------------------------------------------------
        // Timing functions.
        // ----------------------------------------------------------------------------
        u64                     GetTicks			();
        f64                     GetMillisecs		();
        u32                     GetUnixTimestamp	();

        // ----------------------------------------------------------------------------
        // System functions.
        // ----------------------------------------------------------------------------
        Engine::Containers::CString                         GetLaunchDirPath        ();
        Engine::Containers::CString                         GetLaunchExePath        ();
        Engine::Containers::CArray<Engine::Containers::CString>  GetLaunchArguments      ();
		Engine::Containers::CArray<EnvironmentVariable>	   GetEnvironmentVars	   ();

		Engine::Containers::CString						   GetPlatformString	   ();
		Engine::Containers::CString					   GetPlatformName();
		Engine::Containers::CString					   GetPlatformShortName();
		u32											   GetProcessorCount	   ();
		
        void										   Abort				   (s32 exitcode=1);
        void										   Wait					   (u64 milliseconds);

		Priority									   GetProcessPriority	   ();
		bool										   SetProcessPriority	   (Priority level);

        //bool                     Execute                 (Engine::Containers::CString command, Engine::Containers::CString args);
        //void                     PollSystem              ();
        //bool                     GetAppTerminated        ();
        //bool                     GetAppSuspended         ();
        //void                     RegisterOnEnd           (ON_EXIT_FUNCTION   functionPtr);
        //void                     RegisterOnSignal        (SignalType signal, ON_SIGNAL_FUNCTION functionPtr);

        // ----------------------------------------------------------------------------
        // Debug functions.
        // ----------------------------------------------------------------------------
		
		// Debug functions are typically called at points where memory management
		// is totally hosed. DO NOT use any allocator based memory to implement them
		// (eg CString, CArray, etc).
		StackFrame				   DebugResolveAddressToStackFrame(u64 address);
		StackTrace				   DebugTraceCallStack			  (u32 offset=0);
		void					   DebugBreak					  ();
		bool					   DebugCreateBreakpoint		  (BreakpointHandle* handle, void* ptr=NULL, u32 size=4, Engine::Platform::BreakpointMode mode=BREAKPOINT_READWRITE);
		void					   DebugDisposeBreakpoint		  (BreakpointHandle* handle);
		//bool					   DebugDump					  (Engine::Containers::CString path);

		// ----------------------------------------------------------------------------
        // Memory functions.
        // ----------------------------------------------------------------------------
        void*                     MemoryAlloc			(u32 size);
        void                      MemoryFree			(void* ptr);

		u64						  GetTotalMemory		(MemoryType type);
		u64						  GetFreeMemory			(MemoryType type);
		u64						  GetUsedMemory			(MemoryType type);

        // ----------------------------------------------------------------------------
        // STDOUT / STDIN
        // ----------------------------------------------------------------------------
        void                    StdOutWrite    (Engine::Containers::CString format, ...);
        void                    StdErrWrite    (Engine::Containers::CString format, ...);
        Engine::Containers::CString  StdInRead      ();
        u8					    StdInReadChar  ();		

        // ----------------------------------------------------------------------------
        // Threads
        // ----------------------------------------------------------------------------
        bool            ThreadSpawn				(ThreadHandle* handle, const Engine::Containers::CString& name, THREAD_ENTRY_FUNCTION entryPtr, void* args);
		void			ThreadDelete			(ThreadHandle* handle);
        void            ThreadPause				(ThreadHandle* handle);
        void            ThreadResume			(ThreadHandle* handle);
        void            ThreadKill				(ThreadHandle* handle, s32 exitcode=1);
        bool            ThreadWait				(ThreadHandle* handle, s32 timeout=0);
		bool			ThreadRunning			(ThreadHandle* handle);
		void			ThreadSetPriority		(ThreadHandle* handle, Priority priority);
		Priority		ThreadGetPriority		(ThreadHandle* handle);
		s32				ThreadGetExitCode		(ThreadHandle* handle);

		bool			ThreadSetAffinity		(ThreadHandle* handle, s32 affinity);
		s32				ThreadGetAffinity		(ThreadHandle* handle);

		void					ThreadSetName	(ThreadHandle* handle, const Engine::Containers::CString& name);
		Engine::Containers::CString	ThreadGetName	(ThreadHandle* handle);

		//ThreadHandle    ThreadMain			();
		//ThreadHandle    ThreadCurrent			();

        bool            MutexCreate				(MutexHandle* handle);
        void            MutexDelete				(MutexHandle* handle);
        bool            MutexTryLock			(MutexHandle* handle);
        void            MutexLock				(MutexHandle* handle);
        void            MutexUnlock				(MutexHandle* handle);
		
        bool            SemaphoreCreate			(SemaphoreHandle* handle);
        void            SemaphoreDelete			(SemaphoreHandle* handle);
        bool            SemaphoreWait			(SemaphoreHandle* handle, s32 timeout=0);
        void            SemaphorePost			(SemaphoreHandle* handle);
		
        bool            ConditionVarCreate     (ConditionVarHandle* handle);
        void            ConditionVarDelete     (ConditionVarHandle* handle);
        void            ConditionVarBroadcast  (ConditionVarHandle* handle);
        void            ConditionVarSignal     (ConditionVarHandle* handle);
        bool            ConditionVarWait       (ConditionVarHandle* handle, MutexHandle* mutexHandle, s32 timeout=0);

        s32             AtomicAdd              (s32* target, s32 value);
        s32             AtomicSwap             (s32* target, s32 value);
        s32             AtomicCompareAndSwap   (s32* target, s32 newValue, s32 compareValue);

        bool            ThreadLocalDataCreate  (ThreadLocalDataHandle* handle);
        void            ThreadLocalDataDelete  (ThreadLocalDataHandle* handle);
        void            ThreadLocalDataSet	   (ThreadLocalDataHandle* handle, void* ptr);
        void*           ThreadLocalDataGet	   (ThreadLocalDataHandle* handle);

        // ----------------------------------------------------------------------------
        // Sockets
        // ----------------------------------------------------------------------------
		bool SocketSysBegin		();
		bool SocketSysFinish	();

		bool SocketOpen			(SocketHandle* socket, SocketProtocol protocol);
		bool SocketClose		(SocketHandle* socket);
		bool SocketBind			(SocketHandle* socket, u32 host, u32 port);
		bool SocketListen		(SocketHandle* socket, s32 backlog=0);
		bool SocketConnect		(SocketHandle* socket, u32 host, u32 port, u32 timeout=0);
		bool SocketConnected	(SocketHandle* socket);
		u32  SocketBytesAvail	(SocketHandle* socket);
		bool SocketAccept		(SocketHandle* socket, SocketHandle* recieveSocket, u32 timeout=0);

		void SocketLocalAddress	(SocketHandle* socket, u32* host, u32* port);
		void SocketRemoteAddress(SocketHandle* socket, u32* host, u32* port);
		
		u32  SocketSend			(SocketHandle* socket, const u8* buffer, u32 length);
		u32  SocketRecieve		(SocketHandle* socket, u8* buffer, u32 recvlength);

		u32  SocketSendTo		(SocketHandle* socket, const u8* buffer, u32 length, u32 host, u32 port);
		u32  SocketRecieveFrom	(SocketHandle* socket, u8* buffer, u32 recvlength, u32* host, u32* port);

		void SocketSetOption	(SocketHandle* handle, SocketOption option, s32 value);
		s32  SocketGetOption	(SocketHandle* handle, SocketOption option);

		// ----------------------------------------------------------------------------
        // Dynamic Name Server functions.
        // ----------------------------------------------------------------------------
		u32								DnsStringToIP	(Engine::Containers::CString ip);
		Engine::Containers::CString			DnsIPToString	(u32 ip);
		Engine::Containers::CArray<u32>		DnsLookupHost	(Engine::Containers::CString host);
		Engine::Containers::CString			DnsLookupIP		(u32 ip);

/*
        // ----------------------------------------------------------------------------
        // Timing functions.
        // ----------------------------------------------------------------------------
//        Engine::Containers::CString FormatTime          (u64 timestamp, Engine::Containers::CString format);


        // ----------------------------------------------------------------------------
        // Graphical User Interface
        // ----------------------------------------------------------------------------
        bool UIMessage      (Engine::Containers::CString message, UIMessageButtons buttons, UIMessageIcon icon);
        bool UIRequestFile  (Engine::Containers::CString title, Engine::Containers::CString filter, Engine::Containers::CString defaultPath="");
        bool UIRequestDir   (Engine::Containers::CString title, Engine::Containers::CString defaultPath="");
        void UIHideMouse    ();
        void UIShowMouse    ();
*/

    }
}

// ----------------------------------------------------------------------------
// Include platform specific implementation.
// ----------------------------------------------------------------------------
#ifdef OS_WIN
	#include "Win32.h"

//#elif OS_LINUX
 //   #include "Linux.cpp"

//#elif OS_MAC
//    #include "MacOS.cpp"

//#elif OS_IOS
//    #include "iOS.cpp"

#else
    #error (Attempt to include platform.h on unknown or unsupported platform!)

#endif
