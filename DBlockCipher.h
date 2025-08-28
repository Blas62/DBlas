//***************************************************************************************
/**
 * @file DBlockCipher.h
 * @brief Interfaccia base per cifrari a blocchi e impostazioni correlate.
 * @details Definisce l’enumerazione delle modalità di funzionamento (@c DMode)
 *          e la classe astratta @c DBlockCipher, che fornisce le API comuni
 *          per cifratura/decifratura di un singolo blocco, gestione di IV,
 *          endianess, dimensione del blocco e nonce. Le classi derivate
 *          implementano le funzioni pure virtuali @c cipher_block(),
 *          @c decipher_block(), @c set_input() e @c set_output(). :contentReference[oaicite:1]{index=1}
 */
#pragma once
//---------------------------------------------------------------------------------------
// include
#include <cstdint>
#include "DCipherAlgorithm.h"
#include "DUtility.h"
//---------------------------------------------------------------------------------------
// Namespace
namespace blas::cryptography::cipher
{
	/**
	 * @enum DMode
	 * @brief Modalità operative per cifrari a blocchi.
	 * @details Elenca le principali modalità di funzionamento: @c ECB, @c CBC,
	 *          @c OFB, @c CFB, @c CTR, @c XTS e @c UMD (modalità sconosciuta/non impostata).
	 *          L’interpretazione concreta della modalità è demandata alle classi derivate. :contentReference[oaicite:2]{index=2}
	 */
	enum DMode
	{
		ECB, // ECB = Electronic codebook
		CBC, // CBC = Cipher block chaining
		OFB, // OFB = Output feedback
		CFB, // CFB = Cipher feedback
		CTR, // CTR = Counter 
		XTS, // XTS = XEX-based tweaked-codebook mode with ciphertext stealing
		UMD  // UMD = Unknown mode
	};
	//-----------------------------------------------------------------------------------
	/**
	 * @class DBlockCipher
	 * @brief Interfaccia astratta per cifrari a blocchi.
	 * @details Fornisce le operazioni di base per cifrare/decifrare un blocco,
	 *          l’accesso/definizione della dimensione del blocco, la gestione del
	 *          vettore di inizializzazione (IV), dell’endianness, del nonce e della modalità.
	 *          Le classi concrete devono implementare @c cipher_block(),
	 *          @c decipher_block(), @c set_input() e @c set_output(). :contentReference[oaicite:10]{index=10}
	 */
	class DBlockCipher : public DCipherAlgorithm
	{
	public: 
		/**
		 * @brief Costruttore predefinito.
		 * @details Inizializza lo stato del cifrario a blocchi (dimensione del blocco,
		 *          parametri di modalità, puntatori a IV/nonce) secondo i default della
		 *          specifica implementazione. :contentReference[oaicite:11]{index=11}
		 * @return Nessun valore di ritorno.
		 */
		DBlockCipher();
		/**
		 * @brief Distruttore virtuale.
		 * @details Consente la corretta distruzione polimorfica e l’eventuale rilascio
		 *          di risorse nelle classi derivate. :contentReference[oaicite:12]{index=12}
		 * @return Nessun valore di ritorno.
		 */
		~DBlockCipher();
		/**
		 * @brief Cifra un singolo blocco di input.
		 * @details Metodo puro virtuale: le classi derivate devono implementare la
		 *          cifratura di un blocco della dimensione configurata, applicando
		 *          la modalità selezionata. Il buffer di output può essere interno
		 *          alla classe derivata. :contentReference[oaicite:13]{index=13}
		 * @param[in] Plain Puntatore al blocco di testo in chiaro (lunghezza = byte del blocco).
		 * @return Puntatore al blocco cifrato (lunghezza = byte del blocco).
		 */
		virtual uint8_t* cipher_block(uint8_t* Plain) = 0; // Funzione virtuale pura
		/**
		 * @brief Decifra un singolo blocco di input.
		 * @details Metodo puro virtuale: le classi derivate devono implementare la
		 *          decifratura di un blocco della dimensione configurata, coerente
		 *          con la modalità selezionata. :contentReference[oaicite:14]{index=14}
		 * @param[in] Cipher Puntatore al blocco di testo cifrato (lunghezza = byte del blocco).
		 * @return Puntatore al blocco in chiaro (lunghezza = byte del blocco).
		 */
		virtual uint8_t* decipher_block(uint8_t* Cipher) = 0; // Funzione virtuale pura
		/**
		 * @brief Restituisce la dimensione del blocco in byte.
		 * @details Calcola la dimensione come numero di byte attualmente configurati
		 *          per il cifrario. :contentReference[oaicite:15]{index=15}
		 * @return Dimensione del blocco in byte.
		 */
		virtual const uint32_t get_block_byte(); // in byte
		/**
		 * @brief Restituisce la dimensione del blocco in bit.
		 * @details Converte la dimensione del blocco da byte a bit. :contentReference[oaicite:16]{index=16}
		 * @return Dimensione del blocco in bit.
		 */
		virtual const uint32_t get_block_size(); // in bit
		/**
		 * @brief Imposta la dimensione del blocco in bit.
		 * @details Aggiorna l’impostazione interna della dimensione del blocco;
		 *          le classi derivate devono garantire che i buffer interni
		 *          vengano adeguati di conseguenza. :contentReference[oaicite:17]{index=17}
		 * @param[in] NewSize Dimensione del blocco espressa in bit.
		 * @return Nessun valore di ritorno.
		 */
		virtual void set_block_size(const uint32_t NewSize); // in bit
		/**
		 * @brief Imposta il vettore di inizializzazione (IV).
		 * @details Assegna @p IV come buffer da utilizzare per le modalità che lo richiedono
		 *          (ad es. CBC, CFB, OFB). La proprietà/gestione della memoria del buffer
		 *          resta al chiamante salvo diversa specifica dell’implementazione derivata. :contentReference[oaicite:18]{index=18}
		 * @param[in] IV Puntatore al buffer IV (lunghezza coerente con la modalità/blocco).
		 * @return Nessun valore di ritorno.
		 */
		virtual void set_init_vector(uint8_t* IV);
		/**
		 * @brief Imposta l’endianness dei dati.
		 * @details Definisce l’ordine dei byte usato dalle trasformazioni interne
		 *          del cifrario (little/big endian), secondo @c blas::utility::DEndian. :contentReference[oaicite:19]{index=19}
		 * @param[in] NewEndian Valore dell’enumerazione @c blas::utility::DEndian.
		 * @return Nessun valore di ritorno.
		 */
		virtual void set_endian(const blas::utility::DEndian NewEndian);
		/**
		 * @brief Seleziona la modalità operativa del cifrario.
		 * @details Imposta @p NewMode tra le modalità supportate (@c ECB, @c CBC, @c OFB,
		 *          @c CFB, @c CTR, @c XTS). L’effetto concreto dipende dall’implementazione
		 *          della classe derivata. :contentReference[oaicite:20]{index=20}
		 * @param[in] NewMode Modalità operativa da utilizzare.
		 * @return Nessun valore di ritorno.
		 */
		virtual void set_mode(DMode NewMode);
		/**
		 * @brief Imposta il nonce da usare nelle modalità che lo richiedono.
		 * @details Memorizza @p NewNonce e la sua dimensione in byte per impieghi in modalità
		 *          basate su contatori o tweak (ad es. CTR/XTS). La gestione della memoria
		 *          del buffer resta al chiamante salvo diversa specifica. :contentReference[oaicite:21]{index=21}
		 * @param[in] NewNonce    Puntatore al buffer contenente il nonce.
		 * @param[in] NewByteSize Dimensione del nonce in byte.
		 * @return Nessun valore di ritorno.
		 */
		virtual void set_nonce(uint8_t* NewNonce, uint32_t NewByteSize);
	protected:
		// Funzioni
		/**
		 * @brief Verifica la correttezza del padding del blocco.
		 * @details Controlla che @p Block sia formattato secondo lo schema di padding
		 *          previsto (es. PKCS#7) e che i byte finali siano coerenti. L’esatta
		 *          semantica dipende dall’implementazione della classe derivata. :contentReference[oaicite:22]{index=22}
		 * @param[in] Block Puntatore al blocco (dimensione = byte del blocco).
		 * @return @c true se il padding è valido, altrimenti @c false.
		 */
		virtual bool check_padding(uint8_t* Block);
		/**
		 * @brief Applica il padding PKCS#7 a un blocco parziale.
		 * @details Completa @p PaddedBlock riempiendo i byte mancanti con il valore
		 *          del numero di byte di padding secondo lo standard PKCS#7. :contentReference[oaicite:23]{index=23}
		 * @param[out] PaddedBlock Buffer del blocco da completare (dimensione = byte del blocco).
		 * @param[in]  BytesRead   Numero di byte effettivamente letti (<= byte del blocco).
		 * @return Nessun valore di ritorno.
		 */
		virtual void pad_block_PKCS7(uint8_t* PaddedBlock, uint32_t BytesRead);
		/**
		 * @brief Carica i dati di input nello stato interno del cifrario.
		 * @details Metodo puro virtuale: le classi derivate devono trasferire il blocco
		 *          sorgente nel buffer/stato interno prima della trasformazione. :contentReference[oaicite:24]{index=24}
		 * @return Nessun valore di ritorno.
		 */
		virtual void set_input() = 0; // Funzione virtuale pura
		/**
		 * @brief Scarica i dati dallo stato interno nel buffer di output.
		 * @details Metodo puro virtuale: le classi derivate devono trasferire il blocco
		 *          risultante dal buffer/stato interno al buffer di uscita. :contentReference[oaicite:25]{index=25}
		 * @return Nessun valore di ritorno.
		 */
		virtual void set_output() = 0; // Funzione virtuale pura

		// Attributi
		uint8_t* Block;
		uint8_t* InitVector;
		uint8_t* Nonce; // Vettore Nonce
		uint32_t ByteNb; // in byte
		uint32_t NonceSize; // Dimensione del vettore Nonce in bit
		blas::utility::DEndian Endian;
		DMode Mode;
	};
	//-----------------------------------------------------------------------------------
	inline DBlockCipher::~DBlockCipher()
	{
	}
	//-----------------------------------------------------------------------------------
	inline const uint32_t DBlockCipher::get_block_byte()
	{
		return ByteNb;
	}
	//-----------------------------------------------------------------------------------
	inline const uint32_t DBlockCipher::get_block_size()
	{
		return ByteNb * 8;
	}
	//-----------------------------------------------------------------------------------
	inline void DBlockCipher::set_block_size(const uint32_t NewSize)
	{
		ByteNb = NewSize / 8;
	}
	//------------------------------------------------------------------------------------
	inline void DBlockCipher::set_init_vector(uint8_t* IV)
	{
		InitVector = IV;
	}
	//------------------------------------------------------------------------------------
	inline void DBlockCipher::set_endian(const blas::utility::DEndian NewEndian)
	{
		Endian = NewEndian;
	}
	//------------------------------------------------------------------------------------
	inline void DBlockCipher::set_mode(DMode NewMode)
	{
		Mode = NewMode;
	}
}
/*
namespace blas::cryptography::cipher
{
	enum DMode 
	{ 
		ECB, // ECB = Electronic codebook
		CBC, // CBC = Cipher block chaining
		OFB, // OFB = Output feedback
		CFB, // CFB = Cipher feedback
		CTR, // CTR = Counter 
		XTS  // XTS = XEX-based tweaked-codebook mode with ciphertext stealing
	};

	class DBlockCipher : public DCipherAlgorithm
	{
	public:
		DBlockCipher();
		~DBlockCipher();
		virtual uint8_t* cipher_block(uint8_t* Plain) = 0; // Funzione virtuale pura
		virtual uint8_t* decipher_block(uint8_t* Cipher) = 0; // Funzione virtuale pura
		virtual const uint32_t get_block_byte(); // in byte
		virtual const uint32_t get_block_size(); // in bit
		virtual void set_block_size(const uint32_t NewSize); // in bit
		virtual void set_init_vector(uint8_t* IV);
		virtual void set_endian(const blas::utility::DEndian NewEndian);
		virtual void set_mode(DMode NewMode);
		virtual void set_nonce(uint8_t* NewNonce, uint32_t NewByteSize);
	protected:
		// Funzioni
		virtual bool check_padding(uint8_t* Block);
		virtual void pad_block_PKCS7(uint8_t* PaddedBlock, uint32_t BytesRead);
		virtual void set_input() = 0; // Funzione virtuale pura
		virtual void set_output() = 0; // Funzione virtuale pura
		//virtual void zeroize() = 0;

		// Attributi
		uint32_t ByteNb; // in byte
		uint8_t* Block;
		blas::utility::DEndian Endian;
		uint8_t* InitVector;
		DMode Mode;
		uint8_t* Nonce;
		uint32_t NonceSize; // Dimensione del vettore Nonce in bit
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DBlockCipher::~DBlockCipher()
	{
	}
	//-----------------------------------------------------------------------------------
	inline const uint32_t DBlockCipher::get_block_byte()
	{
		return ByteNb;
	}
	//-----------------------------------------------------------------------------------
	inline const uint32_t DBlockCipher::get_block_size()
	{
		return ByteNb * 8;
	}
	//-----------------------------------------------------------------------------------
	inline void DBlockCipher::set_block_size(const uint32_t NewSize)
	{
		ByteNb = NewSize / 8;
	}
	//------------------------------------------------------------------------------------
	inline void DBlockCipher::set_init_vector(uint8_t* IV)
	{
		InitVector = IV;
	}
	//------------------------------------------------------------------------------------
	inline void DBlockCipher::set_endian(const blas::utility::DEndian NewEndian)
	{
		Endian = NewEndian;
	}
	//------------------------------------------------------------------------------------
	inline void DBlockCipher::set_mode(DMode NewMode)
	{
		Mode = NewMode;
	}
}
*/