//***************************************************************************************
// Include
#include "pch.h"
#include "DBlockCipher.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace blas::cryptography::cipher;
using namespace blas::utility;
//---------------------------------------------------------------------------------------
DBlockCipher::DBlockCipher() :DCipherAlgorithm()
{
	// Inizializza il vettore del blocco
	Block = nullptr;

	// Inizializza nullptr il vettore InitVector
	InitVector = nullptr;

	// Inizializza l'attributo Endian a big
	Endian = big;

	// Inizializza con nullptr il vettore Nonce
	Nonce = nullptr;

	// Inizializza l'attributo NonceSize standard a 16 byte
	NonceSize = 16 / 8;
}
//---------------------------------------------------------------------------------------
void DBlockCipher::set_nonce(uint8_t* NewNonce, uint32_t NewByteSize)
{
	// Assegna il vettore
	Nonce = NewNonce;

	// Assegna la lunghezza in bit
	NonceSize = NewByteSize / 8;
}
//---------------------------------------------------------------------------------------
bool DBlockCipher::check_padding(uint8_t* Block)
{
	try
	{
		// Legge il possibile carattere di padding
		uint8_t PadValue = Block[ByteNb - 1];

		// Se il valore di padding è maggiore della lunghezza dal
		// blocco, lancia una eccezione
		if (PadValue > ByteNb)throw "invalid pad value or no padding";

		// Se il valore dell'i-esimo byte di padding non è 
		// uguale al valore di padding, lancia una eccezione
		for (uint32_t i = 0; i < PadValue; i++)
		{
			if (Block[ByteNb - i - 1] != PadValue)throw "insufficient padding";
		}

		// Se il valore dell'ultimo byte valido è uguale al 
		// valore di padding lancia una eccezione

		if (PadValue != ByteNb)
		{
			if (Block[ByteNb - PadValue - 1] == PadValue)throw "excessive padding";
		}
		// Il padding è corretto e restituisce true
		return true;
	}
	catch (const char* e)
	{
		// Imposta il messaggio di errore 
		set_error(STDERROR, e);

		// Restuituisce false
		return false;
	};
}
//---------------------------------------------------------------------------------------
void DBlockCipher::pad_block_PKCS7(uint8_t* PaddedBlock, uint32_t BytesRead)
{
	// Calcola il valore del pad
	uint32_t PadValue = ByteNb - BytesRead;

	// Esegui il padding
	for (uint32_t i = ByteNb - PadValue; i < ByteNb; i++)PaddedBlock[i] = (uint8_t)PadValue;
}
/*
DBlockCipher::DBlockCipher() :DCipherAlgorithm()
{
	// Inizializza nullptr il vettore InitVector
	InitVector = nullptr;

	// Inizializza l'attributo Endian a big
	Endian = big;

	// Inizializza con nullptr il vettore Nonce
	Nonce = nullptr;

	// Inizializza l'attributo NonceSize standard a 16 byte
	NonceSize = 16 / 8;
}
//---------------------------------------------------------------------------------------
void DBlockCipher::set_nonce(uint8_t* NewNonce, uint32_t NewByteSize)
{
	// Assegna il vettore
	Nonce = NewNonce;

	// Assegna la lunghezza in bit
	NonceSize = NewByteSize / 8;
}
//---------------------------------------------------------------------------------------
bool DBlockCipher::check_padding(uint8_t* Block)
{
	try
	{
		// Legge il possibile carattere di padding
		uint8_t PadValue = Block[ByteNb - 1];

		// Se il valore di padding è maggiore della lunghezza dal
		// blocco, lancia una eccezione
		if (PadValue >= ByteNb)throw "invalid pad value or no padding";

		// Se il valore dell'i-esimo byte di padding non è 
		// uguale al valore di padding, lancia una eccezione
		for (uint32_t i = 0; i < PadValue; i++)
		{
			if (Block[ByteNb - i - 1] != PadValue)throw "insufficient padding";
		}

		// Se il valore dell'ultimo byte valido è uguale al 
		// valore di padding lancia una eccezione
		if (Block[ByteNb - PadValue - 1] == PadValue)throw "excessive padding";

		// Il padding è corretto e restituisce true
		return true;
	}
	catch (const char* e)
	{
		// Imposta il messaggio di errore 
		set_error(STDERROR, e);

		// Restuituisce false
		return false;
	};
}
//---------------------------------------------------------------------------------------
void DBlockCipher::pad_block_PKCS7(uint8_t* PaddedBlock, uint32_t BytesRead)
{
	// Calcola il valore del pad
	uint32_t PadValue = ByteNb - BytesRead;

	// Esegui il padding
	for (uint32_t i = ByteNb - PadValue; i < ByteNb; i++)PaddedBlock[i] = (uint8_t)PadValue;
}
*/
