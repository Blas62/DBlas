//***************************************************************************************
// Include
#include "pch.h"
#include "DAESCipher.h"
//--------------------------------------------------------
// Namespace
using namespace blas::utility;
using namespace blas::cryptography::cipher;
//---------------------------------------------------------
// Define
// Macro in inv_mix_columns: accede a wtemp di indice (x*Nb, y)
#define DKEY(x,y) wtemp[x*Nb+y]

// Attiva la decifra con l'algoritmo equivalente
#define EQUIVALENT

// Estrae dalla InvSBox il valore di coordinate (x, y)
#define INVSBOX(x,y) InvSBox[x*Nb*4+y]

// Estrae dalla SBox il valore di coordinate (x, y)
#define SBOX(x,y) SBox[x*Nb*4+y]

// Calcola l'elemento State di coordinate (x, y)
#define STATE(x,y) State[x*Nb+y]

// Attiva la modalità debug
#define DEBUG
//---------------------------------------------------------------------------------------
DAESKey::DAESKey() :DCipherAlgorithm()
{
	// Imposta il valore della lunghezza della chiave a 128 bit
	set_key_size(128);

	// Chiama la funzione di inizializzazione dell'oggetto
	initialize();
}
//---------------------------------------------------------------------------------------
DAESKey::DAESKey(uint16_t NewKeySize): DCipherAlgorithm()
{
	// Imposta il valore della lunghezza della chiave a NewKeySize
	set_key_size(NewKeySize);

	// Chiama la funzione di inizializzazione dell'oggetto
	initialize();
}
//---------------------------------------------------------------------------------------
DAESKey::~DAESKey()
{
	// Chiama la funzione zeroize
	zeroize();

	// Dealloca il vettore W nuova versione
	// delete[]W;
	deallocate_memory<uint8_t>(W);

	// Dealloca il vettore D nuova versione
	// delete[]D;
	deallocate_memory<uint8_t>(D);

	// Dealloca il vettore Rcon nuova versione
	// delete[]Rcon;
	deallocate_memory<uint8_t>(Rcon);
}
//---------------------------------------------------------------------------------------
void DAESKey::set_key(uint8_t* NewKey)
{
	// Assegna a Key la chiave NewKey
	Key = NewKey;

	// Esegue le operazioni di espansione della chiave
	expand_key();
}
// TODO: da questo punto in poi bisogna scrivere tutti i commenti
//---------------------------------------------------------------------------------------
void DAESKey::expand_key()
{
	//for (uint16_t i = 0; i < get_key_byte(); i++)W[i] = Key[i];
	// Copia la chiave Key nel vettore W
	memcpy(W, Key, get_key_byte());

	// Alloca un vettore temporaneo WTemp di Nb byte
	uint8_t* WTemp = new uint8_t[Nb];
	//-----------------------------------------------------------------------------------
	// Indice di partenza
	// KeyLength 128 192 256
	// i=Nk        4   6   8
	//-----------------------------------------------------------------------------------
	// Indice finale
	// KeyLength 128 192 256
	// i          43  51  59
	// i<         44  52  60
	// i<(Nr+1)*4
	//***********************************************************************************
	// Ciclo principale
	for (uint16_t i = Nk; i < (Nr + 1) * 4; i++)
	{
		//-------------------------------------------------------------------------------
		// Copia i byte nel vettore temporaneo
		for (int j = 0; j < 4; j++)WTemp[j] = W[(i - 1) * 4 + j];
		//-------------------------------------------------------------------------------
		// Esegui le operazioni 
		if (!(i % Nk))
		{
			rot_word(WTemp);
			sub_word(WTemp);
			recon(WTemp, i / Nk);
		}
		else
			//---------------------------------------------------------------------------
			// Ciclo intermedio per Nk=8
			if ((Nk == 8) & (!(i % 4))) sub_word(WTemp);
		//-------------------------------------------------------------------------------
		// Trascrivi i byte dal vettore temporaneo
		for (uint16_t j = 0; j < 4; j++)
			W[i * 4 + j] = W[(i - Nk) * 4 + j] ^ WTemp[j];
	}
	//***********************************************************************************
	// Calcola il vettore D
	//	for(uint16_t i=0;i<WLength;i++)D[i]=W[i];
	//	for(uint16_t i=1;i<Nr;i++)InvMixColumns(&D[i*Nb*4]);
	inv_mix_columns();

	// Dealloca il vettore temporaneo WTemp
	delete[]WTemp;
}
//---------------------------------------------------------------------------------------
void DAESKey::initialize()
{
	// Imposta nome e tipo dell'algoritmo
	set_algorithm_name("AES");
	set_algorithm_type("Block cipher");

	// Inizializza le variabili ambientali
	Nb = 4;
	Nk = get_key_byte() / 4;
	Nr = Nk + 6;
	WLength = Nb * (Nr + 1) * 4;

	// Alloca W e D
	W = new uint8_t[WLength];
	D = new uint8_t[WLength];

	// Azzera i vettori
	zeroize();

	// Alloca Rcon a assegna i valori
	Rcon = new uint8_t[10];
	Rcon[0] = 0X01;
	Rcon[1] = 0X02;
	Rcon[2] = 0X04;
	Rcon[3] = 0X08;
	Rcon[4] = 0X10;
	Rcon[5] = 0X20;
	Rcon[6] = 0X40;
	Rcon[7] = 0X80;
	Rcon[8] = 0X1B;
	Rcon[9] = 0X36;
}
//---------------------------------------------------------------------------------------
void DAESKey::inv_mix_columns()
{
	// Dichiara le variabili temporanee
	uint8_t b0, b1, b2, b3;

	// Dichiara il puntatore temporaneo 
	uint8_t* wtemp;

	//for (uint16_t i = 0; i < WLength; i++)D[i] = W[i];

	// Copia W in D
	memcpy(D, W, WLength);

	// Esegue il ciclo principale
	for (uint16_t i = 1; i < Nr; i++)
	{
		wtemp = &D[i * Nb * 4];
		for (uint16_t i = 0; i < Nb; i++)
		{
			// Estrae da wtemp i valori b0, b1, b2, b3
			b0 = DKEY(i, 0);
			b1 = DKEY(i, 1);
			b2 = DKEY(i, 2);
			b3 = DKEY(i, 3);

			// Assegna a wtemp i risultati ottenuti dall'XOR delle 
			// multiplication box
			DKEY(i, 0) = Mulx14[b0] ^ Mulx11[b1] ^ Mulx13[b2] ^ Mulx09[b3];
			DKEY(i, 1) = Mulx09[b0] ^ Mulx14[b1] ^ Mulx11[b2] ^ Mulx13[b3];
			DKEY(i, 2) = Mulx13[b0] ^ Mulx09[b1] ^ Mulx14[b2] ^ Mulx11[b3];
			DKEY(i, 3) = Mulx11[b0] ^ Mulx13[b1] ^ Mulx09[b2] ^ Mulx14[b3];
		}
	}
}
//---------------------------------------------------------------------------------------
void DAESKey::recon(uint8_t* WTemp, const int Index)
{
	WTemp[0] = WTemp[0] ^ Rcon[Index - 1];
}
//---------------------------------------------------------------------------------------
void DAESKey::rot_word(uint8_t* WTemp)
{
	uint8_t Temp = WTemp[0];
	for (int i = 0; i < 3; i++)WTemp[i] = WTemp[i + 1];
	WTemp[3] = Temp;
}
//---------------------------------------------------------------------------------------
void DAESKey::sub_word(uint8_t* WTemp)
{
	int Row;
	int Col;
	for (int i = 0; i < 4; i++)
	{
		Row = WTemp[i] >> 4;
		Col = WTemp[i] & 0xf;
		WTemp[i] = SBOX(Row, Col);
	}
}
//---------------------------------------------------------------------------------------
void DAESKey::zeroize()
{
	// Cancella i contenuti del vettore W e del vettore D
	memset(W, 0, WLength);
	memset(D, 0, WLength);
}
//---------------------------------------------------------------------------------------
DAES::DAES() : DBlockCipher()
{
	// Imposta il valore della chiave a 128 bit
	set_key_size(128);

	// Chiama la funzione di inizializzazione generale
	initialize();
}
//---------------------------------------------------------------------------------------
DAES::DAES(const uint16_t NewKeySize) : DBlockCipher(), Key(NewKeySize)
{
	// Imposta il valore della chiave a NewKeySize bit
	set_key_size(NewKeySize);

	// Chiama la funzione di inizializzazione generale
	initialize();
}
//---------------------------------------------------------------------------------------
DAES::~DAES()
{
	// Azzera il contenuto delle variabili
	zeroize();

	// Dealloca State
	delete[]State;

	// DEalloca Temp
	delete[]Temp;
}
//---------------------------------------------------------------------------------------
uint8_t* DAES::cipher_block(uint8_t* Plain)
{
	// TODO: Da commentare il codice
	Block = allocate_memory<uint8_t>(ByteNb);
	//for (uint16_t i = 0; i < ByteNb; i++)Block[i] = Plain[i];
	memcpy(Block, Plain, ByteNb);
	//-----------------------------------------------------------------------------------
	// Inizializza il vettore di input
	set_input();
	//-----------------------------------------------------------------------------------
	// Somma la chiave del ciclo 0
	add_round_key(0);
	//***********************************************************************************
	// Ciclo principale
	for (uint16_t i = 1; i < Nr; i++)
	{
		sub_bytes();
		shift_row();
		mix_columns();
		add_round_key(i);
	}
	//***********************************************************************************
	sub_bytes();
	shift_row();
	add_round_key(Nr);
	//-----------------------------------------------------------------------------------
	// Completa il vettore di output
	set_output();
	return Block;
}
//---------------------------------------------------------------------------------------
uint8_t* DAES::decipher_block(uint8_t* Cipher)
{
	// TODO: Da commentare il codice
	Block = allocate_memory<uint8_t> (ByteNb);
	for (uint16_t i = 0; i < ByteNb; i++)Block[i] = Cipher[i];
	//-----------------------------------------------------------------------------------
	// Inizializza il vettore di input
	set_input();
#ifndef EQUIVALENT
	//-----------------------------------------------------------------------------------
	// Somma la chiave del ciclo Nr
	AddRoundKey(Nr);
	//***********************************************************************************
	// Ciclo principale
	for (uint16_t i = Nr - 1; i > 0; i--)
	{
		InvShiftRows();
		InvSubBytes();
		AddRoundKey(i);
		InvMixColumns();
	}
	//***********************************************************************************
	InvShiftRows();
	InvSubBytes();
	AddRoundKey(0);
#else
	//***********************************************************************************
	// Equivalent inverse cipher
	//-----------------------------------------------------------------------------------
	// Somma la chiave equivalente del ciclo Nr
	add_eq_round_key(Nr);
	//***********************************************************************************
	// Ciclo principale
	for (uint16_t i = Nr - 1; i > 0; i--)
	{
		inv_sub_bytes();
		inv_shift_rows();
		inv_mix_columns();
		add_eq_round_key(i);
	}
	//***********************************************************************************
	inv_sub_bytes();
	inv_shift_rows();
	add_eq_round_key(0);
#endif
	//-----------------------------------------------------------------------------------
	// Completa il vettore di output
	set_output();
	return Block;
}
//---------------------------------------------------------------------------------------
void DAES::set_key(uint8_t* NewKey)
{
	// TODO: Da commentare il codice
	Key.set_key(NewKey);
}
//---------------------------------------------------------------------------------------
void DAES::add_eq_round_key(const uint16_t NewRound)
{
	// TODO: Da commentare il codice
	uint8_t* Di;
	Di = Key.get_DI(NewRound);
	for (uint16_t i = 0; i < (Nb * 4); i++)State[i] = State[i] ^ Di[i];
}
//---------------------------------------------------------------------------------------
void DAES::add_round_key(const uint16_t NewRound)
{
	// Puntatore alla chiave del round
	uint8_t* Wi;

	// Recupera la chiave di round da Key
	Wi = Key.get_WI(NewRound);

	// Applica lo XOR tra lo stato e la chiave del round
	for (uint16_t i = 0; i < (Nb * 4); i++)State[i] = State[i] ^ Wi[i];
}
//---------------------------------------------------------------------------------------
void DAES::initialize()
{
	// TODO: Da commentare il codice
	//-----------------------------------------------------------------------------------
	// Inizializza le variabili ambientali
	Nk = Key.get_key_byte() / 4;
	Nr = Nk + 6;
	Nb = 4;
	ByteNb = Nb * 4;
	//-----------------------------------------------------------------------------------
	// Imposta nome e tipo dell'algoritmo
	set_algorithm_type("Block cipher");
	//-----------------------------------------------------------------------------------
	// Assegna il nome dell'algoritmo
	switch (Nk)
	{
	case 4: set_algorithm_name("AES 128"); break;
	case 6: set_algorithm_name("AES 192"); break;
	case 8: set_algorithm_name("AES 256"); break;
	}
	//-----------------------------------------------------------------------------------
	// Inizializza i vettori
	State = new uint8_t[Nb * 4];
	Temp = new uint8_t[Nb * 4];
	zeroize();
}
//---------------------------------------------------------------------------------------
void DAES::inv_mix_columns()
{
	// TODO: Da commentare il codice
	uint8_t b0, b1, b2, b3;
	for (uint16_t i = 0; i < Nb; i++)
	{
		b0 = STATE(i, 0);
		b1 = STATE(i, 1);
		b2 = STATE(i, 2);
		b3 = STATE(i, 3);
		STATE(i, 0) = Mulx14[b0] ^ Mulx11[b1] ^ Mulx13[b2] ^ Mulx09[b3];
		STATE(i, 1) = Mulx09[b0] ^ Mulx14[b1] ^ Mulx11[b2] ^ Mulx13[b3];
		STATE(i, 2) = Mulx13[b0] ^ Mulx09[b1] ^ Mulx14[b2] ^ Mulx11[b3];
		STATE(i, 3) = Mulx11[b0] ^ Mulx13[b1] ^ Mulx09[b2] ^ Mulx14[b3];
	}
}
//---------------------------------------------------------------------------------------
void DAES::inv_shift_rows()
{
	// TODO: Da commentare il codice
	uint8_t* Temp;
	Temp = new uint8_t[Nb * 4];
	for (uint16_t i = 0; i < (Nb * 4); i++)
		Temp[i] = State[(16 + (i - Nb * (i + 4)) % (Nb * 4)) % (Nb * 4)];
	for (uint16_t i = 0; i < (Nb * 4); i++)
	{
		State[i] = Temp[i];
		Temp[i] = 0;
	}
}
//---------------------------------------------------------------------------------------
void DAES::inv_sub_bytes()
{
	// TODO: Da compmentare il codice
	int Row;
	int Col;
	for (uint16_t i = 0; i < Nb * 4; i++)
	{
		Row = State[i] >> 4;
		Col = State[i] & 0xf;
		int Index = Row * Nb * 4 + Col;
		State[i] = INVSBOX(Row, Col);
	}
}
//---------------------------------------------------------------------------------------
void DAES::mix_columns()
{
	// TODO: Da commentare il codice
	uint8_t b0, b1, b2, b3;
	for (uint16_t i = 0; i < Nb; i++)
	{
		b0 = STATE(i, 0);
		b1 = STATE(i, 1);
		b2 = STATE(i, 2);
		b3 = STATE(i, 3);
		STATE(i, 0) = Mulx02[b0] ^ Mulx03[b1] ^ b2 ^ b3;
		STATE(i, 1) = b0 ^ Mulx02[b1] ^ Mulx03[b2] ^ b3;
		STATE(i, 2) = b0 ^ b1 ^ Mulx02[b2] ^ Mulx03[b3];
		STATE(i, 3) = Mulx03[b0] ^ b1 ^ b2 ^ Mulx02[b3];
	}
}
//---------------------------------------------------------------------------------------
void DAES::set_input()
{
	// TODO: Da commentare il codice
	memcpy(State, Block, (Nb * 4));
}
//---------------------------------------------------------------------------------------
void DAES::set_output()
{
	// TODO: Da commentare il codice
	//for (uint16_t i = 0; i < (Nb * 4); i++)Block[i] = State[i];
	memcpy(Block, State, (Nb * 4));
}
//---------------------------------------------------------------------------------------
void DAES::shift_row()
{
	// TODO: Da commentare il codice
	for (uint16_t i = 0; i < (Nb * 4); i++)
		Temp[i] = State[i * (Nb + 1) % (Nb * 4)];
	memcpy(State, Temp, (Nb * 4));
	memset(Temp, 0, (Nb * 4));
}
//---------------------------------------------------------------------------------------
void DAES::sub_bytes()
{
	// TODO: Da commentare il codice
	int Row;
	int Col;
	int Index;
	for (uint16_t i = 0; i < Nb * 4; i++)
	{
		Row = State[i] >> 4;
		Col = State[i] & 0xf;
		Index = Row * Nb * 4 + Col;
		State[i] = SBOX(Row, Col);
	}
}
//---------------------------------------------------------------------------------------
void DAES::zeroize()
{
	// TODO: Da commentare il codice
	memset(State, 0, (Nb * 4));
	memset(Temp, 0, (Nb * 4));
}