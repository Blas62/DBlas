//***************************************************************************************
// Pragma
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include <cstdint>
#include <utility>
#include <exception>
#include "DError.h"
//---------------------------------------------------------------------------------------
// Namespace
namespace blas::utility 
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
	//-------------------------------------------------------------------------------------
	template<class T>
	class DMemory
	{
	public:
		DMemory()
			{

			}
		//---------------------------------------------------------------------------
		~DMemory()
			{

			}
		//---------------------------------------------------------------------------
		static std::pair<T*, uint32_t>allocate(uint32_t Length)
			{
				try
				{
					// Alloca lo spazio di memoria
					T* Vector = new T[Length * sizeof(T) + 1];

					// Azzera lo spazio allocato
					memset(Vector, 0, (Length + 1) * sizeof(T));

					// Restituisce il risultato
					return std::make_pair(Vector, Length);
				}
				catch (std::exception& e)
				{
					ErrorMessage.set_error(STDERROR, e.what());
					return std::make_pair(nullptr, 0);
				}
			}
		//---------------------------------------------------------------------------
		static void copy(T* Source, std::pair<T*, uint32_t>& Dest, uint32_t Length)
			{
				try
				{
					if (Source==nullptr)throw std::invalid_argument("invalid pointer");
					else
					{
						if (Dest.first != nullptr)free(Dest);
						Dest = allocate(Length);
						for (uint32_t i = 0; i < Length; i++)
							Dest.first[i] = Source[i];
					}
				}
				catch (std::exception& e)
				{
					ErrorMessage.set_error(STDERROR, e.what());
				}

			}
		//---------------------------------------------------------------------------
		static void copy(std::pair<T*, uint32_t>Source, std::pair<T*, uint32_t>& Dest)
		{
			copy_from_until(Source, Dest, 0, Source.second);
		}
		//---------------------------------------------------------------------------
		static void copy_from(std::pair<T*, uint32_t>Source,
			std::pair<T*, uint32_t>& Dest, uint32_t First)
		{
			copy_from_until(Source, Dest, First, Source.second-First);
		}
		//---------------------------------------------------------------------------
		static void copy_from_until(std::pair<T*, uint32_t>Source,
			std::pair<T*, uint32_t>& Dest, uint32_t First, uint32_t Length)
		{
			try
			{
				if (First + Length > Source.second)throw std::invalid_argument("outside source length");
				else
				{
					Dest = allocate(Length);
					for (uint32_t i = 0; i < Length; i++)
						Dest.first[i] = Source.first[i + First];
				}
			}
			catch (std::exception& e)
			{
				ErrorMessage.set_error(STDERROR, e.what());
			}
		}
		//---------------------------------------------------------------------------
		static void copy_until(std::pair<T*, uint32_t>Source,
			std::pair<T*, uint32_t>& Dest, uint32_t Length)
		{
			copy_from_until(Source, Dest, 0, Length);
		}
		//---------------------------------------------------------------------------
		static void free(std::pair<T*, uint32_t>&Array)
		{
			try
			{
				if (Array.first == nullptr)throw std::invalid_argument("invalid pointer");
				delete[]Array.first;
				Array.first = nullptr;
				Array.second = 0;
			}
			catch (std::exception& e)
			{
				ErrorMessage.set_error(STDERROR, e.what());
			}
		}
		//---------------------------------------------------------------------------
		static std::string get_error_message()
			{
				return ErrorMessage.get_error_message();
			}
		//---------------------------------------------------------------------------
		static std::pair<T*, uint32_t>get_from_until(std::pair<T*, uint32_t> Array,
			uint32_t First, uint32_t Length)
		{
			try
			{
				if (First + Length > Array.second)
					throw std::invalid_argument("outside destination length");
				else
				{
					std::pair<T*, uint32_t>Selection = DMemory<T>::allocate(Length);
					for (uint32_t i = 0; i < Length; i++)
						Selection.first[i] = Array.first[First + i];
					return Selection;
				}
			}
			catch (std::exception& e)
			{
				ErrorMessage.set_error(STDERROR, e.what());
			}

		}
		//---------------------------------------------------------------------------
		static std::pair<T*, uint32_t>get_from(std::pair<T*, uint32_t> Array,
			uint32_t First)
		{
			return get_from_until(Array, First, Array.second - First);
		}
		//---------------------------------------------------------------------------
		static std::pair<T*, uint32_t>get_until(std::pair<T*, uint32_t> Array,
			uint32_t Length)
		{
			return get_from_until(Array, 0, Length);
		}
		//---------------------------------------------------------------------------
		static std::pair<T*, uint32_t>merge(std::pair<T*, uint32_t>Array1,
			std::pair<T*, uint32_t>Array2)
		{
			try
			{
				uint32_t Length = Array1.second + Array2.second;
				std::pair<T*, uint32_t>Merged = DMemory<T>::allocate(Length);
				for (uint32_t i = 0; i < Array1.second; i++)Merged.first[i] = Array1.first[i];
				for (uint32_t i = Array1.second; i < Length; i++)Merged.first[i] = Array2.first[i - Array1.second];
				return Merged;
			}
			catch (std::exception& e)
			{
				ErrorMessage.set_error(STDERROR,e.what());
			}
		}
		//---------------------------------------------------------------------------
		static void set(std::pair<T*, uint32_t>& Array, T Element)
		{
			set_from_until(Array, Element, 0, Array.second);
		}
		//---------------------------------------------------------------------------
		static void set_from(std::pair<T*, uint32_t>& Array, T Element,
			uint32_t First)
		{
			set_from_until(Array, Element, First, Array.second - First);
		}
		//---------------------------------------------------------------------------
		static void set_from_until(std::pair<T*, uint32_t>&Array, T Element, 
			uint32_t First, uint32_t Length)
			{
			try
			{
				if (First + Length > Array.second)
					throw std::invalid_argument("outside destination length");
				else
				{
					for (uint32_t i = 0; i < Length; i++)
						Array.first[First + i] = Element;
				}
			}
			catch (std::exception& e)
			{
				ErrorMessage.set_error(STDERROR, e.what());
			}
		}
		//---------------------------------------------------------------------------
		static void set_until(std::pair<T*, uint32_t>& Array, T Element,
			uint32_t Length)
		{
			set_from_until(Array, Element, 0, Length);
		}
		//---------------------------------------------------------------------------
	protected:
	private:
		static DError ErrorMessage;
	};

	template<class T>
	DError DMemory<T>::ErrorMessage;

}


