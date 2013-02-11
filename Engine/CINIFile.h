///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"
#include "CArray.h"

// 
//	The INI file format used here is designed to encompass all the different 
//	quirks of the different INI file formats.
//
//	# allow comments, full or part lines.
//
//  Sections are defined using;
//	[sections] 
//
//	Values are defined like this (you can put quotes around the values, but they will not do anything useful - they won't escape #'s or anything);
//	name=value 
//
//	You can reference other values in other values, using the following syntax;
//
//	name1=this_is_a
//	name2={name1}_test
//
//	You can not forward reference values though. If the value is in another section, 
//	then you need to prefix it with a slash.
//
//	[sectiona]
//	name1=this_is_a
//	[sectionb]
//	name2={sectiona\name1}_test
//	
//	You can also reference environment variables like so;
//
//	pathvars={%PATH%}
//
//	Special directories can also be referenced similar to environment variables
//	(they are essentially pusedo-envvars).
//
//	profilepath={%SPECIAL_DIR_DESKTOP%}\.MyGame\Profile.dat
//
//	Currently supported are;
//
//		SPECIAL_DIR_APPDATA
//		SPECIAL_DIR_DESKTOP
//		

namespace Engine
{
    namespace FileSystem
    {
		namespace Streams
		{
			class CStream;
		}
		namespace Config
		{

			struct CINIFileValue
			{
				Engine::Containers::CString name;
				Engine::Containers::CString section;
				Engine::Containers::CString value;

				CINIFileValue(const CINIFileValue& v)
				{
					name = v.name;
					section = v.section;
					value = v.value;
				}

				CINIFileValue()
				{

				}
			};

			class CINIFile
			{
				private:
			
					Engine::Containers::CString				 _currentSection;
					Engine::Containers::CArray<CINIFileValue> _values;

					inline void						DeserializeLine			(const Engine::Containers::CString& str);
					inline void						DeserializeSection		(const Engine::Containers::CString& str);
					inline void						DeserializeValue		(const Engine::Containers::CString& name, const Engine::Containers::CString& value);

					inline Engine::Containers::CString	ReplaceValueRefs		(const Engine::Containers::CString& str);
					inline Engine::Containers::CString	ReplaceEnvVarRefs		(const Engine::Containers::CString& str);
					inline Engine::Containers::CString	ReplaceSpecialDirRefs	(const Engine::Containers::CString& str);

				public:

					CINIFile								();
					CINIFile								(Engine::FileSystem::Streams::CStream* stream);

					void					Load			(Engine::FileSystem::Streams::CStream* stream);
					void					Save			(Engine::FileSystem::Streams::CStream* stream);
					void					Clear			();

					void					Deserialize		(const Engine::Containers::CString& str);
					Engine::Containers::CString	Serialize		();

					
					Engine::Containers::CString 	Get				(const Engine::Containers::CString& segment, const Engine::Containers::CString& name, const Engine::Containers::CString& defaultValue="");
					void					Set				(const Engine::Containers::CString& segment, const Engine::Containers::CString& name, const Engine::Containers::CString& data);


			};

		}
	}
}