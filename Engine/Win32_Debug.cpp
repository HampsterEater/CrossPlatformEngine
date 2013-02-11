///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

namespace Engine
{
    namespace Platform
    {

		StackFrame DebugResolveAddressToStackFrame(u64 address)
		{
			DWORD64 displacement = 0;

			char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
			PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

			pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			pSymbol->MaxNameLen = MAX_SYM_NAME;

			StackFrame frame;
			frame.address = address;
			frame.line = 0;

			sprintf_s(frame.name, MAX_SYMBOL_LENGTH, "0x%.16llx", address);

			EnterCriticalSection(&g_dbghelp_critical_section);

			if (SymFromAddr(GetCurrentProcess(), address, &displacement, pSymbol) == TRUE)
			{
				strcpy_s(frame.name, MAX_SYMBOL_LENGTH, pSymbol->Name);

				IMAGEHLP_LINE64 line;
				DWORD lineDisplacement;
				line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
				if (SymGetLineFromAddr64(GetCurrentProcess(), address, &lineDisplacement, &line))
				{
					frame.line = line.LineNumber;
					strcpy_s(frame.file, 255, line.FileName);
				}
				else
				{
					frame.line = 0;
					strcpy_s(frame.file,   255, "<unknown>");
				}
			}
			else
			{
				s32 err = GetLastError();
				strcpy_s(frame.file,   255, "<unknown>");
				frame.line = 0;
			}
			
			LeaveCriticalSection(&g_dbghelp_critical_section);

			return frame;
		}

		StackTrace DebugTraceCallStack(u32 offset)
		{
			StackTrace trace;
			ZeroMemory(&trace, sizeof(StackTrace));
					
			void* frames[MAX_STACK_FRAMES];
			ZeroMemory(&frames, sizeof(frames));

			#ifdef MEMORT_TRACK_CALL_STACK_FAST

				trace.frameCount = CaptureStackBackTrace(1 + offset, MAX_STACK_FRAMES, frames, NULL);
				for (u16 i = 0; i < trace.frameCount; i++)
				{
					trace.frames[i] = (u64)frames[i];
				}

			#else

				trace.frameCount = Win32CaptureStackBackTraceSlow(1 + offset, MAX_STACK_FRAMES, frames, NULL, NULL);
				for (u16 i = 0; i < trace.frameCount; i++)
				{
					#ifdef ARCH_BIT_64
						trace.frames[i] = (u64)frames[i];
					#else
						trace.frames[i] = (u64)frames[i];
					#endif
				}

			#endif

			return trace;
		}

		void DebugBreak()
		{
			__debugbreak();
		}

		bool DebugCreateBreakpoint(BreakpointHandle* handle, void* ptr, u32 size, Engine::Platform::BreakpointMode mode)
		{
			LOG_ASSERT(handle->index == -1);
			LOG_ASSERT(size == 1 || size == 2 || size == 4);

			CONTEXT context;
			HANDLE	currentThread = GetCurrentThread();

			// Reduce size by one.
			size--;

			// Grab the threads context.
			context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
			if (!GetThreadContext(currentThread, &context))
				LOG_ASSERT(false);

			// Find a hardware register.
			u32 hardwareIndex = 0;
			for (hardwareIndex = 0; hardwareIndex < 4; hardwareIndex++)
			{
				if ((context.Dr7 & (1 << (hardwareIndex*2))) == 0)
					break;
			}
			if (hardwareIndex >= 4)
				return false;

			// Set the address.
			switch (hardwareIndex)
			{
				case 0: context.Dr0 = (DWORD)ptr; break;
				case 1: context.Dr1 = (DWORD)ptr; break;
				case 2: context.Dr2 = (DWORD)ptr; break;
				case 3: context.Dr3 = (DWORD)ptr; break;
			}

			u32 when = 0;
			switch (mode)
			{
				case BREAKPOINT_READ:		when = 3; break;
				case BREAKPOINT_WRITE:		when = 1; break;	
				case BREAKPOINT_READWRITE:	when = 3; break;
				default:					LOG_ASSERT(false);
			}

			// Activate the hardware breakpoint.
			u32 dr7 = context.Dr7;
			Engine::Math::SetBits(dr7, 16 + (hardwareIndex * 4), 2, when);
			Engine::Math::SetBits(dr7, 18 + (hardwareIndex * 4), 2, size);
			Engine::Math::SetBits(dr7, (hardwareIndex * 2),		 1, 1);
			context.Dr7 = dr7;

			// Activate the new registers.
			LOG_ASSERT(SetThreadContext(currentThread, &context));

			// Assign the breakpoint.
			handle->index = hardwareIndex;

			return true;
		}

		void DebugDisposeBreakpoint(BreakpointHandle* handle)
		{
			LOG_ASSERT(handle->index != -1);

			CONTEXT context;
			HANDLE	currentThread = GetCurrentThread();

			// Grab the threads context.
			context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
			if (!GetThreadContext(currentThread, &context))
				LOG_ASSERT(false);

			// Zero out register.
			u32 dr7 = context.Dr7;
			Engine::Math::SetBits(dr7, (handle->index * 2), 1, 0);
			context.Dr7 = dr7;

			// Apply new context.
			LOG_ASSERT(SetThreadContext(currentThread, &context));

			// And we are done.
			handle->index = -1;
		}

	}
}