//***************************************************************************************
// Include
#include "pch.h"
#include "DCryptoAttack.h"
#include "DCryptoAnalysis.h"
//---------------------------------------------------------------------------------------
// Define
#define DEBUG
// Namespace
using namespace std;
using namespace blas::utility;
using namespace blas::utility::tcp::v4;
using namespace blas::cryptography;
using namespace blas::cryptography::analysis;
//---------------------------------------------------------------------------------------
tuple<uint8_t*, uint8_t, double> attack::DCryptoAttack::brute_force_single_character_key_xor(
    const pair<uint8_t*, uint32_t> Cipher, const double* Language)
{
    try
    {
		// Controlla se il testo è nullo, ha lunghezza zero o se la lingua è nulla
		if ((Cipher.first == nullptr) || (Cipher.second == 0) || (Language == nullptr))
			throw invalid_argument("Text is null or has zero length or no language.");

		// Dichiara l'oggetto per il risultato dell'operazione XOR
		pair <uint8_t*, uint32_t> Plain{ nullptr,0 };

		// Dichiara la chiave
		uint8_t Key = 0;

		// Dichiara il punteggio corrente
		double Score = 0.0;

		// Dichiara la chiave migliore
		uint8_t BestKey = 0;

		// Dichiara il punteggio migliore
		double BestScore = 1000.0;

		// Dichiara il testo in chiaro migliore
		uint8_t* BestPlain = allocate_memory<uint8_t>(Cipher.second);

		// Dichiara l'oggetto per l'operazione XOR
		fundamental_algorithms::DXOROperation Operation;

		// Dichiara l'oggetto per l'analisi del testo
		analysis::DCryptoAnalysis Analysis;

		// Cicla tutte le chiavi possibili
		for (uint16_t i = 0; i < 256; i++)
		{
			// Esegue l'operazione di XOR tra il testo cifrato e la chiave corrente
			Plain = Operation.XOR(Cipher, Key);

			// Calcola il punteggio della chiave corrente
			Score = Analysis.get_language_score(Plain, Language);

			// Se il punteggio è migliore del migliore trovato finora
			if (Key == 0 || Score < BestScore)
			{
				// Aggiorna il punteggio migliore
				BestScore = Score;

				// Aggiorna la chiave migliore
				BestKey = Key;

				// Copia il testo in chiaro corrente nel migliore trovato finora
				copy_memory<uint8_t>(Plain.first, BestPlain, Plain.second);
			}
			
			// Incrementa la chiave
			if(Key < 255) Key++;

			// Dealloca la memoria del testo in chiaro corrente e azzera la lunghezza
			// per evitare perdite di memoria
			if (Plain.first != nullptr)
			{
				deallocate_memory(Plain.first);
				Plain.first = nullptr;
				Plain.second = 0;
			}
		}
		// Restituisce il testo in chiaro migliore, la chiave migliore e il punteggio migliore
		return make_tuple(BestPlain, BestKey, BestScore);
    }
	catch (const std::exception& e)
	{
		set_error(STDERROR, e.what());
		return make_tuple(nullptr, 0, 0.0);
	}
}
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> attack::DCryptoAttack::byte_at_a_time_to_block_cipher(string IPAddress, uint16_t Port, uint32_t BlockLength)
{
	// Dichiara l'oggetto per le conversioni
	DFormatConverter Converter;

	// Dichiara l'oggetto per l'analisi
	DCryptoAnalysis Analyzer;

	// Dichiata l'oggetto per interrogare il server
	DTCPv4Client Client;

	// Determina la lunghezza del testo
	uint32_t Length = Analyzer.get_cipher_cookie_length(IPAddress, Port, BlockLength);

	// Calcola il numero di blocchi
	uint32_t BlockNo = Length / BlockLength;

	// Calcola gli indici per il confronto
	uint32_t Index1 = (BlockNo - 1) * BlockLength;
	uint32_t Index2 = (2 * BlockNo - 1) * BlockLength;

	// Dichiara e inizializza il vettore del testo in chiaro
	std::pair<uint8_t*, uint32_t>Plain;
	Plain.first = allocate_memory<uint8_t>(Length);
	Plain.second = Length;
	// Il vettore è riempito temporaneamente con il carattere A
	memset(Plain.first, 'A', Length);

	// Dichiara e inizializza il vettore della sonda
	std::pair<uint8_t*, uint32_t>Probe;
	Probe.second = Length - 1;
	Probe.first = allocate_memory<uint8_t>(Probe.second);
	// Il vettore è riempito temporaneamente con il carattere A
	memset(Probe.first, 'A', Probe.second);

	// Ciclo principale
	for (uint32_t i = 0; i < Length; i++)
	{
		// Dichiara e inizializza un carattere
		uint8_t Char = 0;

		// Ciclo di confronto per scoprire il carattere valido
		for (uint32_t j = 0; j < 256; j++)
		{
			// Imposta il testo in chiaro con il carattere da confrontare
			Plain.first[Length - 1] = Char;

			// Genera il messaggio
			std::pair<uint8_t*, uint32_t> Message = Plain + Probe;

			// Converte il messaggio in base64
			string CodedMessage = Converter.binary_to_base64(Message).first;

			// Attiva la connessione
			Client.connect(IPAddress, Port);

			// Invia il messaggio
			Client.send(CodedMessage);

			// Legge la risposta
			string CodedAnswer = Client.read_until_close();

			// Converte la risposta
			std::pair<uint8_t*, uint32_t> Answer = Converter.base64_to_binary(CodedAnswer);

			// Seleziona i blocchi da confrontare
			std::pair<uint8_t*, uint32_t> PlainBlock = make_pair(&Answer.first[Index1], BlockLength);
			std::pair<uint8_t*, uint32_t> ProbeBlock = make_pair(&Answer.first[Index2], BlockLength);

			// Calcola la distanza di Hamming tra i due blocchi
			if (!Analyzer.get_Hamming_distance(PlainBlock, ProbeBlock))
			{
				// Libera la memoria
				deallocate_memory(Message.first);
				deallocate_memory(Answer.first);

				// Esce dal ciclo
				break;
			}
			else
			{
				// Libera la memoria
				deallocate_memory(Message.first);
				deallocate_memory(Answer.first);

				// Incrementa il carattere
				Char++;
			}
		}

		// Se è stato scoperto l'ultimo carattere
		if (i < (Length - 1))
		{
			// Scorre il chiaro di un posto
			for (uint32_t i = 0; i < Plain.second - 1; i++)Plain.first[i] = Plain.first[i + 1];

			// Accorcia la sonda di un elemento
			Probe.first[Probe.second - 1] = 0;
			Probe.second--;
		}
	}

	// Libera la memoria
	deallocate_memory(Probe.first);

	// Restituisce il decrittato
	return Plain;

	// TODO: rendere completo l'attacco ponendo le prime due fasi di analisi all'interno 
	// di questa funzione
	
/*

	// Ciclo principale
	for (uint32_t i = 0; i < Length; i++)
	{
		uint8_t Char = 0;
		for (uint32_t j = 0; j < 256; j++)
		{
			// Assegna il carattere da confrontare
			Plain.first[Length - 1] = Char;

			// Genera il messaggio
			std::pair<uint8_t*, uint32_t> Message = Plain + Probe + Test;

			// Invia il messaggio e ricevi la risposta 
#ifdef DEBUG
			std::pair<uint8_t*, uint32_t> Answer = Message + Test;
			std::pair<uint8_t*, uint32_t> PlainBlock = make_pair(&Answer.first[Index1], BlockLength);
			std::pair<uint8_t*, uint32_t> ProbeBlock = make_pair(&Answer.first[Index2], BlockLength);
#endif
			if (!Analyzer.get_Hamming_distance(PlainBlock,ProbeBlock))break;
			Char++;
		}
		if (i < (Length - 1))
		{
			// Scorre il chiaro di un posto
			Plain = shift_to_left(Plain);

			// Accorcia la sonda di un elemento
			Probe.first[Probe.second - 1] = 0;
			Probe.second--;
		}
	}

	// Restituisce il decrittato
	return Plain;
*/
}
//---------------------------------------------------------------------------------------
/*
pair<uint8_t*, uint32_t> attack::DCryptoAttack::copy_to_probe(pair<uint8_t*, uint32_t> Array1, pair<uint8_t*, uint32_t> Array2)
{
	for (uint32_t i = 0; i < Array1.second - 1; i++)Array1.first[i] = Array2.first[i + 1];
	Array1.first[Array1.second - 1] = 0;
	Array1.second--;
	return Array1;
}
*/
//---------------------------------------------------------------------------------------
/*
uint32_t attack::DCryptoAttack::get_cookie_length(string IPAddress, uint16_t Port, uint32_t BlockLength)
{
	// Dichiara un oggetto per la conversione di formati
	DFormatConverter Converter;

	// Dichiara e alloca la memoria per la sonda
	pair<uint8_t*, uint32_t>Probe;
	Probe.first = allocate_memory<uint8_t>(BlockLength);
	Probe.second = BlockLength;

	// Riempie la sonda con il carattere A
	memset(Probe.first, 'A', BlockLength);

	// Converte il contenuto della sonda in base64 URL
	std::string Request = Converter.binary_to_base64(Probe, true, true).first;

	// Dichiara e inizializza il client 
	DTCPv4Client Client;

	// Attiva la connessione
	Client.connect(IPAddress, Port);

	// Invia la richiesta al server
	Client.send(Request);

	// Riceve la risposta
	string Answer = Client.read_until_close();

	// Converte la risposta in binario
	pair<uint8_t*, uint32_t> Cipher = Converter.base64_to_binary(Answer, true, true);

	return Cipher.second - BlockLength;
}
*/
//---------------------------------------------------------------------------------------
/*
pair<uint8_t*, uint32_t> attack::DCryptoAttack::shift_to_left(pair<uint8_t*, uint32_t> Array)
{
	// Sposta i byte del risultato di un posto
	for (uint32_t i = 0; i < Array.second - 1; i++)Array.first[i] = Array.first[i + 1];
	return Array;
}
*/    