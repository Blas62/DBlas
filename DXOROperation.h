//***************************************************************************************
#pragma once
//---------------------------------------------------------------------------------------
// include
#include <cstdint>
#include <utility>
#include "DUtility.h"
//---------------------------------------------------------------------------------------
// namespace
namespace blas::cryptography::fundamental_algorithms
{
	class DXOROperation : public blas::utility::DError
	{
	public:
		DXOROperation();
		~DXOROperation();
		std::pair<uint8_t*, uint32_t> XOR(const std::filesystem::path Cipher,
			const std::filesystem::path Plain,
			const std::pair<uint8_t*, uint32_t> Key);
		std::pair<uint8_t*, uint32_t> XOR(const std::pair<uint8_t*, uint32_t> Text, 
			const uint8_t Key);
		std::pair<uint8_t*, uint32_t> XOR(const std::pair<uint8_t*, uint32_t> Text, 
			const std::pair<uint8_t*, uint32_t> Key);
	protected:
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DXOROperation::DXOROperation() : DError()
	{
		// Costruttore
	}
	//-----------------------------------------------------------------------------------
	inline DXOROperation::~DXOROperation()
	{
		// Distruttore
	}
}
