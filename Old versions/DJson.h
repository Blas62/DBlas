//----------------------------------------------------------------------------+
// DJson.h                                                                    |
//----------------------------------------------------------------------------+
#pragma once
//----------------------------------------------------------------------------+
// include
#include "nlohmann/json.hpp"
#include "DError.h"
//----------------------------------------------------------------------------+
// namespace
using json = nlohmann::json;
//----------------------------------------------------------------------------+

namespace jplibrary
{
	namespace format 
	{
		class DJson; // Gestisce oggetti in formato json
		class DJsonArray; // Gestisce array di oggetti json
		//--------------------------------------------------------------------+
		class DJson:public DError
		{
		public:
			DJson();
			~DJson();
			virtual void clear();
			virtual bool get(std::string Key, double& Value);
			virtual bool get(std::string Key, std::string& Value);
			virtual bool get(std::string Key, unsigned int& Value);
			virtual bool get(std::string Key, uint16_t& Value);
			virtual bool get(std::string Key, int& Value);
			virtual std::string get_object();
			virtual std::string get_object(int Indent);
			virtual std::string get_object(std::string Key);
			virtual bool is_empty();
			virtual bool is_empty(std::string Key);
			virtual bool load(std::string FileName);
			virtual bool save(std::string FileName);
			virtual void set(std::string Value);
			virtual void set(std::string Key, bool Value);
			virtual void set(std::string Key, double Value);
			virtual void set(std::string Key, int Value);
			virtual void set(std::string Key, unsigned int Value);
			virtual void set(std::string Key, std::string Value);
			virtual void set(std::string Key, DJsonArray Value);
		protected:
			 json Object;
		private:
		};
		//--------------------------------------------------------------------+
		class DJsonArray :public DJson
		{
		public:
			DJsonArray();
			~DJsonArray();
			virtual void add(DJson Value);
			virtual void add(std::string Value);
			virtual void clear();
			virtual bool get(
				unsigned int Index, 
				std::string Key, 
				double& Value);
			virtual bool get(
				unsigned int Index, 
				std::string Key, 
				unsigned int& Value);
			virtual bool get(
				unsigned int Index, 
				std::string Key, 
				std::string& Value);
			virtual std::string get_object();
			virtual std::string get_object(unsigned int i);
			virtual std::string get_object(
				unsigned int i, 
				std::string Key);
			virtual bool is_empty();
			virtual bool is_empty(unsigned int i,std::string Key);
			virtual unsigned long size();
		};
	}
}


