//***************************************************************************************
// 0.02.00
#pragma once
//---------------------------------------------------------------------------------------
// include
#include <cstdint>
#include "DError.h"
#include "DBlockCipher.h"
//---------------------------------------------------------------------------------------
// Namespace
namespace blas::cryptography::analysis
{
	static double ENG[27] =
	{
		/*a*/0.08167,/*b*/0.01492,/*c*/0.02782,/*d*/0.04253,/*e*/0.12702,/*f*/0.02228,
		/*g*/0.02015,/*h*/0.06094,/*i*/0.06966,/*j*/0.00153,/*k*/0.00772,/*l*/0.04025,
		/*m*/0.02406,/*n*/0.06749,/*o*/0.07507,/*p*/0.01929,/*q*/0.00095,/*r*/0.05987,
		/*s*/0.06327,/*t*/0.09056,/*u*/0.02758,/*v*/0.00978,/*w*/0.02361,/*x*/0.00150,
		/*y*/0.01974,/*z*/0.00074,/* */0.19181
	};
	//-----------------------------------------------------------------------------------
	class DCryptoAnalysis : public blas::utility::DError
	{
	public:
		DCryptoAnalysis();
		~DCryptoAnalysis();
		std::pair<uint8_t*, uint32_t>        compute_XOR_best_key(std::filesystem::path Text, const uint32_t KeySize, const double* Language);
		std::tuple<uint32_t, double, double> compute_XOR_best_key_size(std::filesystem::path Text, const uint32_t MaxKeySize);
		uint32_t                             detect_block_length(std::string IPAddress, uint16_t Port);
		cipher::DMode                        detect_block_cipher_mode(std::filesystem::path Cipher, const uint32_t BlockLength);
		cipher::DMode                        detect_block_cipher_mode(const std::pair<uint8_t*, uint32_t> Cipher, const uint32_t BlockLength);
		cipher::DMode                        detect_block_cipher_mode(std::string IPAddress, uint16_t Port, uint32_t BlockLength);
		bool                                 find_identical_adjacent_blocks(const std::pair<uint8_t*, uint32_t> Text, const uint32_t BlockLength);
		double*                              get_character_frequency(const std::pair<uint8_t*, uint32_t> Text);
		uint32_t                             get_cipher_cookie_length(std::string IPAddress, uint16_t Port, uint32_t BlockLength);
		std::pair<uint32_t, uint32_t>        get_cipher_cookie_and_prefix_length(std::string IPAddress, uint16_t Port, uint32_t BlockLength);
		double                               get_Hamming_distance(const std::pair<uint8_t*, uint32_t> Text, uint32_t BlockLength);
		uint32_t                             get_Hamming_distance(const std::pair<uint8_t*, uint32_t> Text1, const std::pair<uint8_t*, uint32_t> Text2);
		std::pair<bool, uint32_t>            get_identical_adjacent_blocks(const std::pair<uint8_t*, uint32_t> Text, const uint32_t BlockLength);
		double                               get_language_score(const std::pair<uint8_t*, uint32_t> Text, const double* Language);
	protected:
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DCryptoAnalysis::DCryptoAnalysis(): DError()
	{

	}
	//-----------------------------------------------------------------------------------
	inline DCryptoAnalysis::~DCryptoAnalysis()
	{

	}
}
