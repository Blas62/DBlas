//***************************************************************************************
#pragma once
//---------------------------------------------------------------------------------------
// include
#include <cstdint>
#include "DUtility.h"
//---------------------------------------------------------------------------------------
// Namespace
namespace blas::cryptography::cipher
{
	class DCipherAlgorithm : public blas::utility::DError
	{
	public:
		DCipherAlgorithm();
		~DCipherAlgorithm();
		virtual std::string get_algorithm_name();
		virtual std::string get_algorithm_type();
		virtual uint32_t    get_key_byte(); // in byte
		virtual uint32_t    get_key_size(); // in bit
		virtual void        set_key_size(uint32_t NewKeysize); // in bit
		virtual void set_key(uint8_t* NewKey) = 0; // Funzione virtuale pura
	protected:
		// Funzioni
		virtual void initialize() = 0; // Funzione virtuale pura
		virtual void set_algorithm_name(std::string Name);
		virtual void set_algorithm_type(std::string Type);
		virtual void zeroize() = 0;
		//virtual void set_key(uint8_t* NewKey) = 0; // Funzione virtuale pura

		// Attributi
		std::string AlgorithmName; // Nome dell'algoritmo di cifratura
		std::string AlgorithmType; // Tipo di algoritmo di cifratura (simmetrico, asimmetrico, etc.)
		uint32_t KeySize;          // Dimensione della chiave in bit
		uint8_t* Key;              // Puntatore alla chiave di cifratura
	};
	//-----------------------------------------------------------------------------------
	inline DCipherAlgorithm::~DCipherAlgorithm()
	{
		// Azzera la chiave
		//zeroize();
	}
	//-----------------------------------------------------------------------------------
	inline std::string DCipherAlgorithm::get_algorithm_name()
	{
		return AlgorithmName;
	}
	//-----------------------------------------------------------------------------------
	inline std::string DCipherAlgorithm::get_algorithm_type()
	{
		return AlgorithmType;
	}
	//-----------------------------------------------------------------------------------
	inline uint32_t DCipherAlgorithm::get_key_byte()
	{
		return KeySize / 8;
	}
	//-----------------------------------------------------------------------------------
	inline uint32_t DCipherAlgorithm::get_key_size()
	{
		return KeySize;
	}
	//-----------------------------------------------------------------------------------
	inline void DCipherAlgorithm::set_key_size(uint32_t NewKeysize)
	{
		KeySize = NewKeysize;
	}
	//-----------------------------------------------------------------------------------
	inline void DCipherAlgorithm::set_algorithm_name(std::string Name)
	{
		AlgorithmName = Name;
	}
	//-----------------------------------------------------------------------------------
	inline void DCipherAlgorithm::set_algorithm_type(std::string Type)
	{
		AlgorithmType = Type;
	}
}
/*
namespace blas::cryptography::cipher
{
	class DCipherAlgorithm : public blas::utility::DError
	{
	public:
		DCipherAlgorithm();
		virtual ~DCipherAlgorithm();
		virtual std::string get_algorithm_name();
		virtual uint32_t    get_key_byte(); // in byte
		virtual uint32_t    get_key_size(); // in bit
		virtual std::string get_algorithm_type();
		virtual void        set_key_size(uint32_t NewKeysize); // in bit
	protected:
		// Funzioni
		virtual void initialize() = 0; // Funzione virtuale pura
		virtual void set_algorithm_name(std::string Name);
		virtual void set_algorithm_type(std::string Type);
		virtual void set_key(uint8_t* NewKey) = 0; // Funzione virtuale pura
		virtual void zeroize() = 0;

		// Attributi
		std::string AlgorithmName; // Nome dell'algoritmo di cifratura
		std::string AlgorithmType; // Tipo di algoritmo di cifratura (simmetrico, asimmetrico, etc.)
		uint32_t KeySize;          // Dimensione della chiave in bit
		uint8_t* Key;              // Puntatore alla chiave di cifratura
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DCipherAlgorithm::~DCipherAlgorithm()
	{
		// Azzera la chiave
		zeroize();
	}
	//-----------------------------------------------------------------------------------
	inline std::string DCipherAlgorithm::get_algorithm_name()
	{
		return AlgorithmName;
	}
	//-----------------------------------------------------------------------------------
	inline uint32_t DCipherAlgorithm::get_key_byte()
	{
		return KeySize / 8;
	}
	//-----------------------------------------------------------------------------------
	inline uint32_t DCipherAlgorithm::get_key_size()
	{
		return KeySize;
	}
	//-----------------------------------------------------------------------------------
	inline std::string DCipherAlgorithm::get_algorithm_type()
	{
		return AlgorithmType;
	}
	//-----------------------------------------------------------------------------------
	inline void DCipherAlgorithm::set_key_size(uint32_t NewKeysize)
	{
		KeySize = NewKeysize;
	}
	//-----------------------------------------------------------------------------------
	inline void DCipherAlgorithm::set_algorithm_name(std::string Name)
	{
		AlgorithmName = Name;
	}
	//-----------------------------------------------------------------------------------
	inline void DCipherAlgorithm::set_algorithm_type(std::string Type)
	{
		AlgorithmType = Type;
	}
}
*/