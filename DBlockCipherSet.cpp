//***************************************************************************************
// Include
#include "pch.h"
#include "DBlockCipherSet.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace blas::cryptography::cipher;
using namespace blas::utility;
using namespace std;
//---------------------------------------------------------------------------------------
void DBlockCipherSet::cipher()
{
	try
	{
		// Inizializza il vettore di uscita
		uint32_t Blocks = Input.second / ByteNb + 1;
		Output.second = Blocks * ByteNb;
		Output.first = allocate_memory<uint8_t>(Output.second);
		uint16_t Pad = Output.second - Input.second;
		if (Pad)memset(Output.first, Pad, Output.second);

		// Inizializza il vettore iniziale
		uint8_t* Vector = nullptr;
		uint8_t* Counter = nullptr;

		switch (Mode)
		{
		case ECB:
			break;
		case CBC:
			if (InitVector == NULL)throw "Initial vector missing";
			Vector = allocate_memory<uint8_t>(ByteNb);
			memcpy(Vector, InitVector, ByteNb);
			break;
		case OFB:
			break;
		case CFB:
			break;
		case CTR:
			if (Nonce == NULL)throw "Nonce missing";
			Counter = allocate_memory<uint8_t>(ByteNb);
			break;
		case XTS:
			break;
		}

		// Dichiara e inizializza un indice
		uint32_t Index = 0;

		// Dichiara e inizializza un vettore per il cifrato
		uint8_t* CipherText = nullptr;

		// Copia il vettore in ingresso in quello in uscita
		memcpy(Output.first, Input.first, Input.second);

		do
		{
			switch (Mode)
			{
			case ECB:
				// Cifra un blocco
				CipherText = cipher_block(&Output.first[Index]);

				// Copia il blocco cifrato nel blocco in uscita
				memcpy(&Output.first[Index], CipherText, ByteNb);

				// Aggiona l'indice
				Index += ByteNb;

				// Esce e va al blocco successivo
				break;
			case CBC:
				// Fa XOR con il vettore
				for (uint16_t i = 0; i < ByteNb; i++)Output.first[Index + i] ^= Vector[i];

				// Cifra un blocco
				CipherText = cipher_block(&Output.first[Index]);

				// Copia il blocco cifrato nel vettore
				memcpy(Vector, CipherText, ByteNb);

				// Copia il blocco cifrato nel blocco in uscita
				memcpy(&Output.first[Index], CipherText, ByteNb);

				// Aggiona l'indice
				Index += ByteNb;

				// Esce e va al blocco successivo
				break;
			case OFB:
				break;
			case CFB:
				break;
			case CTR:
				// Aggiorna il counter in funzione dell'endian
				for (uint16_t i = 0; i < NonceSize; i++)
				{
					if (Endian == big)
					{
						Counter[ByteNb - NonceSize + i] = Nonce[i];
					}
					else
					{
						Counter[ByteNb - NonceSize + i] = Nonce[NonceSize - 1 - i];
					}
				}

				// Cifra il contatore
				CipherText = cipher_block(Counter);

				// Cifra il testo
				for (uint16_t i = 0; i < ByteNb; i++)
				{
					CipherText[i] ^= Output.first[Index + i];
				}

				// Copia il blocco cifrato nel blocco in uscita
				if ((Output.second - Index) < ByteNb)
					memcpy(&Output.first[Index], CipherText, (Output.second - Index));
				else
					memcpy(&Output.first[Index], CipherText, ByteNb);

				// Aggiorna la nuance
				for (uint16_t i = NonceSize; i > 0; i--)
				{
					Nonce[i - 1] += 1;
					if (Nonce[i - 1] != 0)break;
				}
				Index += ByteNb;
				break;
			case XTS:
				break;
			default:
				break;
			}
		} while (Index < Output.second);
	}
	catch (char* e)
	{
		DBlockCipher::set_error(1, e);
	};
}
//---------------------------------------------------------------------------------------
void DBlockCipherSet::decipher()
{
	try
	{
		// Inizializza il vettore di uscita
		uint32_t Blocks = Input.second / ByteNb;
		Output.second = Blocks * ByteNb;
		Output.first = allocate_memory<uint8_t>(Output.second);
		//-----------------------------------------------------------------------------------------
		// Inizializza il vettore iniziale
		uint8_t* Vector = nullptr;
		uint8_t* Counter = nullptr;

		switch (Mode)
		{
		case ECB:
			break;
		case CBC:
			if (InitVector == NULL)throw "Initial vector missing";
			Vector = allocate_memory<uint8_t>(ByteNb);
			memcpy(Vector, InitVector, ByteNb);
			break;
		case OFB:
			break;
		case CFB:
			break;
		case CTR:
			if (Nonce == NULL)throw "Nonce missing";
			Counter = allocate_memory<uint8_t>(ByteNb);
			break;
		case XTS:
			break;
		}

		// Dichiara e inizializza un indice
		uint32_t Index = 0;

		// Dichiara e inizializza un vettore per il chiaro
		uint8_t* PlainText = nullptr;
		
		// Copia il vettore in ingresso in quello in uscita
		memcpy(Output.first, Input.first, Input.second);

		do
		{
			switch (Mode)
			{
			case ECB:
				// Decifra un blocco
				PlainText = decipher_block(&Output.first[Index]);

				// Copia il blocco cifrato nel blocco in uscita
				memcpy(&Output.first[Index], PlainText, ByteNb);

				// Aggiona l'indice
				Index += ByteNb;

				// Esce e va al blocco successivo
				break;
			case CBC:
				// Decifra un blocco
				PlainText = decipher_block(&Output.first[Index]);

				// Fa XOR con il vettore
				for (uint16_t i = 0; i < ByteNb; i++)PlainText[i] ^= Vector[i];

				// Copia il blocco decifrato nel vettore
				memcpy(Vector, &Output.first[Index], ByteNb);

				// Copia il blocco decifrato nel vettore
				memcpy(&Output.first[Index], PlainText, ByteNb);

				// Aggiona l'indice
				Index += ByteNb;

				// Esce e va al blocco successivo
				break;
			case OFB:
				break;
			case CFB:
				break;
			case CTR:
				// Aggiorna il counter in funzione dell'endian
				for (uint16_t i = 0; i < NonceSize; i++)
				{
					if (Endian == big)
					{
						Counter[ByteNb - NonceSize + i] = Nonce[i];
					}
					else
					{
						Counter[ByteNb - NonceSize + i] = Nonce[NonceSize - 1 - i];
					}
				}
				// Cifra il contatore
				PlainText = cipher_block(Counter);

				// Cifra il testo
				for (uint16_t i = 0; i < ByteNb; i++)
				{
					PlainText[i] ^= Output.first[Index + i];
				}
				if ((Output.second - Index) < ByteNb)
					memcpy(&Output.first[Index], PlainText, (Output.second - Index));
				else
					memcpy(&Output.first[Index], PlainText, ByteNb);

				// Aggiorna la nonce
				for (uint16_t i = NonceSize; i > 0; i--)
				{
					Nonce[i - 1] += 1;
					if (Nonce[i - 1] != 0)break;
				}
				// Aggiona l'indice
				Index += ByteNb;
				// Esce e va al blocco successivo
				break;
			case XTS:
				break;
			default:
				break;
			}
		} while (Index < Output.second);
		switch (Mode)
		{
		case ECB:
		case CBC:
			if (check_padding(PlainText))
			{
				Output.second -= PlainText[ByteNb - 1];
				Output.first[Output.second] = 0;
			}
			break;
		case OFB:
			break;
		case CFB:
			break;
		case CTR:
			break;
		case XTS:
			break;
		default:
			break;
		}
	}
	catch (char* e)
	{
		DBlockCipher::set_error(1, e);
	};
}
//---------------------------------------------------------------------------------------
void DBlockCipherSet::open(pair<uint8_t*, uint32_t> Source)
{
	Input.first = Source.first;
	Input.second = Source.second;
}
