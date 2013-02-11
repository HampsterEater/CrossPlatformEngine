///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

namespace Engine
{
    namespace Platform
    {

        u64 GetTicks()
        {
             return GetTickCount();
        }
		
        f64 GetMillisecs()
		{
			static LARGE_INTEGER freq;			
			static LARGE_INTEGER start;
			static u32 state = 0; // 0=not initialized, -1=not supported, 1=supported

			if (state == 0)
			{
				if (QueryPerformanceFrequency(&freq) == 0)
				{
					state = -1;
				}
				else
				{
					QueryPerformanceCounter(&start);
					state = 1;
				}
			}

			// Not supported, use tick count.
			if (state == -1)
			{
				return GetTickCount();
			}
			else
			{
				LARGE_INTEGER tickCount;
				QueryPerformanceCounter(&tickCount);

				f64 f = (f64)freq.QuadPart / 1000.0;

				return f64(tickCount.QuadPart - start.QuadPart) / f;
			}		
		}

        u32 GetUnixTimestamp()
        {
            SYSTEMTIME sysTime;
            FILETIME fileTime;

            // Get system time!
            GetSystemTime(&sysTime);

            // Turn it into file time.
            SystemTimeToFileTime(&sysTime, &fileTime);

            // Turn file time into unix timestamp and return!
            return FileTimeToUnixTime(fileTime);
        }

        Engine::Containers::CString GetLaunchDirPath()
        {
			return Engine::Platform::PathDirectoryName(g_platform_argv[0]);
//            return g_platform_exe_dir;
        }

        Engine::Containers::CString GetLaunchExePath()
        {
			return g_platform_argv[0];
//            return g_platform_exe;
        }

        Engine::Containers::CArray<Engine::Containers::CString> GetLaunchArguments()
        {
			Engine::Containers::CArray<Engine::Containers::CString> args;

			for (s32 i = 1; i < g_platform_argc; i++)
				args.AddToEnd(g_platform_argv[i]);

			return args;
//			return g_platform_args;
        }

		Engine::Containers::CArray<EnvironmentVariable> GetEnvironmentVars()
		{
			Engine::Containers::CArray<EnvironmentVariable> vars;

			LPTCH str = GetEnvironmentStrings();

			// Find the end of the strings.
			Engine::Containers::CString newvar = "";
			u32 offset = 0;
			while (true)
			{
				u8 chr = str[offset];
				if (chr == '\0')
				{
					// Should be in the format of name=value.
					u32 idx = newvar.IndexOf('=');
					if (idx > 0) // Ignore envvars that start with an = sign 
								 // (some wierd variables we don't care are reported at the start like this).
					{
						Engine::Platform::EnvironmentVariable var;
						var.name  = newvar.SubString(0, idx);
						var.value = newvar.SubString(idx + 1);
						vars.AddToEnd(var);
					}

					// End of values?
					if (str[offset + 1] == '\0')
					{
						break;
					}

					newvar = "";
				}
				else
				{
					newvar += chr;
				}

				offset++;
			}
			
			FreeEnvironmentStrings(str);

			return vars;
		}

		Engine::Containers::CString	GetPlatformName()
		{
			return "windows";
		}

		Engine::Containers::CString	GetPlatformShortName()
		{
			return "windows";
		}

		Engine::Containers::CString GetPlatformString()
		{
			Engine::Containers::CString versionString = "Unknown Windows Version";
		
			OSVERSIONINFOEX versionInfo;
			versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

			if (GetVersionEx((OSVERSIONINFO*)&versionInfo))
			{
				if (versionInfo.dwPlatformId == 1)
				{
					switch (versionInfo.dwMinorVersion)
					{
						case 0:		versionString = "Windows 95";			break;
						case 10:	versionString = "Windows 98";			break;
						case 90:	versionString = "Windows Millennium";	break;
					}
				}
				else if (versionInfo.dwPlatformId == 2)
				{
					switch (versionInfo.dwMajorVersion)
					{
						case 3:		versionString = "Windows NT 3.51";		break;
						case 4:		versionString = "Windows NT 4.0";		break;
						case 5:		
							switch (versionInfo.dwMinorVersion)
							{
								case 0:		versionString = "Windows 2000";			break;
								case 1:		versionString = "Windows XP";			break;
								case 2:		versionString = "Windows Server 2003";	break;
							}
							break;
						case 6:	
							switch (versionInfo.dwMinorVersion)
							{
								case 0:		
									if(versionInfo.wProductType == VER_NT_WORKSTATION)
										versionString = "Windows Vista";	
									else
										versionString = "Windows Server 2008";		
									break;
								case 1:	
									if(versionInfo.wProductType == VER_NT_WORKSTATION)
										versionString = "Windows 7";	
									else
										versionString = "Windows Server 2008 R2";		
									break;
								case 2:
									versionString = "Windows 8";
							}
							break;
					}
				}

				versionString += " (Build ";		
				versionString += (u32)versionInfo.dwBuildNumber;
				versionString += ") ";
				versionString += versionInfo.szCSDVersion;
				versionString += " - ";
		
				DWORD dwType;
				GetProductInfo(versionInfo.dwMajorVersion, versionInfo.dwMinorVersion, 0, 0, &dwType);
				switch( dwType )
				{
					case PRODUCT_ULTIMATE:						versionString += "Ultimate Edition";								break;
					case PRODUCT_PROFESSIONAL:					versionString += "Professional";									break;
					case PRODUCT_HOME_PREMIUM:					versionString += "Home Premium Edition";							break;
					case PRODUCT_HOME_BASIC:					versionString += "Home Basic Edition";								break;
					case PRODUCT_ENTERPRISE:					versionString += "Enterprise Edition";								break;
					case PRODUCT_BUSINESS:						versionString += "Business Edition";								break;
					case PRODUCT_STARTER:						versionString += "Starter Edition";									break;
					case PRODUCT_CLUSTER_SERVER:				versionString += "Cluster Server Edition";							break;
					case PRODUCT_DATACENTER_SERVER:				versionString += "Datacenter Edition";								break;
					case PRODUCT_DATACENTER_SERVER_CORE:		versionString += "Datacenter Edition (core installation)";			break;
					case PRODUCT_ENTERPRISE_SERVER:				versionString += "Enterprise Edition";								break;
					case PRODUCT_ENTERPRISE_SERVER_CORE:		versionString += "Enterprise Edition (core installation)";			break;
					case PRODUCT_ENTERPRISE_SERVER_IA64:		versionString += "Enterprise Edition for Itanium-based Systems";	break;
					case PRODUCT_SMALLBUSINESS_SERVER:			versionString += "Small Business Server";							break;
					case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:	versionString += "Small Business Server Premium Edition";			break;
					case PRODUCT_STANDARD_SERVER:				versionString += "Standard Edition";								break;
					case PRODUCT_STANDARD_SERVER_CORE:			versionString += "Standard Edition (core installation)";			break;
					case PRODUCT_WEB_SERVER:					versionString += "Web Server Edition";								break;
				}

				SYSTEM_INFO si;
				ZeroMemory(&si, sizeof(SYSTEM_INFO));
				GetNativeSystemInfo(&si);

				 if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					 versionString += ", 64-bit"; 
				 else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
					 versionString += ", 32-bit"; 
			}

			return versionString;
		}

		u32	GetProcessorCount()
		{
			SYSTEM_INFO si;
			ZeroMemory(&si, sizeof(SYSTEM_INFO));
			GetNativeSystemInfo(&si);

			return si.dwNumberOfProcessors;
		}

        void Abort(s32 exitcode)
        {
			//throw "Game Aborted!";
            RaiseException(exitcode, 0, 0, NULL);
        }
		
        void Wait(u64 milliseconds)
		{
			Sleep((DWORD)milliseconds);
		}
	
		Priority GetProcessPriority()
		{
			DWORD win32level = GetPriorityClass(GetCurrentProcess());
			switch (win32level)
			{
				case IDLE_PRIORITY_CLASS:			return PRIORITY_LOWEST;
				case BELOW_NORMAL_PRIORITY_CLASS:	return PRIORITY_LOW;
				case NORMAL_PRIORITY_CLASS:			return PRIORITY_NORMAL;
				case ABOVE_NORMAL_PRIORITY_CLASS:	return PRIORITY_HIGH;	
				case REALTIME_PRIORITY_CLASS:		return PRIORITY_HIGHEST;
			}
			return PRIORITY_NORMAL;
		}

		bool SetProcessPriority(Priority level)
		{
			DWORD win32level = 0;
			switch (level)
			{
				case PRIORITY_LOWEST:	win32level = IDLE_PRIORITY_CLASS;			break;
				case PRIORITY_LOW:		win32level = BELOW_NORMAL_PRIORITY_CLASS;	break;
				case PRIORITY_NORMAL:	win32level = NORMAL_PRIORITY_CLASS;			break;
				case PRIORITY_HIGH:		win32level = ABOVE_NORMAL_PRIORITY_CLASS;	break;
				case PRIORITY_HIGHEST:	win32level = REALTIME_PRIORITY_CLASS;		break;
			}
			if (!SetPriorityClass(GetCurrentProcess(), win32level))
			{
                DWORD ec = GetLastError();
                LOG_ERROR(S("SetPriorityClass (%i) failed, GetLastError()=%i (%s)").Format(win32level, ec, FormatSystemError(ec).c_str()));
				return false;
			}
			return true;
		}

        void* MemoryAlloc(u32 size)
        {
            // NEVER ATTEMPT TO LOG FAILURES
            // The reason for this is because the MemoryAlloc/Free functions are primarily used
            // for initializing the original memory space. Logging requires memory, which prior
            // to this call succeeding, dosen't exist D:
			return LocalAlloc(LMEM_FIXED, size);
//            return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
        }

        void MemoryFree(void* ptr)
        {
            // Read note in MemoryAlloc before touching this function.
            LocalFree(ptr);
			//VirtualFree(ptr, MEM_RELEASE, 0);
        }

		u64	GetTotalMemory(MemoryType type)
		{
			MEMORYSTATUSEX statex;
			statex.dwLength = sizeof(statex);

			GlobalMemoryStatusEx(&statex);

			switch (type)
			{
				case MEMORY_TYPE_PHYSICAL:	return statex.ullTotalPhys;
				case MEMORY_TYPE_VIRTUAL:	return statex.ullTotalVirtual;
				case MEMORY_TYPE_PAGING:	return statex.ullTotalPageFile;
			}

			return 0;
		}

		u64	GetFreeMemory(MemoryType type)
		{
			MEMORYSTATUSEX statex;
			statex.dwLength = sizeof(statex);

			GlobalMemoryStatusEx(&statex);

			switch (type)
			{
				case MEMORY_TYPE_PHYSICAL:	return statex.ullAvailPhys;
				case MEMORY_TYPE_VIRTUAL:	return statex.ullAvailVirtual;
				case MEMORY_TYPE_PAGING:	return statex.ullAvailPageFile;
			}

			return 0;
		}

		u64	GetUsedMemory(MemoryType type)
		{
			return GetTotalMemory(type) - GetFreeMemory(type);
		}

        void StdOutWrite(Engine::Containers::CString format, ...)
		{
			va_list ap;
			va_start(ap, format);
			format = Engine::Containers::CString::FormatStringVA(format, ap);
			va_end(ap);

			fprintf(stdout, format.c_str());
		}

        void StdErrWrite(Engine::Containers::CString format, ...)
		{
			va_list ap;
			va_start(ap, format);
			format = Engine::Containers::CString::FormatStringVA(format, ap);
			va_end(ap);

			fprintf(stderr, format.c_str());
		}

        Engine::Containers::CString StdInRead()
		{
			Engine::Containers::CString msg;

			while (true)
			{
				u8 c = getchar();
				if (c == '\n')
					msg += c;
			}

			return msg;
		}

        u8 StdInReadChar()
		{
			return getchar();
		}


	}
}