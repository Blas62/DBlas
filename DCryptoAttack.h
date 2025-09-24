//***************************************************************************************
#pragma once
//---------------------------------------------------------------------------------------
// include
#include <cstdint>
#include <utility>
#include "DUtility.h"
#include "DXOROperation.h"
//---------------------------------------------------------------------------------------
// namespace
namespace blas::cryptography::attack
{
	class DCryptoAttack: public blas::utility::DError
	{
	public:
		DCryptoAttack();
		~DCryptoAttack();
		std::tuple<uint8_t*, uint8_t, double> brute_force_single_character_key_xor(
			const std::pair<uint8_t*, uint32_t> Cipher,
			const double* Language);
		std::pair<uint8_t*, uint32_t>byte_at_a_time_to_block_cipher(std::string IPAddress, uint16_t Port, uint32_t BlockLength);
		std::pair<uint8_t*, uint32_t>byte_at_a_time_to_block_cipher_with_prefix(std::string IPAddress, uint16_t Port, uint32_t BlockLength);
		std::pair<uint8_t*, uint32_t>CBC_padding_oracle(std::string IPAddress, 
			uint16_t Port, 
			std::pair<uint8_t*, uint32_t> Cookie, 
			uint32_t BlockLength);
	
	protected:
		std::pair<uint8_t*, uint32_t>block_CBC_padding_oracle(std::string IPAddress,
			uint16_t Port,
			std::pair<uint8_t*, uint32_t> CipherBlock,
			uint32_t Index,
			uint32_t BlockLength);

		//std::pair<uint8_t*, uint32_t>copy_to_probe(std::pair<uint8_t*, uint32_t>Array1, std::pair<uint8_t*, uint32_t>Array2);
		//std::pair<uint8_t*, uint32_t>shift_to_left(std::pair<uint8_t*, uint32_t>Array);
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DCryptoAttack::DCryptoAttack() : DError() 
	{

	}
	//-----------------------------------------------------------------------------------
	inline DCryptoAttack::~DCryptoAttack() 
	{

	}
}