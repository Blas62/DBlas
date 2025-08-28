//***************************************************************************************
// Include
#include "pch.h"
#include "DBlockPadding.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace std;
using namespace blas::cryptography::fundamental_algorithms;
using namespace blas::utility;
//---------------------------------------------------------------------------------------
uint8_t* DBlockPadding::PCS7_padding(pair<uint8_t*, uint32_t> Text, uint32_t BlockSize)
{
	// Alloca lo spazio per il blocco da completare
	uint8_t* PaddedText = allocate_memory<uint8_t>(BlockSize);
	
	// Calcola il valore di padding
	uint32_t PadVaue = BlockSize - Text.second;

	// Riempie il blocco PaddedText con il valore di padding
	memset(PaddedText, (uint8_t)PadVaue, BlockSize);

	// Copia i caratteri del testo all'inizio del blocco da completare
	for (uint32_t i = 0; i < Text.second; i++)PaddedText[i] = Text.first[i];

	// Restituise il blocco completato
	return PaddedText;
}
//---------------------------------------------------------------------------------------
uint8_t* DBlockPadding::unpad_block_PKCS7(pair<uint8_t*, uint32_t> Block)
{
	try
	{
		// Calcola il valore di padding
		uint8_t PadValue = Block.first[Block.second - 1];

		// Se è più grande della dimensio ne del blocco lancia una eccezione
		if (PadValue >= Block.second)throw "invalid pad value";

		// Se l'ultimo byte del padding è diverso lancia una eccezione
		for (uint32_t i = 0; i < PadValue; i++)
			if (Block.first[Block.second - i - 1] != PadValue)throw "wrong padding";

		// Alloca lo spazio per un blocco senza padding
		//uint8_t* CleanBlock = new uint8_t[Size - PadValue + 1];
		//memset(CleanBlock, 0, Size - PadValue + 1);
		uint8_t* CleanBlock = allocate_memory<uint8_t>(Block.second - PadValue + 1);

		// Copia i caratteri validi nel blocco senza padding
		for (uint32_t i = 0; i < Block.second - PadValue; i++)CleanBlock[i] = Block.first[i];

		// Restituisce il blocco senza padding
		return CleanBlock;
	}
	catch (const char* e)
	{
		// Assegna l'errore
		set_error(STDERROR, e);

		// Restituisce il puntatore nullo
		return nullptr;
	};
}
