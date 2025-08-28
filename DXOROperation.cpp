//***************************************************************************************
#include "pch.h"
#include "DXOROperation.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace std;
using namespace std::filesystem;
using namespace blas::utility;
using namespace blas::cryptography::fundamental_algorithms;
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> DXOROperation::XOR(const path Cipher, const path Plain, 
	const pair<uint8_t*, uint32_t> Key)
{
	try
	{
		// Verifica che il file di input esista e sia accessibile
		if (!exists(Cipher))throw runtime_error("The input file does not exist");

		// Verifica il file di input non si avuoto
		if (Cipher.empty())throw runtime_error("The input file name is empty");

		// Apre il file di input
		ifstream In(Cipher, ios::in | ios::binary);

		// Apre il file di output
		ofstream Out(Plain, ios::out | ios::binary | ios::trunc);

		// Verifica che il file di output sia stato aperto correttamente
		if (!Out.is_open())throw runtime_error("Cannot open the output file");

		// Dichiara la variabile dove copiare quello letto dal file
		uint8_t c;

		//  Dichiara il contatore per la chiave
		uint32_t i = 0;

		// Ciclo principale
		while (In.get((char&)c))
		{
			// Determina l'indica della chiave da usare
			i = i % Key.second;

			// Esegue l'operazione di XOR tra il byte letto e il byte della chiave
			c = c ^ Key.first[i];

			// Scrive il byte cifrato nel file di output
			Out.put(c);

			// Incrementa l'indice della chiave
			i++;
		}
		
		// Chiude i file di input e output
		In.close();
		Out.close();

		// Restituisce una coppia <tt>{nullptr, 0}</tt> se l'operazione ha avuto successo
		return make_pair(nullptr, 0); 	
	}
	catch (const std::exception& e)
	{

		// Gestisce le eccezioni e imposta l'errore
		set_error(STDERROR, e.what());

		// Restituisce il codice d'errore
		return make_pair((uint8_t*)get_error_message().c_str(), STDERROR);
	}
	return std::pair<uint8_t*, uint32_t>();
}
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> DXOROperation::XOR(const pair<uint8_t*, uint32_t> Text,
	const uint8_t Key)
{
	try
	{
		if((Text.first==nullptr)||(Text.second==0))	
			throw invalid_argument("Text is null or has zero length.");
		else
		{
			// Dichiara e alloca il buffer di output
			uint32_t Length = Text.second;
			uint8_t* Result = allocate_memory<uint8_t>(Length);

			// Esegue l'operazione di XOR tra il testo e la chiave
			for (uint32_t i = 0; i < Length; ++i) Result[i] = Text.first[i] ^ Key;

			// Restituisce il risultato come coppia di puntatore e lunghezza
			return make_pair(Result, Length);
		}
	}
	catch (const std::exception& e)
	{
		// Gestisce le eccezioni e imposta l'errore
		set_error(STDERROR, e.what());

		// Restituisce un puntatore nullo e lunghezza zero in caso di errore
		return make_pair(nullptr, 0);
	}
}
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> DXOROperation::XOR(const pair<uint8_t*, uint32_t> Text,
	const pair<uint8_t*, uint32_t> Key)
{
	try
	{
		if ((Text.first == nullptr) || (Text.second == 0))
			throw invalid_argument("Text is null or has zero length.");

		// Dichiara e alloca il buffer di output
		uint32_t Length = Text.second;
		uint8_t* Result = allocate_memory<uint8_t>(Length);

		// Esegue l'operazione di XOR tra il testo e la chiave
		for (uint32_t i = 0; i < Length; ++i) Result[i] = Text.first[i] ^ Key.first[i % Key.second];

		// Restituisce il risultato come coppia di puntatore e lunghezza
		return make_pair(Result, Length);
	}
	catch (const std::exception& e)
	{
		// Gestisce le eccezioni e imposta l'errore
		set_error(STDERROR, e.what());

		// Restituisce un puntatore nullo e lunghezza zero in caso di errore
		return make_pair(nullptr, 0);
	}
}

