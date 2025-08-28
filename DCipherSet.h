//***************************************************************************************
// 1.00.00
//---------------------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include "DError.h"
//---------------------------------------------------------------------------------------
namespace blas::cryptography::cipher
{
	class DCipherSet: public blas::utility::DError
	{
	public:
		DCipherSet();
		~DCipherSet();
		virtual std::pair<uint8_t*, uint32_t> cipher_set(std::pair<uint8_t*, uint32_t>Plain);
		virtual std::pair<uint8_t*, uint32_t> cipher_set(std::string Plain);
		virtual std::pair<uint8_t*, uint32_t> decipher_set(std::pair<uint8_t*, uint32_t>Cipher);
	protected:
		// Funzioni
		virtual void clear();
		virtual void cipher() = 0;
		virtual void decipher() = 0;
		virtual void open(std::pair<uint8_t*, uint32_t>Source) = 0;

		// Attributi
		std::pair<uint8_t*, uint32_t> Input;
		std::pair<uint8_t*, uint32_t> Output;
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DCipherSet::DCipherSet() :DError()
	{

	}
	//-----------------------------------------------------------------------------------
	inline DCipherSet::~DCipherSet()
	{

	}
	//-----------------------------------------------------------------------------------
	inline std::pair<uint8_t*, uint32_t> DCipherSet::cipher_set(std::string Plain)
	{
		return cipher_set(std::make_pair((uint8_t*)Plain.c_str(), Plain.size()));
	}
}


