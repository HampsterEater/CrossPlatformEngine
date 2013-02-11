///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

// ----------------------------------------------------------------------------
// Bits and pieces of this source code are based off ideas
// or code found in other open source games. A lot of the mathmatics code
// especially has been taken from the idLib. Many thanks to the authors :3
// ----------------------------------------------------------------------------

// Conditional compliation defines.
#include "Conditionals.h"

// Debug stuff.
#include "CLog.h"
#include "CBreakpoint.h"				

// Container types..
#include "CString.h"
#include "CArray.h"
#include "CList.h"
#include "CHashTable.h"						

// Platform-Specific Code
#include "Platform.h"

// Memory stuff.
#include "Memory.h"
#include "CAllocator.h"
#include "CFrameAllocator.h"
#include "CHeapAllocator.h"
#include "CProxyAllocator.h"

// Threading stuff.
#include "CThread.h"
#include "CThreadLocalData.h"
#include "CSemaphore.h"
#include "CMutex.h"
#include "CConditionVariable.h"

// Networking stuff.
#include "CSocket.h"
#include "DNS.h"					

// Mathmatics.
#include "CMatrix2.h"				
#include "CMatrix3.h"					
#include "CMatrix4.h"				
#include "CVector2.h"				
#include "CVector3.h"				
#include "CVector4.h"				
#include "CQuaternion.h"			
#include "CRandom.h"				
#include "CLine.h"					
#include "CCircle.h"				
#include "CRect.h"					
#include "Math.h"					

// Cryptography.
#include "CDataTransformer.h"
#include "CBase64Encoder.h"
#include "CBase64Decoder.h"
#include "CZipCompressor.h"
#include "CZipDecompressor.h"
#include "CCRC32Encoder.h"
#include "CMD5Encoder.h"
#include "CPublicKeyEncryptor.h"	
#include "CPublicKeyDecryptor.h"	

// File System
#include "CINIFile.h"
#include "CXMLFile.h"				
#include "CFileSystem.h"
#include "CFilePackage.h"
#include "CFilePackageStream.h"
#include "CStream.h"
#include "CSocketStream.h"
#include "CMemoryStream.h"
#include "CHTTPStream.h"
#include "CFileStream.h"

// Core system.
#include "CGameEngine.h"			// Unfinished
#include "CTaskManager.h"			
#include "CTask.h"					
//#include "CCmdLineParser.h"			// Unfinished

// Scripting system.
#include "CScriptCompileContext.h"		
#include "CScriptParser.h"				
#include "CScriptLexer.h"				
#include "CScriptGenerator.h"				
#include "CScriptManager.h"				
#include "CScriptVirtualMachine.h"		

// Misc stuff.
#include "Endianness.h"
