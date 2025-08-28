//***************************************************************************************
/**
 * @file DAESCipherFile.h
 * @brief Implementazione di un cifrario AES applicato a file.
 * @details La classe @c DAESCipherFile estende le funzionalità di @c DBlockCipherFile
 *          integrando l’algoritmo @c DAES per fornire servizi di cifratura e
 *          decifratura su blocchi di file. Espone costruttori per inizializzare
 *          il contesto AES con dimensione chiave predefinita o specificata e
 *          ridefinisce le operazioni di @c cipher_block() e @c decipher_block()
 *          per delegarle direttamente a @c DAES. :contentReference[oaicite:0]{index=0}
 */
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include "DBlockCipherFile.h"
#include "DAESCipher.h"
//---------------------------------------------------------------------------------------
namespace blas::cryptography::cipher
{
	/**
	 * @class DAESCipherFile
	 * @brief Cifrario AES per la gestione di file a blocchi.
	 * @details Combina le capacità di @c DBlockCipherFile e @c DAES per consentire
	 *          la cifratura e decifratura di file binari a blocchi di 128 bit.
	 *          Fornisce costruttori per inizializzare il contesto con o senza
	 *          dimensione chiave specificata. :contentReference[oaicite:1]{index=1}
	 */
	class DAESCipherFile :public virtual DBlockCipherFile,
		public virtual DAES
	{
	public:
		/**
		 * @brief Costruttore predefinito.
		 * @details Inizializza il contesto senza configurare una chiave;
		 *          la chiave dovrà essere impostata tramite @c set_key(). :contentReference[oaicite:2]{index=2}
		 * @return Nessun valore di ritorno.
		 */
		DAESCipherFile();
		/**
		 * @brief Costruttore con dimensione della chiave.
		 * @details Inizializza l’oggetto impostando la dimensione della chiave AES
		 *          a @p NewKeyLength bit (tipicamente 128, 192 o 256). :contentReference[oaicite:3]{index=3}
		 * @param[in] NewKeyLength Dimensione della chiave in bit.
		 * @return Nessun valore di ritorno.
		 */
		DAESCipherFile(uint16_t NewKeyLength);
		/**
		 * @brief Distruttore.
		 * @details Invoca @c DAES::zeroize() per azzerare la memoria sensibile
		 *          relativa alle chiavi e allo stato interno. :contentReference[oaicite:4]{index=4}
		 * @return Nessun valore di ritorno.
		 */
		~DAESCipherFile();
		/**
		 * @brief Cifra un blocco di input.
		 * @details Delegata diretta a @c DAES::cipher_block(), applica l’algoritmo AES
		 *          al blocco di 16 byte fornito. :contentReference[oaicite:5]{index=5}
		 * @param[in] input Puntatore a 16 byte di testo in chiaro.
		 * @return Puntatore a 16 byte di testo cifrato (buffer gestito dalla classe).
		 */
		uint8_t* cipher_block(uint8_t* input) override;
		/**
		 * @brief Decifra un blocco di input.
		 * @details Delegata diretta a @c DAES::decipher_block(), applica l’algoritmo AES
		 *          al blocco di 16 byte fornito. :contentReference[oaicite:6]{index=6}
		 * @param[in] input Puntatore a 16 byte di testo cifrato.
		 * @return Puntatore a 16 byte di testo in chiaro (buffer gestito dalla classe).
		 */
		uint8_t* decipher_block(uint8_t* input) override;
	protected:
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DAESCipherFile::DAESCipherFile() :
		DBlockCipherFile(), 
		DAES()
	{
		// Costruttore predefinito: nessuna logica aggiuntiva
	}
	//-----------------------------------------------------------------------------------
	inline DAESCipherFile::DAESCipherFile(uint16_t NewKeyLength) : 
		DBlockCipherFile(), 
		DAES(NewKeyLength)
	{
		// Costruttore con inizializzazione della chiave AES a NewKeyLength bit
	}
	//-----------------------------------------------------------------------------------
	inline DAESCipherFile::~DAESCipherFile()
	{
		DAES::zeroize();
	}
	//-----------------------------------------------------------------------------------
	inline uint8_t* DAESCipherFile::cipher_block(uint8_t* input)
	{
		return DAES::cipher_block(input);
	}
	//-----------------------------------------------------------------------------------
	inline uint8_t* DAESCipherFile::decipher_block(uint8_t* input)
	{
		return DAES::decipher_block(input);
	}
}
