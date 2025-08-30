//***************************************************************************************
// Include
#include "pch.h"
#include "DJson.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace blas::utility::data_format;
using namespace nlohmann;
using namespace std;
//---------------------------------------------------------------------------------------
std::string DJson::get_object(int Indent)
{
	try
	{
		if (!Object.empty()) return Object.dump(Indent);
		else throw std::invalid_argument("Empty or non-existent key");

	}
	catch (std::exception& e)
	{
		set_error(STDERROR, e.what());
	}
}
//---------------------------------------------------------------------------------------
string DJson::get_object(string Key)
{
	try
	{
		if (!Object[Key].empty())
		{
			auto Buffer = Object[Key];
			return Buffer.dump();
		}
		else throw std::invalid_argument("Empty or non-existent key");
	}
	catch (std::exception& e)
	{
		set_error(STDERROR, e.what());
	}
}

//---------------------------------------------------------------------------------------
void DJson::load(std::filesystem::path FileName)
{
	try
	{
		std::ifstream File;
		File.open(FileName);
		File >> Object;
		File.close();
	}
	catch (std::exception& e)
	{
		set_error(STDERROR, e.what());
	}
}
//---------------------------------------------------------------------------------------
void blas::utility::data_format::DJson::save(std::filesystem::path FileName)
{
	try
	{
		std::ofstream File;
		File.open(FileName);
		File << Object;
		File.close();
	}
	catch (std::exception& e)
	{
		set_error(STDERROR, e.what());
	}
}
