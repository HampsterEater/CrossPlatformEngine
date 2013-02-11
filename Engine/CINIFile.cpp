///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "CINIFile.h"
#include "CStream.h"
#include "CLog.h"
#include "CArray.h"

using namespace Engine::FileSystem::Config;

CINIFile::CINIFile()
{

}

CINIFile::CINIFile(Engine::FileSystem::Streams::CStream* stream)
{
	Load(stream);
}

void CINIFile::Load(Engine::FileSystem::Streams::CStream* stream)
{
	Deserialize(stream->ReadToEnd());
}

void CINIFile::Save(Engine::FileSystem::Streams::CStream* stream)
{
	stream->WriteText(Serialize());
}

void CINIFile::Clear()
{
	_currentSection = "";
	_values.Clear();
}

Engine::Containers::CString CINIFile::ReplaceValueRefs(const Engine::Containers::CString& str)
{
	Engine::Containers::CString cleanstr = str;

	for (u32 i = 0; i < _values.Size(); i++)
	{
		CINIFileValue v = _values[i];
		cleanstr = cleanstr.Replace("{"+v.section+"\\"+v.name+"}", v.value);

		if (v.section == _currentSection)
			cleanstr = cleanstr.Replace("{"+v.name+"}", v.value);
	}

	return cleanstr;
}

Engine::Containers::CString CINIFile::ReplaceEnvVarRefs(const Engine::Containers::CString& str)
{	
	Engine::Containers::CString cleanstr = str;

	Engine::Containers::CArray<Engine::Platform::EnvironmentVariable> vars = Engine::Platform::GetEnvironmentVars();
	for (u32 i = 0; i < vars.Size(); i++)
	{
		cleanstr = cleanstr.Replace("{%"+vars[i].name+"%}", vars[i].value);
	}

	return cleanstr;
}

Engine::Containers::CString CINIFile::ReplaceSpecialDirRefs(const Engine::Containers::CString& str)
{	
	Engine::Containers::CString cleanstr = str;

	Engine::Containers::CString replacestr = Engine::Platform::PathGetSpecialDir(Engine::Platform::SPECIAL_DIR_APPDATA);
	cleanstr = cleanstr.Replace("{%SPECIAL_DIR_APPDATA%}", replacestr);

	replacestr = Engine::Platform::PathGetSpecialDir(Engine::Platform::SPECIAL_DIR_DESKTOP);
	cleanstr = cleanstr.Replace("{%SPECIAL_DIR_DESKTOP%}", replacestr);

	return cleanstr;
}

void CINIFile::Deserialize(const Engine::Containers::CString& str)
{
	Engine::Containers::CString data = str;

	// Swap static values.
	data = ReplaceEnvVarRefs(data);
	data = ReplaceSpecialDirRefs(data);

	// Clear out old values.
	Clear();

	// Parse through line by line.
	s32 oldIndex = -1;
	s32 index	 = data.IndexOf('\n', 0);
	while (index >= 0)
	{
		Engine::Containers::CString line = data.SubString(oldIndex + 1, (index - oldIndex) - 1);
		DeserializeLine(line);

		// At the end of the data?
		if (index + 1 >= (s32)data.Length())
			break;

		oldIndex = index;
		index	 = data.IndexOf('\n', oldIndex + 1);
	}

	if (oldIndex < (s32)str.Length())
	{
		Engine::Containers::CString line = data.SubString(oldIndex + 1);
		DeserializeLine(line);
	}
}

void CINIFile::DeserializeLine(const Engine::Containers::CString& str)
{
	Engine::Containers::CString data = str;
	data = data.Trim();

	// Strip off any comments.
	s32 index = data.IndexOf('#');
	//s32 index2 = data.IndexOf(';');
	//if ((index2 < index && index2 >= 0) || index == -1)
	//{
	//	index = index2;
	//}
	if (index >= 0)
	{
		data = data.SubString(0, index);
	}

	// Do we still have any data?
	if (data == "")
	{
		return;
	}

	// Section declaration.
	if (str[0] == '[')
	{
		index = data.IndexOf(']');
		if (index >= 0)
		{
			data = data.SubString(1, index - 1).Trim();
			DeserializeSection(data);
		}
	}

	// Value declaration.
	else
	{
		s32 index = data.IndexOf('=', 0);
		if (index > 0)
		{
			Engine::Containers::CString name = data.SubString(0, index);
			Engine::Containers::CString value = data.SubString(index + 1);
			DeserializeValue(name, value);
		}
		else
		{
			// wtf is this declaring then? Lets ignore.
			LOG_ERROR("Encountered invalid value syntax: %s", data);
		}
	}
}

void CINIFile::DeserializeSection(const Engine::Containers::CString& str)
{
	Engine::Containers::CString cleanName = str;

	if (cleanName.IndexOf('{') >= 0)
	{
		cleanName = ReplaceValueRefs(cleanName);
	}

	_currentSection = cleanName;
}

void CINIFile::DeserializeValue(const Engine::Containers::CString& name, const Engine::Containers::CString& value)
{
	Engine::Containers::CString cleanName = name;
	Engine::Containers::CString cleanValue = value;

	// Strip off the quotation marks.
	if (cleanValue.Length() > 2 && cleanValue[0] == '"' && cleanValue[cleanValue.Length() - 1] == '"')
		cleanValue = cleanValue.SubString(1, cleanValue.Length() - 2);
	
	if (cleanName.IndexOf('{') >= 0)
	{
		cleanName = ReplaceValueRefs(cleanName);
	}
	if (cleanValue.IndexOf('{') >= 0)
	{
		cleanValue = ReplaceValueRefs(cleanValue);
	}

	Set(_currentSection, cleanName, cleanValue);
}

Engine::Containers::CString CINIFile::Serialize()
{
	Engine::Containers::CString data = "";

	// Go through values by section and deserialize.
	Engine::Containers::CArray<Engine::Containers::CString> sections;
	for (u32 i = 0; i < _values.Size(); i++)
	{
		CINIFileValue			secVal			= _values[i];
		Engine::Containers::CString	lowerSection	= secVal.section.ToLower();

		if (!sections.Contains(lowerSection))
		{
			sections.AddToEnd(lowerSection);

			data += "[" + secVal.section + "]\n";

			// Serialize values.
			for (u32 i = 0; i < _values.Size(); i++)
			{
				CINIFileValue val = _values[i];
				if (val.section.ToLower() == lowerSection)
				{
					data += val.name + "=" + val.value + "\n";
				}
			}

			data += "\n\n";
		}
	}

	return data;
}

Engine::Containers::CString CINIFile::Get(const Engine::Containers::CString& section, const Engine::Containers::CString& name, const Engine::Containers::CString& defaultValue)
{
	Engine::Containers::CString value = "";
	bool found = false;

	Engine::Containers::CString lowerName = name;
	lowerName = lowerName.ToLower();

	Engine::Containers::CString lowerSection = section;
	lowerSection = lowerSection.ToLower();

	for (u32 i = 0; i < _values.Size(); i++)
	{
		CINIFileValue val = _values[i];
		if (val.name.ToLower() == lowerName && val.section.ToLower() == lowerSection)
		{
			found = true;
			value = val.value;
			break;
		}
	}

	if (found == false)
	{
		value = defaultValue;
	}

	return value;
}

void CINIFile::Set(const Engine::Containers::CString& section, const Engine::Containers::CString& name, const Engine::Containers::CString& data)
{
	Engine::Containers::CString lowerName = name;
	lowerName = lowerName.ToLower();

	Engine::Containers::CString lowerSection = section;
	lowerSection = lowerSection.ToLower();

	for (u32 i = 0; i < _values.Size(); i++)
	{
		CINIFileValue val = _values[i];
		if (val.name.ToLower() == lowerName && val.section.ToLower() == lowerSection)
		{
			val.value = data;
			return;
		}
	}

	CINIFileValue storedValue;
	storedValue.name = name;
	storedValue.value = data;
	storedValue.section = section;
	_values.AddToEnd(storedValue);
}
