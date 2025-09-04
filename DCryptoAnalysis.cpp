//***************************************************************************************
// Include
#include "pch.h"
#include "DCryptoAnalysis.h"
#include "DUtility.h"
#include "DCryptoAttack.h"
//---------------------------------------------------------------------------------------
// Define
#define MAXBLOCKLENGTH 128
#define MAXANSWERLENGTH 4096
//#define CHALLENGE_12
//---------------------------------------------------------------------------------------
// Namespace
using namespace std;
using namespace std::filesystem;
using namespace blas::utility;
using namespace blas::cryptography::analysis;
using namespace blas::cryptography::attack;
using namespace blas::cryptography::cipher;
using namespace blas::utility::tcp::v4;
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> DCryptoAnalysis::compute_XOR_best_key(path Text, 
	const uint32_t KeySize, const double* Language)
{
	try
	{
		// Verifica che il file di input esista e sia accessibile
		if (!exists(Text))throw runtime_error("The input file does not exist");

		// Verifica il file di input non si avuoto
		if (Text.empty())throw runtime_error("The input file name is empty");

		// Dichiara la dimensione del file
		uint32_t FileSize = (uint32_t)file_size(Text);

		// Dichiara la dimensione del campione e assegna il valore
		uint32_t SampleSize = FileSize / KeySize + 1;

		// Dichiara un puntatore per il campione e alloca la memoria
		uint8_t* Sample = blas::utility::allocate_memory<uint8_t>(SampleSize);

		// Dichiara un puntatore per la chiave e alloca la memoria
		uint8_t* Key = blas::utility::allocate_memory<uint8_t>(KeySize);

		// Dichiara un contatore per il campione
		uint32_t Count;

		// Dichiara un oggetto per l'attacco a forza bruta del cifrario XOR
		DCryptoAttack Attack;

		// Apre il file di input
		ifstream In(Text, ios::in | ios::binary);

		// Ciclo principale 
		for (uint32_t i = 0; i < KeySize; i++)
		{
			// Inizializza il contatore
			Count = 0;

			// Resetta il file stream al punto di partenza
			In.clear();

			// Calcola il punto di partenza per il campione
			In.seekg(i, ios::beg);

			// Legge il campione dal file
			while (In.good())
			{
				// Legge un byte dal file e lo assegna al campione
				Sample[Count] = In.get();

				// Se il byte letto è EOF, esce dal ciclo
				if (In.eof()) break;

				// Sposta il file stream al prossimo byte da leggere
				In.seekg(KeySize - 1, ios::cur);

				// Incrementa il contatore
				Count++;
			}

			// Assegna il carattere della chiave
			tie(ignore, Key[i], ignore) = Attack.brute_force_single_character_key_xor(make_pair(Sample, SampleSize), Language);

		}
		// Chiude il file
		In.close();

		// Restituisce la chiave
		return make_pair(Key,KeySize);
	}
	catch (const exception& e)
	{
		set_error(STDERROR, e.what());
		return make_pair((uint8_t*)get_error_message().c_str(), STDERROR);
	}
	/*

		// Dichiara un oggetto di tipo DAnalysisTools per l'analisi
		DAnalysisTools Analysis;

		//-------------------------------------------------------
		// Ciclo principale 
		for (uint32_t i = 0; i < KeySize; i++)
		{
			// Inizializza il contatore
			Count = 0;

			// Resetta il file stream al punto di partenza
			In.clear();

			// Calcola il punto di partenza per il campione
			In.seekg(i, ios::beg);

			// Legge il campione dal file
			while (In.good())
			{
				// Legge un byte dal file e lo assegna al campione
				Sample[Count] = In.get();

				// Se il byte letto è EOF, esce dal ciclo
				if (In.eof()) break;

				// Sposta il file stream al prossimo byte da leggere
				In.seekg(KeySize - 1, ios::cur);

				// Incrementa il contatore
				Count++;
			}

			// Assegna il carattere della chiave
			Key[i] = Analysis.brute_force_attack_on_single_character_xor_key(Sample, Count, Language);

		}
		//-------------------------------------------------------
		// Chiude il file
		In.close();

		// Restituisce la chiave
		return Key;
	}
	catch (system_error& e)
	{
		set_error(e.code().value(), e.what());
		return nullptr;
	}
*/
	return std::pair<uint8_t*, uint32_t>();
}
//---------------------------------------------------------------------------------------
tuple<uint32_t, double, double> DCryptoAnalysis::compute_XOR_best_key_size(path Text, 
	const uint32_t MaxKeySize)
{
	try
	{
		// Verifica che il file di input esista e sia accessibile
		if (!exists(Text))throw runtime_error("The input file does not exist");

		// Verifica il file di input non si avuoto
		if (Text.empty())throw runtime_error("The input file name is empty");

		// Apre i file di input	
		ifstream In(Text, ios::in | ios::binary);

		// Dichiara la variabile KeySize
		uint32_t KeySize;

		// Dichiara la variabile per la distanza media 
		double AverageDistance;

		// Dichiara la variabile per la lunghezza della chiave migliore
		uint32_t BestSize = 0;

		// Dichiara e inizializza la variabile BestAverageDistance
		double BestAverageDistance = 1000.0;

		// Dichiara e inizializza la variabile Delta
		double Delta = 0.0;

		// Dichiara un contatore 
		uint32_t Count = 0;

		// Dichiara il punto di partenza
		uint32_t Start = 0;

		// Dichiara la variabile per la distanza di Hamming
		uint32_t Distance = 0;

		// Alloca due blocchi di memoria per il confronto
		uint8_t* Block1 = allocate_memory<uint8_t>(MaxKeySize);
		uint8_t* Block2 = allocate_memory<uint8_t>(MaxKeySize);

		// Ciclo principale per calcolare la migliore dimensione della chiave
		for (KeySize = 2; KeySize <= MaxKeySize; KeySize++)
		{
			// Legge il blocco da confrontare dal file
			while (In.read((char*)Block1, KeySize))
			{
				// Legge i blocchi successivi per il confronto
				while (In.read((char*)Block2, KeySize))
				{
					// incrementa il contatore
					Count++;

					// Calcola la distanza di Hamming tra i due blocchi
					Distance += get_Hamming_distance(make_pair(Block1,KeySize),make_pair(Block2,KeySize));
				}
				// Passa al blocco successivo
				Start++;

				// Resetta il file stream al punto di partenza
				In.clear();
				In.seekg(Start * KeySize, ios::beg);
			}
			// Calcola la distanza media di Hamming
			AverageDistance = (double)Distance / (double)(Count * KeySize);

			// Se la distanza media è migliore della migliore distanza media
			if (AverageDistance < BestAverageDistance)
			{
				// Aggiorna il Delta
				Delta = BestAverageDistance - AverageDistance;

				// Aggiorna la migliore distanza media
				BestAverageDistance = AverageDistance;

				// Aggiorna la migliore lunghezza della chiave
				BestSize = KeySize;
			}
			// Resetta il contatore, il punto di partenza e la distanza di Hamming
			Count = 0;
			Start = 0;
			Distance = 0;

			// Resetta il file stream al punto di partenza
			In.clear();
			In.seekg(0, ios::beg);
		}

		// Chiude il file di input
		In.close();

		// Dealloca i blocchi di memoria
		deallocate_memory<uint8_t>(Block1);
		deallocate_memory<uint8_t>(Block2);

		// Restituisce la migliore lunghezza della chiave
		return make_tuple(BestSize, BestAverageDistance, Delta); 
	}
	catch (const std::exception& e)
	{
		set_error(STDERROR, e.what());
		return make_tuple(STDERROR, 0.0, 0.0); // Restituisce un errore
	}
}
//---------------------------------------------------------------------------------------
uint32_t DCryptoAnalysis::detect_block_length(std::string IPAddress, uint16_t Port)
{
	// Dichiara e inizializza la lunghezza del blocco a MAXBLOCKLENGTH
	uint32_t Length = MAXBLOCKLENGTH;

	// Dichiara un oggetto per la conversione di formati
	DFormatConverter Converter;

	// Dichiara e alloca la memoria per la sonda
	pair<uint8_t*, uint32_t>Probe;
	Probe.first = allocate_memory<uint8_t>(Length);
	Probe.second = Length; // Assegnazione per memoria

	// Dichiara e inizializza un indice
	uint32_t Index = 0;

	// Assegna un valore alla sonda e convertilo in base64 URL
	Probe.first[Index] = 'A';
	Probe.second = Index + 1;

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
	tie(ignore, Length) = Converter.base64_to_binary(Answer);

	// Dichiara la lunghezza del messaggio dopo la ricezione della risposta
	uint32_t NewLength = 0;

	// Ciclo principale
	do
	{
		// Prepara una nuova richiesta
		Index++;

		// Assegna un valore alla sonda e convertilo in base64 URL
		Probe.first[Index] = 'A';
		Probe.second = Index + 1;

		Request = Converter.binary_to_base64(Probe, true, true).first;

		// Attiva la connessione
		Client.connect(IPAddress, Port);

		// Invia la richiesta al server
		Client.send(Request);

		// Riceve la risposta
		string Answer = Client.read_until_close();

		// Calcola la nuova lunghezza
		tie(ignore, NewLength) = Converter.base64_to_binary(Answer);

	} while (NewLength == Length);

	// Dealloca la sonda
	deallocate_memory(Probe.first);

	// Calcola la dimensione del blocco
	uint32_t BlockLength = 0;
	if (Length > NewLength)BlockLength = Length - NewLength;
	else BlockLength = NewLength - Length;

	// Restituisce la dimensione del blocco
	return BlockLength;
}
//---------------------------------------------------------------------------------------
DMode DCryptoAnalysis::detect_block_cipher_mode(path Cipher, const uint32_t BlockSize)
{
	try
	{
		// Verifica che il file di input esista e sia accessibile
		if (!exists(Cipher))throw runtime_error("The input file does not exist");

		// Verifica il file di input non si avuoto
		if (Cipher.empty())throw runtime_error("The input file name is empty");

		// Apre il file sorgente
		ifstream In(Cipher, ifstream::binary);

		// Dichiara i blocchi da confrontare
		uint8_t* Block1 = allocate_memory<uint8_t>(BlockSize);
		uint8_t* Block2 = allocate_memory<uint8_t>(BlockSize);

		// Dichiara un contatore
		uint32_t Count = 0;

		// Dichiara il punto di partenza della lettura del blocco 1
		uint32_t Start = 0;

		// Dichiara il numero di distanze pari a 0
		uint32_t Hamming = 0;

		while (In.read((char*)Block1, BlockSize))
		{
			while (In.read((char*)Block2, BlockSize))
			{
				Count++;
				if (get_Hamming_distance(make_pair(Block1, BlockSize), make_pair(Block2, BlockSize)) == 0) 
					Hamming++;
			}
			Start++;
			In.clear();
			In.seekg(Start * BlockSize, In.beg);
		}
		// Chiude il file e libera la memoria
		In.close();
		deallocate_memory(Block1);
		deallocate_memory(Block2);

		// Restituisce il risultato
		if (Hamming != 0)return ECB;
		else return CBC;
	}
	catch (const std::exception& e)
	{
		set_error(STDERROR, e.what());
		return UMD; // Restituisce un errore
	}
}
//---------------------------------------------------------------------------------------
DMode DCryptoAnalysis::detect_block_cipher_mode(const pair<uint8_t*, uint32_t> Cipher, const uint32_t BlockLength)
{
	// Dichiara e inizializza i puntatori ai blocchi da confrontare
	pair<uint8_t*, uint32_t> Block1;
	Block1.second = BlockLength;

	pair<uint8_t*, uint32_t> Block2;
	Block2.second = BlockLength;

	// Dichiara e inizializza il massimo numero di confronti pari al massimo numero di 
	// blocchi meno 2
	uint32_t BlockNo = Cipher.second / BlockLength - 2;

	// Dichiara e inizializza il contatore la distanza di Hamming pari a 0
	uint32_t Hamming = 0;

	// Ciclo principale
	for (uint32_t i = 0; i <= BlockNo; i++)
	{
		Block1.first = &Cipher.first[i * BlockLength];
		for (uint32_t j = i + 1; j <= (BlockNo + 1); j++)
		{
			// Calcola il blocco da confrontare
			Block2.first = &Cipher.first[j * BlockLength];

			// Se la distanza di hamming è 0 incrementa il contatore
			if (get_Hamming_distance(Block1, Block2) == 0) Hamming++;
		}
	}
	
	// Restituisce il risultato
	if (Hamming != 0)return ECB;
	else return CBC;
}
//---------------------------------------------------------------------------------------
DMode DCryptoAnalysis::detect_block_cipher_mode(string IPAddress, uint16_t Port, uint32_t BlockLength)
{
	// Dichiara un oggetto per la conversione di formati
	DFormatConverter Converter;
#ifdef CHALLENGE_12
	// Dichiara e alloca la memoria per la sonda
	pair<uint8_t*, uint32_t>Probe;
	Probe.first = allocate_memory<uint8_t>(BlockLength * 2);
	Probe.second = BlockLength * 2; 

	// Riempie la sonda con il carattere A
	memset(Probe.first, '.', BlockLength * 2);
#else
	// Dichiara e alloca la memoria per la sonda
	pair<uint8_t*, uint32_t>Probe;
	Probe.first = allocate_memory<uint8_t>(BlockLength * 3);
	Probe.second = BlockLength * 3;

	// Riempie la sonda con il carattere A
	memset(Probe.first, '.', BlockLength * 3);
#endif
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
#if CHALLENGE_12
	// Determina la modalità di cifratura
	return detect_block_cipher_mode(Cipher, BlockLength);
#else
	// Determina la modalità di cifratura
	if (find_identical_adjacent_blocks(Cipher, BlockLength)) return ECB;
	else return UMD;
#endif
}
//---------------------------------------------------------------------------------------
bool DCryptoAnalysis::find_identical_adjacent_blocks(const pair<uint8_t*, uint32_t> Text, const uint32_t BlockLength)
{
	// Dichiara i blocchi da confrontare
	pair<uint8_t*, uint32_t>Block1;
	pair<uint8_t*, uint32_t>Block2;

	// Assegna la lunghezza
	Block1.second = BlockLength;
	Block2.second = BlockLength;

	bool Found = false;

	// Ciclo principale
	for (uint32_t i = 0; i < Text.second - (BlockLength * 2); i++)
	{
		Block1.first = &(Text.first[i * BlockLength]);
		Block2.first = &(Text.first[i * BlockLength + BlockLength]);
		if (get_Hamming_distance(Block1, Block2) == 0)
		{
			Found = true;
			break;
		}
	}
	return Found;
}
//---------------------------------------------------------------------------------------
double* DCryptoAnalysis::get_character_frequency(const std::pair<uint8_t*, uint32_t> Text)
{
	// Dichiara il vettore delle frequenze
	double* Freq = allocate_memory<double>(256);

	// Azzera l'array delle frequenze
	for (uint32_t i = 0; i < 256; i++) Freq[i] = 0.0;

	// Calcola le ripetizioni dei caratteri nel testo
	for (uint32_t i = 0; i < Text.second; i++)Freq[Text.first[i]] += 1.0;

	// Calcola la frequenza relativa dei caratteri
	for (uint32_t i = 0; i < 256; i++) Freq[i] /= (double)Text.second;

	// Restituisce il vettore delle frequenze
	return Freq;
}
//---------------------------------------------------------------------------------------
uint32_t DCryptoAnalysis::get_cipher_cookie_length(string IPAddress, uint16_t Port, uint32_t BlockLength)
{
	// Dichiara un oggetto per la conversione di formati
	DFormatConverter Converter;

	// Dichiara e alloca la memoria per la sonda
	pair<uint8_t*, uint32_t>Probe;
	Probe.first = allocate_memory<uint8_t>(BlockLength);
	Probe.second = BlockLength;

	// Riempie la sonda con il carattere A
	memset(Probe.first, '.', BlockLength);

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
//---------------------------------------------------------------------------------------
pair<uint32_t, uint32_t> DCryptoAnalysis::get_cipher_cookie_and_prefix_length(string IPAddress, uint16_t Port, uint32_t BlockLength)
{
	// Dichiara l'oggetto per le conversioni di formato di dati
	DFormatConverter Converter;

	// Dichiara l'oggetto per interrogare il server
	DTCPv4Client Client;

	// Dichiara e inizializza il massimo spazio da riservare alla sonda
	uint32_t ProbeLength = BlockLength * 3;

	// Alloca lo spazio massimo per la sonda
	pair<uint8_t*, uint32_t>Probe;
	Probe.first = allocate_memory<uint8_t>(ProbeLength);

	// Dichiara la Lmin
	uint32_t Length = MAXANSWERLENGTH;

	// Indice iniziale della sonda
	uint32_t ProbeIndex;

	// Indice del blocco
	uint32_t BlockIndex;

	// Vera se trova due blocchi contigui uguali
	bool Found;

	// Ciclo principale
	for (uint32_t i = 0; i < ProbeLength; i++)
	{
		// Aggiorna la sonda
		Probe.first[i] = '.';
		Probe.second = i + 1;

		// Converte il messaggio in base64
		string CodedMessage = Converter.binary_to_base64(Probe, true, true).first;

		// Attiva la connessione
		Client.connect(IPAddress, Port);

		// Invia il messaggio
		Client.send(CodedMessage);

		// Legge la risposta
		string CodedAnswer = Client.read_until_close();

		// Converte la risposta
		std::pair<uint8_t*, uint32_t> Answer = Converter.base64_to_binary(CodedAnswer, true, true);

		// Assegna la lunghezza minima
		if (Length > Answer.second)
		{
			Length = Answer.second;
			ProbeIndex = Probe.second;
		}

		// Verifica che vi siano due blocchi uguali successivi
		tie(Found, BlockIndex) = get_identical_adjacent_blocks(Answer, BlockLength);
		if (Found)break;
	}

	// Lunghezza del prefisso
	uint32_t PrefixLength = (ProbeIndex + 3) * BlockLength - Probe.second;

	// Lunghezza del testo
	uint32_t PlainLength = Length - PrefixLength - (ProbeIndex + 1);

	// Restituisce i risultati
	return make_pair(PrefixLength, PlainLength);
}
//---------------------------------------------------------------------------------------
double DCryptoAnalysis::get_Hamming_distance(const pair<uint8_t*, uint32_t> Text, uint32_t BlockLength)
{
	// Dichiara e inizializza la distanza
	uint32_t Distance = 0;

	// Dichiara e inizializza un contatore
	uint32_t Count = 0;

	// Dichiara un puntatore per il testo e la sua lunghezza
	uint8_t* String;
	uint32_t Length;

	// Associa alla stringa i valori di Text
	tie(String, Length) = Text;

	// Dichiara la distanza media
	double Average = 0;
	for (uint32_t i = 0; i < Length - BlockLength; i += BlockLength)
	{
		for (uint32_t j = i + BlockLength; j < Length - BlockLength; j += BlockLength)
		{
			// Incrementa il contatore dei confronti
			Count++;

			// Confronta i blocchi che iniziano a i e a j
			Distance += get_Hamming_distance(make_pair(&String[i], BlockLength), make_pair(&String[j], BlockLength));
		}

	}

	//AverageDistance = (double)Distance / (double)(Count*BlockLength);
	Average = (double)Distance / (double)(Count);

	return Average;
}
//---------------------------------------------------------------------------------------
uint32_t DCryptoAnalysis::get_Hamming_distance(const pair<uint8_t*, uint32_t> Text1, 
	const pair<uint8_t*, uint32_t> Text2)
{
	// Dichiara e inizializza la distanza di Hamming
	uint32_t Distance = 0;

	// Dichiara e inizializza la variabile dove copiare il risultato della XOR tra i due 
	// byte
	uint8_t Result = 0;

	// Attenzione: il calcolo è effettuato sulla lunghezza minima dei due testi
	uint32_t Length = 0;
	if (Text1.second < Text2.second) Length = Text1.second;
	else Length = Text2.second;

	// Calcola la distanza di Hamming tra i due testi
	for (uint32_t i = 0; i < Length; i++)
	{
		// Esegue l'operazione XOR tra i due byte
		Result = Text1.first[i] ^ Text2.first[i];

		// Conta il numero di bit a 1 nel risultato
		Distance += popcount(Result);
	}
	// Se le lunghezze dei due testi sono diverse	
	if (Text1.second != Text2.second)
	{
		// Aggiunge la differenza di lunghezza alla distanza di Hamming
		Distance += abs((int32_t)Text1.second - (int32_t)Text2.second) * 8;
	}

	// Restituisce la distanza di Hamming calcolata
	return Distance;
}
//---------------------------------------------------------------------------------------
pair<bool, uint32_t> DCryptoAnalysis::get_identical_adjacent_blocks(const pair<uint8_t*, uint32_t> Text, const uint32_t BlockLength)
{
	// Indice del primo blocco identico
	uint32_t Index = 0;

	// Dichiara i blocchi da confrontare
	pair<uint8_t*, uint32_t>Block1;
	pair<uint8_t*, uint32_t>Block2;

	// Assegna la lunghezza
	Block1.second = BlockLength;
	Block2.second = BlockLength;

	bool Found = false;

	// Calcola il numero dei blocchi
	uint32_t BlockNo = Text.second / BlockLength;

	// Ciclo principale
	for (uint32_t i = 0; i < BlockNo-3; i++)
	{
		Block1.first = &(Text.first[i * BlockLength]);
		Block2.first = &(Text.first[i * BlockLength + BlockLength]);
		if (get_Hamming_distance(Block1, Block2) == 0)
		{
			Found = true;
			Index = i;
			break;
		}
	}
	return make_pair(Found, Index);
}
//---------------------------------------------------------------------------------------
double DCryptoAnalysis::get_language_score(const pair<uint8_t*, uint32_t> Text,
    const double* Language)
{
	try
	{
		// Controlla se il testo è nullo, ha lunghezza zero o se la lingua è nulla
		if ((Text.first == nullptr) || (Text.second == 0)||(Language==nullptr))
			throw invalid_argument("Text is null or has zero length or no language.");

		// Dichiara Chi quadro
		double Chi2 = 0.0;

		// Dichiara il vettore delle frequenze e calcola la frequenza dei caratteri
		double* Freq = get_character_frequency(Text);

		// Calcola il punteggio di scostamento dalla distribuzione (Chi2)
		for (uint32_t i = 0; i < 256; i++)
		{
			switch (i)
			{
			case ' ':
			case '\n': //Chi2 += (Freq[32] - Language[26]) * (Freq[32] - Language[26]) /Language[26];
				Chi2 += pow((Freq[32] - Language[26]), 2.0) / Language[26];
				break;
			case '.':
			case ',':
			case ':':
			case ';':
			case '!':
			case '?':
				Chi2 += Freq[i];
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z': //Chi2 += (Freq[i] + Freq[i + 32] - Language[i - 65]) * (Freq[i] + Freq[i + 32] - Language[i - 65]) / Language[i - 65];
				Chi2 += pow((Freq[i] + Freq[i + 32] - Language[i - 65]), 2.0) / Language[i - 65];
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
				break;
			default: if (Freq[i])Chi2 += 1; break;
			}
		}

		// Restituisce il punteggio calcolato
		return Chi2;
	}
	catch (const std::exception& e)
	{
		set_error(STDERROR, e.what());
		return -1.0; // Restituisce -1 in caso di errore
	}
}
