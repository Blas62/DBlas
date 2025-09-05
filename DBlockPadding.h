//***************************************************************************************
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include "DUtility.h"
//---------------------------------------------------------------------------------------
// Namespace
namespace blas::cryptography::fundamental_algorithms
{
	class DBlockPadding: public blas::utility::DError
	{
	public:
		DBlockPadding();
		~DBlockPadding();
		virtual uint8_t* PCS7_padding(std::pair<uint8_t*, uint32_t> Text, uint32_t BlockSize);
		virtual uint8_t* unpad_block_PKCS7(std::pair<uint8_t*, uint32_t> Block);

	protected:
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DBlockPadding::DBlockPadding() : DError()
	{

	}
	//-----------------------------------------------------------------------------------
	inline DBlockPadding::~DBlockPadding()
	{

	}
}

