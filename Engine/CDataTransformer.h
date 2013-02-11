///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "CArray.h"

namespace Engine
{
	namespace Containers
	{
		class CString;
	}
	namespace FileSystem
	{
		namespace Streams
		{
			class CStream;
		}
	}
	namespace Data
	{

		//
		// Contains the output of a data transformation operation.
		// Provides a selection of different methods to retrieve
		// the data in (string/hex form, data buffer, etc)
		//
		class CTransformedData
		{
			private:
					
				u8* _buffer;
				u32 _length;

			public:		
					
				~CTransformedData       ();
				CTransformedData        ();
				CTransformedData        (const u8* buffer, u32 length);
				CTransformedData        (const CTransformedData& v);

				// Operators, delicious operators!
	            const u8				operator[]  (u32 index) const;
		        const u8				operator[]  (s32 index) const;
				void					operator=   (const CTransformedData &str);
	
				friend bool				operator==  (const CTransformedData& a, const CTransformedData& b);
				friend bool				operator!=  (const CTransformedData& a, const CTransformedData& b);

				// General properties.
				u32						Size		();	// Size in bytes of digest.
				const u8*				GetBuffer	();	// Copies the digest into a buffer.
				Engine::Containers::CString	GetString	();	// Returns a string representation of the digest.
				Engine::Containers::CString	GetHexString();	// Returns a hex string representation of the digest.

		};

		//
		// This is the base class for data transformer algorithems (MD5, CRC, Compression, etc).
		// it provides a common interface that all should adhear too. 
		//
		// It should be used similar to the following example;
		//
		//	CMD5DataTransformer d;
		//	d.Initialize();
		//	d.AddBuffer(buffer, size);
		//	CTransformedData output = d.Calculate(output, outputsize);
		//	
		// There are also some helper functions too, to speed things up.
		//
		//	CMD5DataTransformer d;
		//	CTransformedData output = CDataTransformer.CalculateBuffer(d, buffer, size);
		//	CTransformedData output = CDataTransformer.CalculateFile(d, buffer, size);
		//
		class CDataTransformer 
		{
			private:
					
			public:

				virtual void Initialize				()								= 0;
				virtual void AddBuffer				(const u8* buffer, u32 size)			= 0;
				virtual CTransformedData Calculate	()								= 0;

				void AddStream						(Engine::FileSystem::Streams::CStream* stream);
				
				// Helper functions.
				static CTransformedData CalculateBuffer		(CDataTransformer* trans, const u8* buffer, u32 size);
				static CTransformedData CalculateStream		(CDataTransformer* trans, Engine::FileSystem::Streams::CStream* stream);
			
		};

	}
}