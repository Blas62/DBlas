//***************************************************************************************
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include <filesystem>
#include <fstream>
#include "DUtility.h"
//---------------------------------------------------------------------------------------
// Namespace
namespace blas::cryptography::cipher
{
	class DCipherFile : public blas::utility::DError
	{
	public:
		DCipherFile();
		~DCipherFile();
		virtual void cipher_file(std::filesystem::path Plain, std::filesystem::path Cipher);
		virtual void decipher_file(std::filesystem::path Cipher, std::filesystem::path Plain);
		virtual void set_temp_ext(std::string Ext);
	protected:
		// Funzioni
		virtual std::string cipher() = 0;
		virtual void close_file();
		virtual std::string decipher() = 0;
		virtual void open_file();
		virtual void set_file_name(std::filesystem::path NewSrcName, std::filesystem::path NewDstName);
		virtual void set_source_size();
		// Attributi
		std::filesystem::path SrcName;
		std::filesystem::path DstName;
		std::filesystem::path TmpName;
		std::ifstream In;
		std::ofstream Out;
		std::string TmpExt;
		uint32_t SrcSize;
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DCipherFile::DCipherFile() :DError()
	{
		// Imposta l'estensione temporanea predefinita
		set_temp_ext(".123");
	}
	//-----------------------------------------------------------------------------------
	inline DCipherFile::~DCipherFile()
	{

	}
	//-----------------------------------------------------------------------------------
	inline void DCipherFile::set_temp_ext(std::string Ext)
	{
		// Imposta l'estensione temporanea da usare per i file intermedi
		TmpExt = Ext;
	}
	//-----------------------------------------------------------------------------------
	inline void DCipherFile::set_source_size()
	{
		// Calcola la dimensione del file sorgente in byte e la memorizza
		SrcSize = (uint32_t)std::filesystem::file_size(SrcName);
	}
}
