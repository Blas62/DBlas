//***************************************************************************************
// 0.01.00
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include <utility>
#include "DCryptoAnalysis.h"
#include "DBlockCipher.h"
#include "DBlockCipherFile.h"
//---------------------------------------------------------------------------------------
// Namespace
namespace blas::cryptography::analysis
{
	class DCryptoTestTools
	{
	public:
		DCryptoTestTools();
		~DCryptoTestTools();
		uint8_t* create_test_random_string(const uint32_t Length);
		std::pair<blas::cryptography::cipher::DMode,std::filesystem::path> encryption_oracle(
			blas::cryptography::cipher::DBlockCipherFile& Cipher, 
			std::filesystem::path FileName,const uint32_t BlockLength);
	protected:
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DCryptoTestTools::DCryptoTestTools()
	{

	}
	//-----------------------------------------------------------------------------------
	inline DCryptoTestTools::~DCryptoTestTools()
	{
	}
}


