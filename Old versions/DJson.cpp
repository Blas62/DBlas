//*************************************************************************************************
// DJson
//*************************************************************************************************
// include
#include "pch.h"
#include "DJson.h"
//-------------------------------------------------------------------------------------------------
// namespace
using namespace jplibrary::json;
using namespace std;
//-------------------------------------------------------------------------------------------------
void DJson::clear()
{
	Object = nlohmann::json::object();
}
//-------------------------------------------------------------------------------------------------
bool DJson::get(string Key, double & Value)
{
	if (!Object[Key].empty())
	{
		Value = Object[Key];
		return true;
	}
	else return false;
}
//-------------------------------------------------------------------------------------------------
bool DJson::get(string Key, string & Value)
{
	if (!Object[Key].empty())
	{
		string Buffer = Object[Key];
		Value = Buffer;
		return true;
	}
	else return false;
}
//-------------------------------------------------------------------------------------------------
bool DJson::get(string Key, unsigned int & Value)
{
	if (!Object[Key].empty())
	{
		Value = Object[Key];
		return true;
	}
	else return false;
}
//-------------------------------------------------------------------------------------------------
bool DJson::get(string Key, uint16_t & Value)
{
	if (!Object[Key].empty())
	{
		Value = Object[Key];
		return true;
	}
	else return false;
}
//-------------------------------------------------------------------------------------------------
bool DJson::get(string Key, int & Value)
{
	if (!Object[Key].empty())
	{
		Value = Object[Key];
		return true;
	}
	else return false;
}
//-------------------------------------------------------------------------------------------------
string DJson::get_object()
{
	string StringObject;
	StringObject = Object.dump();
	return StringObject;
}
//-------------------------------------------------------------------------------------------------
string DJson::get_object(int Indent)
{
	string StringObject;
	StringObject = Object.dump(Indent);
	return StringObject;
}
//-------------------------------------------------------------------------------------------------
string DJson::get_object(string Key)
{
	if (!Object[Key].empty())
	{
		auto Buffer = Object[Key];
		string StringObject;
		StringObject = Buffer.dump();
		return StringObject;
	}
	else return "";
}
//-------------------------------------------------------------------------------------------------
bool DJson::is_empty()
{
	return Object.empty();
}
//-------------------------------------------------------------------------------------------------
bool DJson::is_empty(string Key)
{
	return Object[Key].empty();
}
//-------------------------------------------------------------------------------------------------
bool DJson::load(string FileName)
{
	bool Success = true;
	ifstream File;
	File.open(FileName.c_str());
	if (File.is_open())
	{
		File >> Object;
		File.close();
	}
	else
	{
		set_error(errno, strerror(errno));
		Success = false;
	}
	return Success;
}
//-------------------------------------------------------------------------------------------------
bool DJson::save(string FileName)
{
	bool Success = true;
	ofstream File;
	File.open(FileName.c_str());
	if (File.is_open())
	{
		File << Object;
		File.close();
	}
	else
	{
		set_error(errno, strerror(errno));
		Success = false;
	}
	return Success;
}
//-------------------------------------------------------------------------------------------------
void DJson::set(string Value)
{
	Object = nlohmann::json::parse(Value);
}
//-------------------------------------------------------------------------------------------------
void DJson::set(string Key, bool Value)
{
	Object[Key] = Value;
}
//-------------------------------------------------------------------------------------------------
void DJson::set(string Key, double Value)
{
	Object[Key] = Value;
}
//-------------------------------------------------------------------------------------------------
void DJson::set(string Key, int Value)
{
	Object[Key] = Value;
}
//-------------------------------------------------------------------------------------------------
void DJson::set(string Key, unsigned int Value)
{
	Object[Key] = Value;
}
//-------------------------------------------------------------------------------------------------
void DJson::set(string Key, string Value)
{
	Object[Key] = Value;
}
//-------------------------------------------------------------------------------------------------
void DJson::set(string Key, DJsonArray Value)
{
	auto Array = nlohmann::json::parse(Value.get_object());
	Object[Key] = Array;
}
//-------------------------------------------------------------------------------------------------
void DJsonArray::add(DJson Value)
{
	add(Value.get_object());
}
//-------------------------------------------------------------------------------------------------
void DJsonArray::add(string Value)
{
	// Converte la stringa in un oggetto json
	auto JsonValue = nlohmann::json::parse(Value);
	// Inserisce l'oggetto nell'array
	Object += JsonValue;
}
//-------------------------------------------------------------------------------------------------
void DJsonArray::clear()
{
	Object = nlohmann::json::array();
}
//-------------------------------------------------------------------------------------------------
bool DJsonArray::get(unsigned int Index, string Key, double & Value)
{
	bool Success = true;
	if (!Object[Index][Key].empty())Value = Object[Index][Key];
	else Success = false;
	return Success;
}
//-------------------------------------------------------------------------------------------------
bool DJsonArray::get(unsigned int Index, string Key, unsigned int & Value)
{
	bool Success = true;
	if (!Object[Index][Key].empty())Value = Object[Index][Key];
	else Success = false;
	return Success;
}
//-------------------------------------------------------------------------------------------------
bool DJsonArray::get(unsigned int Index, string Key, string & Value)
{
	bool Success = true;
	if (!Object[Index][Key].empty())Value = Object[Index][Key];
	else Success = false;
	return Success;
}
//-------------------------------------------------------------------------------------------------
string DJsonArray::get_object()
{
	return DJson::get_object();
}
//-------------------------------------------------------------------------------------------------
string DJsonArray::get_object(unsigned int i)
{
	auto Element = Object[i];
	string JsonValue = Element.dump();
	return JsonValue;
}
//-------------------------------------------------------------------------------------------------
string DJsonArray::get_object(unsigned int i, string Key)
{
	auto Element = Object[i][Key];
	string JsonValue = Element.dump();
	return JsonValue;
}
//-------------------------------------------------------------------------------------------------
bool DJsonArray::is_empty()
{
	return Object.empty();
}
//-------------------------------------------------------------------------------------------------
bool DJsonArray::is_empty(unsigned int i, string Key)
{
	return Object[i][Key].empty();
}
//-------------------------------------------------------------------------------------------------
unsigned long jplibrary::json::DJsonArray::size()
{
	return (unsigned long)Object.size();
}
