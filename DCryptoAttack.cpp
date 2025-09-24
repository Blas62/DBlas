//***************************************************************************************
// Include
#include "pch.h"
#include "DCryptoAttack.h"
#include "DCryptoAnalysis.h"
//---------------------------------------------------------------------------------------
// Define
//#define DEBUG
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
	memset(Plain.first, '.', Length);

	// Dichiara e inizializza il vettore della sonda
	std::pair<uint8_t*, uint32_t>Probe;
	Probe.second = Length - 1;
	Probe.first = allocate_memory<uint8_t>(Probe.second);
	// Il vettore è riempito temporaneamente con il carattere A
	memset(Probe.first, '.', Probe.second);

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
			string CodedMessage = Converter.binary_to_base64(Message,true,true).first;

			// Attiva la connessione
			Client.connect(IPAddress, Port);

			// Invia il messaggio
			Client.send(CodedMessage);

			// Legge la risposta
			string CodedAnswer = Client.read_until_close();

			// Converte la risposta
			std::pair<uint8_t*, uint32_t> Answer = Converter.base64_to_binary(CodedAnswer,true,true);

			// Seleziona i blocchi da confrontare
			std::pair<uint8_t*, uint32_t> PlainBlock = make_pair(&Answer.first[Index1], BlockLength);
			std::pair<uint8_t*, uint32_t> ProbeBlock = make_pair(&Answer.first[Index2], BlockLength);

			// Calcola la distanza di Hamming tra i due blocchi
			if (!Analyzer.get_Hamming_distance(PlainBlock, ProbeBlock))
			{
				// Libera la memoria
				deallocate_memory(Message.first);
				deallocate_memory(Answer.first);
				cout << Char;
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
			//for (uint32_t i = 0; i < Plain.second - 1; i++)Plain.first[i] = Plain.first[i + 1];
			memmove(Plain.first, Plain.first + 1, Plain.second - 1);

			// Accorcia la sonda di un elemento
			Probe.first[Probe.second - 1] = 0;
			Probe.second--;
		}
	}

	// Libera la memoria
	deallocate_memory(Probe.first);
	/*
	// Elimina il padding
	uint32_t Shift = Plain.second - PlainLength - 1;
	memmove(Plain.first, Plain.first + Shift, PlainLength);
	Plain.first[PlainLength] = 0;
	Plain.second = PlainLength;
	*/
	// Restituisce il decrittato
	return Plain;

	// TODO: rendere completo l'attacco ponendo le prime due fasi di analisi all'interno 
	// di questa funzione
}
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> attack::DCryptoAttack::byte_at_a_time_to_block_cipher_with_prefix(string IPAddress, uint16_t Port, uint32_t BlockLength)
{
	// Dichiara gli oggetti per la conversione di formati, l'analisi e la trasmissione dei dati
	DFormatConverter Converter;
	analysis::DCryptoAnalysis Analyzer;
	DTCPv4Client Client;

	// Dichiara la variabile per lunghezza del prefisso
	uint32_t PrefixLength;

	// Dichiara la variabile per la lunghezza del testo in chiaro
	uint32_t PlainLength;

	// Dichiara la variabile contenente il testo in chiaro
	pair<uint8_t*, uint32_t>Plain;

	// Dichiara la variabile contenente la sonda
	pair<uint8_t*, uint32_t>Probe;

	// Dichiara la variabile contenente la stringa di riepimento del prefisso
	pair<uint8_t*, uint32_t>Fill;

	// Determina la lunghezza del prefisso e del cookie in chiaro
	tie(PrefixLength, PlainLength) = Analyzer.get_cipher_cookie_and_prefix_length(IPAddress, Port, BlockLength);

	// Calcola la lunghezza del riempimento del blocco del prefisso
	Fill.second = BlockLength - PrefixLength % BlockLength;

	// Alloca la memoria per il riempimento e lo riempie con'+'
	Fill.first = allocate_memory<uint8_t>(Fill.second);
	memset(Fill.first, '+', Fill.second);

	// Dichiara e calcola il numero complessivo di blocchi del prefisso più il riempimento
	uint32_t PrefixBlockNo = (Fill.second + PrefixLength) / BlockLength;

	// Calcola il numero di blocchi del testo in chiaro
	uint32_t PlainBlockNo = PlainLength / BlockLength;
	if (PlainLength % BlockLength)PlainBlockNo++;

	// Calcola lo spazio necessario per il testo in chiaro più padding
	Plain.second = PlainBlockNo * BlockLength;

	// Alloca la memoria per il testo in chiaro e lo riempie con '.'
	Plain.first = allocate_memory<uint8_t>(Plain.second);
	memset(Plain.first, '.', Plain.second);

	// Calcola lo spazio necessario per la sonda
	Probe.second = Plain.second - 1;

	// Alloca la memoria per la sonda e lo riempie con '.'
	Probe.first = allocate_memory<uint8_t>(Probe.second);
	memset(Probe.first, '.', Probe.second);

	// Dichiara e calcola gli indici da confrontare
	uint32_t Index1 = (PlainBlockNo + PrefixBlockNo - 1) * BlockLength;
	uint32_t Index2 = (2 * PlainBlockNo + PrefixBlockNo - 1) * BlockLength;

	// Ciclo principale
	for (uint32_t i = 0; i < PlainLength; i++)
	{
		// Dichiara e inizializza un carattere
		uint8_t Char = 0;

		// Dichiara la variabile per il messaggio
		std::pair<uint8_t*, uint32_t> Message;

		// Dichiara la variabile per il messaggio in base64
		string CodedMessage;

		// Dichiara la variabile per la risposta base64
		string CodedAnswer;

		// Dichiara la variabile per la risposta decodificata
		std::pair<uint8_t*, uint32_t> Answer;

		// Dichiara i due blocchi per il confronto
		std::pair<uint8_t*, uint32_t> PlainBlock;
		std::pair<uint8_t*, uint32_t> ProbeBlock;

		// Ciclo di confronto per scoprire il carattere valido
		for (uint32_t j = 0; j < 256; j++)
		{
			// Imposta il testo in chiaro con il carattere da confrontare
			Plain.first[Plain.second - 1] = Char;

			// Genera il messaggio
			Message = Fill + Plain + Probe;

			// Converte il messaggio in base64
			CodedMessage = Converter.binary_to_base64(Message, true, true).first;

			// Attiva la connessione
			Client.connect(IPAddress, Port);

			// Invia il messaggio
			Client.send(CodedMessage);

			// Legge la risposta
			CodedAnswer = Client.read_until_close();

			// Converte la risposta
			Answer = Converter.base64_to_binary(CodedAnswer, true, true);

			// Seleziona i blocchi da confrontare
			PlainBlock = make_pair(&Answer.first[Index1], BlockLength);
			ProbeBlock = make_pair(&Answer.first[Index2], BlockLength);

			// Calcola la distanza di Hamming tra i due blocchi
			if (!Analyzer.get_Hamming_distance(PlainBlock, ProbeBlock))
			{
				// Libera la memoria
				deallocate_memory(Message.first);
				deallocate_memory(Answer.first);

				// Stampa il carattere trovato
				cout << Char;

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
		// Se è non stato scoperto l'ultimo carattere
		if (i < (Plain.second - 1))
		{
			// Scorre il chiaro di un posto
			//for (uint32_t i = 0; i < Plain.second - 1; i++)Plain.first[i] = Plain.first[i + 1];
			memmove(Plain.first, Plain.first + 1, Plain.second - 1);

			// Accorcia la sonda di un elemento
			Probe.first[Probe.second - 1] = 0;
			Probe.second--;
		}
	}
	// Libera la memoria
	deallocate_memory(Probe.first);

	// Elimina il padding
	uint32_t Shift = Plain.second - PlainLength - 1;
	memmove(Plain.first, Plain.first + Shift, PlainLength);
	Plain.first[PlainLength] = 0;
	Plain.second = PlainLength;

	// Restituisce il decrittato
	return Plain;
}
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> attack::DCryptoAttack::CBC_padding_oracle(string IPAddress, 
	uint16_t Port, pair<uint8_t*, uint32_t> Cookie,
	uint32_t BlockLength)
{
	// Dichiara il vettore del testo in chiaro
	pair<uint8_t*, uint32_t>Plain;

	// Inizializza l'oggetto Json
	data_format::DJson JsonCookie; 
	JsonCookie.set((char*)Cookie.first);

	// ottiene l'indice del cookie
	uint32_t Index = JsonCookie.get<uint32_t>("index");

	// Ottiene il cifrato base64
	string Base64Cookie = JsonCookie.get<std::string>("cookie");

	// Converte il cookie da base64 a binario
	DFormatConverter Converter;
	pair<uint8_t*, uint32_t>CipherCookie = Converter.base64_to_binary(Base64Cookie, true, true);

	// Separa il vettore iniziale dal cifrato
	pair<uint8_t*, uint32_t>InitialVector = DMemory<uint8_t>::get_until(CipherCookie, BlockLength);
	pair<uint8_t*, uint32_t>Cipher = DMemory<uint8_t>::get_from(CipherCookie, BlockLength);


	// Calcola il numero dei blocchi e dichiara l'array dove mettere il singolo blocco cifrato
	uint32_t BlockNo = Cipher.second / BlockLength;
	pair<uint8_t*, uint32_t>CipherBlock;

	// Ciclo principale
	for (uint32_t i = 0; i < BlockNo; i++)
	{
		// Estrae un blocco dal cifrato
		CipherBlock = DMemory<uint8_t>::get_from_until(Cipher, i * BlockLength, BlockLength);

		// Decripta il blocco e lo unisce all'array in chiaro mettendolo in un array temporaneo
		pair<uint8_t*, uint32_t>Temp=blas::utility::DMemory<uint8_t>::merge(Plain, block_CBC_padding_oracle(IPAddress, Port, CipherBlock, Index, BlockLength));

		// Copia il contenuto dell'array temporaneo nell'array in chiaro
		DMemory<uint8_t>::copy(Temp,Plain);

		// Libera la memoria
		DMemory<uint8_t>::free(Temp);
		DMemory<uint8_t>::free(CipherBlock);
	}

	// Restituisce il risultato
	return Plain;
}
//--------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> attack::DCryptoAttack::block_CBC_padding_oracle(string IPAddress, 
	uint16_t Port, 
	pair<uint8_t*, uint32_t> CipherBlock, 
	uint32_t Index, 
	uint32_t BlockLength)
{
	// Dichiara il client
	tcp::v4::DTCPv4Client Client;

	// Alloca lo spazio per il blocco in chiaro
	pair<uint8_t*, uint32_t>PlainBlock = DMemory<uint8_t>::allocate(BlockLength);

	// Alloca lo spazio per il vettore iniziale di test
	pair<uint8_t*, uint32_t>InitialVector = DMemory<uint8_t>::allocate(BlockLength);

	// Crea il json per la richiesta
	blas::utility::data_format::DJson JsonRequest;
	JsonRequest.set("index", Index);

	// Ciclo principale di prova
	for (uint32_t i = 0; i < 256; i++)
	{
		// Inizializza l'ultimo carattere del vettore iniziale
		InitialVector.first[BlockLength - 1] = (uint8_t)i;

		// Unisce il vettore iniziale al blocco da decrittare
		pair<uint8_t*, uint32_t>Cipher = DMemory<uint8_t>::merge(InitialVector, CipherBlock);

		// Converte i vettore ottenuto in base 64
		DFormatConverter Converter;
		std::string CodedCipher = Converter.binary_to_base64(Cipher, true, true).first;

		// Inserisce il risultato nel JSON
		JsonRequest.set("cookie", CodedCipher);

		// Converte il jason in una stringa
		string Request = JsonRequest.get_object();

		// Converte la stringa in base64URL e la restituisce 
		pair<char*, uint32_t>Message = Converter.binary_to_base64(Request, true, true);

		// Invia la richiesta al server alla porta dell'indirizzo IP del server
		Client.connect(IPAddress, Port);
		Client.send(Message.first);
		string Answer = Client.read_until_close();


	}










	DMemory<uint8_t>::set(PlainBlock, 'A');
	return PlainBlock;
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