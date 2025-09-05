//***************************************************************************************
// Pragma
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include <cstdint>
#include <utility>
//---------------------------------------------------------------------------------------
// Namespace
namespace blas 
{
	namespace utility
	{

		/**
		 * @brief Enum per rappresentare l'ordine dei byte (endianness).
		 * @details Consente di specificare se la memoria utilizza ordine little-endian, big-endian o nativo della piattaforma.
		 *
		 * @author Blas de Lezo
		 * @date 2025
		 */
		enum DEndian { little, big, native };
		//-------------------------------------------------------------------------------
		template<class T>
		T* allocate_memory(size_t Size)
		{
			// Alloca lo spazio di memoria
			T* Vector = new T[Size + 1];
			// Azzera lo spazio allocato
			memset(Vector, 0, (Size + 1)*sizeof(T));
			// Restituisce il puntatore all'area di memoria allocata
			return Vector;
		}
		//-------------------------------------------------------------------------------
		template<class T>
		void deallocate_memory(T* Vector)
		{
			if (Vector != nullptr)
			{
				delete[] Vector;
				Vector = nullptr;
			}
		}
		//-------------------------------------------------------------------------------
		template<class T>
		void copy_memory(const T* Source, T* Destination, size_t Size)
		{
			if (Source != nullptr && Destination != nullptr)
			{
				memcpy(Destination, Source, Size);
			}
		}
		//-------------------------------------------------------------------------------
		template<class T>
		void fill_memory(T* Destination, T Filler, size_t Size)
		{
			if (Destination != nullptr)
			{
				for (size_t i = 0; i < Size; i++)Destination[i] = Filler;
			}
		}
		//-------------------------------------------------------------------------------
		template<class T>
		std::pair<T*, uint32_t>operator+(std::pair<T*, uint32_t>a, std::pair<T*, uint32_t>b)
		{
			uint32_t Length = a.second + b.second;
			T* Sum = allocate_memory<T>(Length);
			for (uint32_t i = 0; i < a.second; i++)Sum[i] = a.first[i];
			for (uint32_t i = 0; i < b.second; i++)Sum[a.second+i] = b.first[i];
			return std::make_pair(Sum, Length);
		}
		/**
		 * @brief Alloca dinamicamente uno spazio di memoria e lo azzera.
		 * @details Alloca un array di uint8_t di dimensione (Space + 1) byte e inizializza a zero tutta la memoria allocata.
		 *
		 * @tparam T Tipo del parametro che indica la quantità di spazio da allocare.
		 * @param[in] Space Numero di byte da allocare (più uno).
		 * @returns Puntatore all’area di memoria allocata (da liberare con deallocate_space).
		 *
		 * @author Blas de Lezo
		 * @date 2025
		template<class T>
		uint8_t* allocate_space(T Space)
		{
			// Alloca lo spazio di memoria
			uint8_t* Vector = new uint8_t[Space + 1];
			// Azzera lo spazio
			memset(Vector, 0, Space + 1);
			// Restituisci il puntatore
			return Vector;
		}
		 */
		//-------------------------------------------------
		/**
		 * @brief Dealloca lo spazio di memoria precedentemente allocato.
		 * @details Libera la memoria puntata da Vector e imposta il puntatore a nullptr.
		 *
		 * @tparam T Tipo del puntatore.
		 * @param[in,out] Vector Puntatore allo spazio di memoria da deallocare.
		 *
		 * @author Blas de Lezo
		 * @date 2025
		template<class T>
		void deallocate_space(T* Vector)
		{
			if (Vector != nullptr)
			{
				delete[] Vector;
				Vector = nullptr;
			}
		}
		 */
		//-------------------------------------------------
		/**
		 * @brief Copia un blocco di memoria da una sorgente a una destinazione.
		 * @details Copia Size byte da Source a Destination se entrambi i puntatori sono validi.
		 *
		 * @tparam T Tipo degli array da copiare.
		 * @param[in] Source Puntatore alla memoria sorgente.
		 * @param[out] Destination Puntatore alla memoria di destinazione.
		 * @param[in] Size Numero di byte da copiare.
		 *
		 * @author Blas de Lezo
		 * @date 2025
		template<class T>
		void copy_space(const T* Source, T* Destination, size_t Size)
		{
			if (Source != nullptr && Destination != nullptr)
			{
				memcpy(Destination, Source, Size);
			}
		}
		 */
	}
}

