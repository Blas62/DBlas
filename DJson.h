//***************************************************************************************
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include <exception>
#include "DError.h"
#include <nlohmann\json.hpp>
//---------------------------------------------------------------------------------------
namespace blas::utility::data_format
{
	class DJson: public blas::utility::DError
	{
	public:
		DJson();
		~DJson();
		void clear();
		template<typename T>T get(const std::string& Key) const 
		{
			try
			{
				if (!Object[Key].empty()) return static_cast<T>(Object[Key]);
				else throw std::invalid_argument("Empty or non-existent key");
			}
			catch (std::exception& e)
			{
				// TODO: capire perché la compilazione provoca un errore
				//set_error(STDERROR, e.what());
			}
		}
		std::string get_object(int Indent);
		std::string get_object(std::string Key);
		void set(const std::string& Value);
        template<typename T> void set(const std::string& Key, const T& Value)
        {
            Object[Key] = Value;
        }
		const std::string get_object();
		bool is_empty();
		bool is_empty(std::string Key);
		void load(std::filesystem::path FileName);
		void save(std::filesystem::path FileName);
	protected:
		nlohmann::json Object;
	};
	//-----------------------------------------------------------------------------------
	inline DJson::DJson() : DError()
	{
		clear();
	}
	//-----------------------------------------------------------------------------------
	inline DJson::~DJson()
	{

	}
	//-----------------------------------------------------------------------------------
	inline void DJson::clear()
	{
		Object = nlohmann::json::object();
	}
	//-----------------------------------------------------------------------------------
	inline const std::string DJson::get_object()
	{
		return get_object(0);
	}
	//-----------------------------------------------------------------------------------
	inline bool DJson::is_empty()
	{
		return Object.empty();
	}
	//-----------------------------------------------------------------------------------
	inline bool DJson::is_empty(std::string Key)
	{
		return Object[Key].empty();
	}
	inline void DJson::set(const std::string& Value)
	{
		Object= nlohmann::json::parse(Value);
	}
}

