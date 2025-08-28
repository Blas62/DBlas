//***************************************************************************************
#include "pch.h"
#include "DFormatConverter.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace std;
using namespace std::filesystem;
using namespace blas::utility;
//---------------------------------------------------------------------------------------
DFormatConverter::DFormatConverter():DError()
{
	// Inizializza la tabella di conversione
	Base64Table = allocate_memory<char>(64);
	strcpy_s(Base64Table, 65, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

}
//---------------------------------------------------------------------------------------
DFormatConverter::~DFormatConverter()
{
	// Libera la memoria allocata per la tabella di conversione
	if (Base64Table != nullptr)
	{
		deallocate_memory(Base64Table);
		Base64Table = nullptr;
	}
}
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> DFormatConverter::base64_to_binary(path InName, path OutName, bool URL, bool Padded)
{
	try
	{
		// Verifica che non sia url
		if (URL)
		{
			Base64Table[62] = '-'; // Sostituisce '+' con '-'
			Base64Table[63] = '_'; // Sostituisce '/' con '_'
		}
		else
		{
			Base64Table[62] = '+'; // Ripristina '+' se l'URL non è abilitato
			Base64Table[63] = '/'; // Ripristina '/' se l'URL non è abilitato
		}

		// Verifica che il file di input esista e sia accessibile
		if (!exists(InName))throw runtime_error("The input file does not exist");

		// Verifica il file di input non si avuoto
		if (InName.empty())throw runtime_error("The input file name is empty");

		// Crea un nome di file temporaneo
		path TempName = InName;
		TempName.replace_extension(".tmp");

		// Apre i file di input	
		ifstream In(InName, ios::in | ios::binary);

		// Apre il file temporaneo di output
		ofstream Tmp(TempName, ios::out | ios::binary | ios::trunc);

		// Dichiara il carattere da leggere
		char c;

		// Dichiara la variabile per il conteggio dei caratteri letti
		uint32_t Length = 0;

		// Copia solo i caratteri base64 dal file di input al file temporaneo
		do
		{
			// Legge un carattere dal file di input
			In.get(c);
			// Se il carattere è in base64
			if ((!In.eof()) && is_base64(c))
			{
				// Scrive il carattere nel file temporaneo
				Tmp.put(c);
				Length++;
			}
		}while (!In.eof());

		// Verifica che ci sia il padding
		if (Padded && Length % 4)throw length_error("The Base64 text length is invalid");

		// Se il padding non è abilitato, aggiunge i caratteri di padding
		if (!Padded && Length % 4)
		{
			uint32_t NewLength = (Length/4+1)*4;
			for (uint32_t i = Length; i < NewLength; i++)
			{
				Tmp.put('=');
			}
			Length = NewLength;
		}

		// Chiude i file di input e temporaneo
		In.close();
		Tmp.close();

		// Apre i file di input	
		In.open(TempName, ios::in | ios::binary);

		// Apre il file di output
		ofstream Out(OutName, ios::out | ios::binary | ios::trunc);

		// Verifica che il file di output sia stato aperto correttamente
		if (!Out.is_open())throw runtime_error("Cannot open the output file");

		// Dichiara un buffer per il testo base64
		uint8_t Base64Block[4];

		// Dichiara un buffer per il testo binario
		uint8_t BinaryBlock[3];

		while (!In.eof())
		{
			// Legge quattro caratteri dal file
			In.read((char*)Base64Block, 4);

			// Converte il blocco di quattro caratteri base 64 in tre byte binari
			uint32_t BytesToWrite = block_to_binary((char*)Base64Block, BinaryBlock);

			// Scrive i byte binari nel file di output.
			if(!In.eof()) Out.write((char*)(BinaryBlock), BytesToWrite);

		}

		// Chiude i file di input e output
		In.close();
		Out.close();

		// Cancella il file temporaneo
		if (exists(TempName))remove(TempName);

		// Restituisce una coppia <tt>{nullptr, 0}</tt> se l'operazione ha avuto successo
		return make_pair(nullptr, 0);
	}
	catch (exception& e)
	{
		set_error(STDERROR, e.what());
		return make_pair((uint8_t*)get_error_message().c_str(), STDERROR);
	}
}
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> DFormatConverter::base64_to_binary(pair<char*, uint32_t> Base64, bool URL, bool Padded)
{
	try
	{
		// Verifica che non sia url
		if (URL)
		{
			Base64Table[62] = '-'; // Sostituisce '+' con '-'
			Base64Table[63] = '_'; // Sostituisce '/' con '_'
		}
		else
		{
			Base64Table[62] = '+'; // Ripristina '+' se l'URL non è abilitato
			Base64Table[63] = '/'; // Ripristina '/' se l'URL non è abilitato
		}

		// Verifica che ci sia il padding
		if(Padded && Base64.second % 4)throw length_error("The Base64 text length is invalid");
		if (!Padded)
		{
			// TODO: da completare la parte che gestisce il caso senza padding
		}

		// Calcola la lunghezza del testo binario
		uint32_t Length = (Base64.second / 4) * 3;

		// Alloca lo spazio di memoria necessario
		uint8_t* Binary = allocate_memory<uint8_t>(Length);

		// Dichiara e inizializza i contatori
		uint32_t j = 0; // Indice per il testo binario

		// Converte i blocchi di quattro caratteri base 64 in
		// tre caratteri esadecimali
		for (uint32_t i = 0; i <= Base64.second - 4; i += 4)
		{
			// Converte 4 caratteri base64 in 3 byte binari
			j += block_to_binary(&Base64.first[i], &Binary[j]);
		}
		return make_pair(Binary, j);

	}
	catch (exception& e)
	{
		set_error(STDERROR, e.what());
		return make_pair(nullptr, 0);
	}
}
//---------------------------------------------------------------------------------------
pair<char*, uint32_t> DFormatConverter::binary_to_base64(pair<uint8_t*, uint32_t> Binary, bool URL, bool Padded)
{
	try
	{
		// Se l'URL è abilitato, sostituisce i caratteri '+' e '/' con '-' e '_'
		if (URL)
		{
			Base64Table[62] = '-'; // Sostituisce '+' con '-'
			Base64Table[63] = '_'; // Sostituisce '/' con '_'
		}
		else
		{
			Base64Table[62] = '+'; // Ripristina '+' se l'URL non è abilitato
			Base64Table[63] = '/'; // Ripristina '/' se l'URL non è abilitato
		}

		// Calcola la lunghezza del testo in base64
		uint32_t Length = (Binary.second / 3 + 1) * 4;

		// Alloca lo spazio di memoria necessario
		char* Base64 = allocate_memory<char>(Length);

		// Dichiara e inizializza i contatori
		uint32_t i = 0; // Indice per il testo binario
		uint32_t j = 0; // Indice per il testo base64

		// Ciclo principale
		while (i < Binary.second)
		{
			// Converte i 3 byte in 4 caratteri base64
			block_to_base64(&Binary.first[i], &Base64[j], Binary.second - i);

			// Aggiorna gli indici
			i += 3; // Incrementa di 3 byte
			j += 4; // Incrementa di 4 caratteri base64
		}

		// Elimina il padding se non è richiesto
		if (!Padded)
		{
			// Rimuove i caratteri di padding '='
			while ((j > 0) && (Base64[j - 1] == '='))
			{
				Base64[j - 1] = '\0'; // Sostituisce il carattere di padding con il terminatore di stringa
				Length--; // Decrementa la lunghezza
				j--;
			}
		}

		// Restituisce il risultato
		return make_pair(Base64, Length);
	}
	catch (exception& e)	
	{
		set_error(STDERROR, e.what());
		return make_pair(nullptr, 0);
	}
}
//---------------------------------------------------------------------------------------
pair<char*, uint32_t> DFormatConverter::binary_to_text(pair<uint8_t*, uint32_t> Binary)
{
	try
	{
		uint32_t Length = Binary.second * 2;
		char* Text = allocate_memory<char>(Length);
		uint32_t j = 0;
		for (uint32_t i = 0; i < Binary.second; i++)
		{
			Text[j] = nibble_to_char(Binary.first[i] >> 4);
			j++;
			Text[j] = nibble_to_char(Binary.first[i] & 0x0F);
			j++;
		}
		return make_pair(Text, Length);
	}
	catch (exception& e)
	{
		set_error(STDERROR, e.what());
		return make_pair(nullptr, 0);
	}
	return std::pair<char*, uint32_t>();
}
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> DFormatConverter::text_to_binary(pair<char*, uint32_t> Text)
{
	try
	{
		// Verifica che la lunghezza del testo sia pari 
		if (Text.second % 2) throw length_error("The text length is invalid");
		else
		{
			// Alloca lo spazio di memoria pari alla meta' della lunghezza del testo
			uint8_t* Binary = allocate_memory<uint8_t>(Text.second / 2);

			// Dichiara e inizializza i contatori
			uint32_t i = 0;
			uint32_t j = 0;

			// Ciclo principale
			while (i < Text.second)
			{
				// Converte due caratteri in un byte
				Binary[j] = two_char_to_binary(&Text.first[i]);

				// Aggiorna gli indici
				i++;
				i++;
				j++;
			}

			// Restituisce il risultato
			return make_pair(Binary, j);
		}

	}
	catch (exception& e)
	{
		set_error(STDERROR, e.what());
		return make_pair(nullptr, 0);
	}
}
//---------------------------------------------------------------------------------------
uint8_t DFormatConverter::block_to_binary(char* InBlock, uint8_t* OutBlock)
{
	// Dichiara e inizializza le variabili temporanee
	uint32_t i = 0; // Indice per il testo base64
	uint32_t j = 0; // Indice per il testo binario
	uint8_t b1 = 0; // Primo nibble
	uint8_t b2 = 0; // Secondo nibble

	// Prima cifra
	b1 = decode_base64(InBlock[i]); // Estrae il primo carattere base64
	b1 = b1 << 2; // 00bbbbbb => 000000bb
	i++;
	b2 = decode_base64(InBlock[i]); // Estrae il secondo carattere base64
	b2 = b2 >> 4; // bbbb0000 => 0000bbbb
	OutBlock[j] = b1 | b2; // Esegue OR tra i due nibble per ottenere il primo byte
	j++;

	// Seconda cifra
	b1 = decode_base64(InBlock[i]); // Estrae il terzo carattere base64
	i++;
	if (InBlock[i] != '=')
	{
		b1 = b1 << 4; // 00bbbbbb => 0000bbbb
		b2 = decode_base64(InBlock[i]); // Estrae il quarto carattere base64
		b2 = b2 >> 2; // bbbb0000 => 000000bb
		OutBlock[j] = b1 | b2; // Esegue OR tra i due nibble per ottenere il secondo byte
		j++;
	}
	else return 1;

	// Terza cifra
	b1 = decode_base64(InBlock[i]); // Estrae il quinto carattere base64
	i++;
	if (InBlock[i] != '=')
	{
		b1 = b1 << 6; // 00bbbbbb => 000000bb
		b2 = decode_base64(InBlock[i]); // Estrae il sesto carattere base64
		OutBlock[j] = b1 | b2; // Esegue OR tra i due nibble per ottenere il terzo byte
		return 3; // Ritorna il numero di byte convertiti
	}
	else return 2;
}
//---------------------------------------------------------------------------------------
void DFormatConverter::block_to_base64(uint8_t* InBlock, char* OutBlock, const uint32_t Bytes)
{
	// Dichiara e inizializza le variabili temporanee
	uint32_t i = 0; // Indice per il testo base64
	uint32_t j = 0; // Indice per il testo binario
	uint8_t b1 = 0; // Primo nibble
	uint8_t b2 = 0; // Secondo nibble

	// Prima cifra
	// Estrae i primi sei bit
	b1 = InBlock[i] >> 2; // Primi 6 bit del primo byte
	OutBlock[j] = Base64Table[b1];

	// Seconda cifra
	// Estrae gli ultimi due bit  e gli sposta alla posizione 5 e 6
	// xxxxxxbb => 00bb0000
	b1 = InBlock[i];
	b1 = b1 & 0x03;
	b1 = b1 << 4;
	i++;
	// Se esiste, prende il carattere successivo ed estrae i primi 4 bit
	// bbbbxxx => 0000bbbb
	if (i < Bytes)
	{
		b2 = InBlock[i] >> 4;
		// Esegue OR tra b1 e b2 per ottenere la nuova cifra 00bbbbbb
		b1 = b2 | b1;
		j++;
		// Converte la cifra base 64
		OutBlock[j] = Base64Table[b1];
	}
	// Se non esiste, assegna 00bb0000 alla cifra successiva e '=' alle
	// altre due
	else
	{
		j++;
		OutBlock[j] = Base64Table[b1];
		j++;
		OutBlock[j] = '=';
		j++;
		OutBlock[j] = '=';
		return;
	}

	// Terza cifra
	// Estrae gli ultimi 4 bit e li mette in posizione 6,5,4,3
	// xxxxbbbb => 0000bbbb => 00bbbb00
	b1 = (InBlock[i] & 0x0f) << 2;
	i++;
	if (i < Bytes)
	{
		// Se esite il carattere successivo estrae i primi due bit
		// bbxxxxxx => 000000bb
		b2 = InBlock[i] >> 6;
		// Esegue OR tra b1 e b2 per ottenere la nuova cifra 00bbbbbb
		b1 = b2 | b1;
		j++;
		// Converte la cifra base 64
		OutBlock[j] = Base64Table[b1];
	}
	else
	{
		// Se non esiste, assegna 0000bbbb alla cifra successiva e '=' all'altra
		j++;
		OutBlock[j] = Base64Table[b1];
		j++;
		OutBlock[j] = '=';
		return;
	}

	// Quarta cifra
	// Estrae i primi 6 bit xxbbbbbb => 00bbbbbb
	b1 = InBlock[i] & 0x3f;
	j++;
	// Converte la cifra base 64
	OutBlock[j] = Base64Table[b1];
	i++;
	j++;
}
//---------------------------------------------------------------------------------------
const uint8_t blas::utility::DFormatConverter::decode_base64(const uint8_t C)
{
	try
	{
		// Se C è un carattere 'A'...'Z'...'a'...'z'
		if (isalpha(C))
		{
			if (isupper(C))return C - 'A';
			else return C - 'a' + 26;
		}
		else
		{
			if (isdigit(C))return C - '0' + 52;// Se C è un numero '0'...'9'
			else
			{
				if (C == Base64Table[62])return 62;// Se C è uno dei due caratteri speciali
				else
				{
					if (C == Base64Table[63])return 63;
					else throw invalid_argument("Invalid character for Base64 conversion");
				}
			}
		}
	}
	catch (exception& e)
	{
		set_error(STDERROR, e.what());
		return 0; // Ritorna 0 in caso di errore
	}
}
//---------------------------------------------------------------------------------------
uint8_t DFormatConverter::char_to_binary(char In)
{
	try
	{
		// Converte un carattere in un byte
		if (In >= '0' && In <= '9') return In - '0'; // Numeri
		else if (In >= 'A' && In <= 'F') return In - 'A' + 10; // Lettere maiuscole
		else if (In >= 'a' && In <= 'f') return In - 'a' + 10; // Lettere minuscole
		else throw invalid_argument("Invalid character for conversion");
	}
	catch (exception& e)
	{
		set_error(STDERROR, e.what());
		return 0; // Ritorna 0 in caso di errore
	}
}
//---------------------------------------------------------------------------------------
char DFormatConverter::nibble_to_char(const uint8_t Nibble)
{
	if (Nibble < 10) return Nibble + 48;
	else return Nibble + 87;

}
//---------------------------------------------------------------------------------------
uint8_t DFormatConverter::two_char_to_binary(char* In)
{
	// Converte due caratteri in un byte
	uint8_t Out = 0;
	Out |= char_to_binary(In[0]) << 4; // Primo carattere
	Out |= char_to_binary(In[1]);      // Secondo carattere
	return Out;
}
//---------------------------------------------------------------------------------------
const bool DFormatConverter::is_base64(const uint8_t C)
{
	if (C == Base64Table[62])return true;
	if (C == Base64Table[63])return true;
	if (C == '=')return true;
	if ((C >= 'A') && (C <= 'Z'))return true;
	if ((C >= 'a') && (C <= 'z'))return true;
	if ((C >= '0') && (C <= '9'))return true;
	return false;
}
//---------------------------------------------------------------------------------------
const bool DFormatConverter::is_binary(const uint8_t C)
{
	// Controlla il carattere
	if (C >= '0' && C <= '9') return true; // Numeri
	else if (C >= 'A' && C <= 'F') return true; // Lettere maiuscole
	else if (C >= 'a' && C <= 'f') return true; // Lettere minuscole
	return false;
}
