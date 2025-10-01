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
		std::tuple<uint8_t*, uint8_t, double> brute_force_single_character_key_xor(const std::pair<uint8_t*, uint32_t> Cipher, const double* Language);
		std::pair<uint8_t*, uint32_t>byte_at_a_time_to_block_cipher(std::string IPAddress, uint16_t Port, uint32_t BlockLength);
		std::pair<uint8_t*, uint32_t>byte_at_a_time_to_block_cipher_with_prefix(std::string IPAddress, uint16_t Port, uint32_t BlockLength);
		std::pair<uint8_t*, uint32_t>CBC_padding_oracle(const std::pair<std::string, uint16_t>Oracle, uint32_t BlockLength, std::string Cookie);
		bool ask_padding_oracle(const std::pair<std::string, uint16_t>Oracle, uint32_t BlockLength,std::pair<uint8_t*, uint32_t>CipherBlock,std::pair<uint8_t*, uint32_t>Vector);
	protected:
		//std::pair<uint8_t*, uint32_t>block_decrypt_oracle(const std::pair<std::string, uint16_t>Oracle, uint32_t BlockLength, std::pair<uint8_t*, uint32_t>Cipher);
		//uint8_t word_decrypt_oracle(const std::pair<std::string, uint16_t>Oracle, uint32_t BlockLength);
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