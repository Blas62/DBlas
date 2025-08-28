//***************************************************************************************
// Include
#include "pch.h"
#include "DBlockCipherFile.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace std;
using namespace blas::cryptography::cipher;
using namespace blas::utility;
//---------------------------------------------------------------------------------------
string DBlockCipherFile::cipher()
{
	try
	{
		// Dichiara e inizializza i vettori iniziali
		uint8_t* Vector = nullptr;
		uint8_t* Counter = nullptr;

		// Inizializza le modalità di cifartura a blocchi
		switch (Mode)
		{
		case ECB:
			break;
		case CBC:
			if (InitVector == nullptr)throw "Initial vector missing";
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

		// Dichiara i vettori e lle variabili
		uint32_t BytesRead = 0;
		uint16_t BytesToWrite = ByteNb;
		uint8_t* PlainText = allocate_memory<uint8_t>(ByteNb);
		uint8_t* CipherText = NULL;

		// Ciclo principale
		do
		{
			// Legge un blocco dal file
			In.read((char*)PlainText, ByteNb);

			// Aggiorna il numero di byte letti
			BytesRead += (uint32_t)In.gcount();

			// Se non sono stati letti abbastanza byte inserisci byte di padding
			if (Mode != CTR)
			{
				if ((uint16_t)In.gcount() != ByteNb)
					pad_block_PKCS7(PlainText, (uint16_t)In.gcount());
			}

			// Esegue la cifra
			switch (Mode)
			{
			case ECB:
				CipherText = cipher_block(PlainText);
				break;
			case CBC:
				for (uint16_t i = 0; i < ByteNb; i++)PlainText[i] ^= Vector[i];
				CipherText = cipher_block(PlainText);
				memcpy(Vector, CipherText, ByteNb);
				break;
			case OFB:
				//CipherText = cipher_block(Vector);
				//for (uint16_t i = 0; i < ByteNb; i++)Vector[i] = CipherText[i];
				//for (uint16_t i = 0; i < ByteNb; i++)CipherText[i] ^= PlainText[i];
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

				// Decifra il testo
				for (uint16_t i = 0; i < ByteNb; i++)
				{
					CipherText[i] ^= PlainText[i];
				}

				// Aggiorna la nonce
				for (uint16_t i = NonceSize; i > 0; i--)
				{
					Nonce[i - 1] += 1;
					if (Nonce[i - 1] != 0)break;
				}
				break;
			case XTS:
				break;
			}

			// Scrive i adti sul file di output
			if ((BytesRead == SrcSize) && (Mode == CTR))
				BytesToWrite = (uint16_t)In.gcount();
			Out.write((char*)CipherText, BytesToWrite);
		} while (BytesRead != SrcSize);

		// Se SrcSize%ByteNb==0 aggiunge un blocco di padding 16 16 ...
		if ((SrcSize % ByteNb == 0) && (Mode != CTR))
		{
			memset(PlainText, ByteNb, ByteNb);
			switch (Mode)
			{
			case ECB:
				CipherText = cipher_block(PlainText);
				break;
			case CBC:
				for (uint16_t i = 0; i < ByteNb; i++)PlainText[i] ^= Vector[i];
				CipherText = cipher_block(PlainText);
				memcpy(Vector, CipherText, ByteNb);
				//for (uint16_t i = 0; i < ByteNb; i++)Vector[i] = CipherText[i];
				break;
			}

			// Scrive questo ultimo blocco
			Out.write((char*)CipherText, ByteNb);
		}

		// Fine del ciclo principale

		// Chiude i file
		//close();

		// Azzera del blocco e cancella i vettori
		//for (uint16_t i = 0; i < ByteNb; i++)PlainText[i] = 0;
		memset(PlainText, 0, ByteNb);
		delete[] PlainText;
		if (Vector != NULL)delete[] Vector;
		if (Counter != NULL)delete[] Counter;
	}
	catch (filesystem::filesystem_error& e)
	{
		DCipherFile::set_error(e.code().value(), e.what());
		return DCipherFile::get_error_message();
	}
	return "";
}
//---------------------------------------------------------------------------------------
string DBlockCipherFile::decipher()
{
	try
	{
		// Inizializza i vettori iniziali
		uint8_t* Vector = nullptr;
		uint8_t* Counter = nullptr;

		// Inizializza le modalità di cifartura a blocchi
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

		// Dichiarazione dei vettori e delle variabili
		uint32_t BytesRead = 0;
		uint16_t BytesToWrite = 0;
		uint8_t* CipherText = allocate_memory<uint8_t>(ByteNb);
		uint8_t* PlainText = nullptr;

		// Ciclo principale
		do
		{
			// Legge un blocco dal file
			In.read((char*)CipherText, ByteNb);
			
			// Aggiorna il numero di byte letti
			//BytesRead = (uint16_t)InFile.gcount();
			BytesRead += (uint32_t)In.gcount();

			// Se non sono stati letti byte esce dal ciclo
			//if (BytesRead == 0)break;

			// Esegue la decifra
			switch (Mode)
			{
			case ECB:
				PlainText = decipher_block(CipherText);
				break;
			case CBC:
				PlainText = decipher_block(CipherText);
				for (uint16_t i = 0; i < ByteNb; i++)PlainText[i] ^= Vector[i];
				//for (uint16_t i = 0; i < ByteNb; i++)Vector[i] = CipherText[i];
				memcpy(Vector, CipherText, ByteNb);
				break;
			case OFB:
				if (Vector != nullptr)
				{
					PlainText = decipher_block(Vector);
					//for (uint16_t i = 0; i < ByteNb; i++)Vector[i] = PlainText[i];
					memcpy(Vector, PlainText, ByteNb);
					for (uint16_t i = 0; i < ByteNb; i++)PlainText[i] ^= CipherText[i];
				}
				else throw std::invalid_argument("Vector is not allocated");
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

				// Decifra il testo
				for (uint16_t i = 0; i < ByteNb; i++)
				{
					PlainText[i] ^= CipherText[i];
				}

				// Aggiorna la nuance
				for (uint16_t i = NonceSize; i > 0; i--)
				{
					Nonce[i - 1] += 1;
					if (Nonce[i - 1] != 0)break;
				}
				break;
			case XTS:
				break;
			}

			// Controlla che non vi sia stato del padding
			//if (SrcSize == InFile.tellg())
			if (BytesRead == SrcSize)
			{
				if (Mode == CTR)
				{
					BytesToWrite = (uint16_t)In.gcount();
				}
				else
				{
					if (PlainText != nullptr)
					{
						if (check_padding(PlainText))
							BytesToWrite = ByteNb - PlainText[ByteNb - 1];
						else BytesToWrite = ByteNb;
					}
					else throw std::exception("Plaintext is not allocated");
				}
			}
			else BytesToWrite = ByteNb;

			// Scrive il risultato sul file di output
			Out.write((char*)PlainText, BytesToWrite);
			//} while (BytesRead == ByteNb);
		} while (BytesRead != SrcSize);
		// Fine del ciclo principale

		// Chiusura dei file
		//close();

		// Azzera del blocco e cancella i vettori
		//for (uint16_t i = 0; i < ByteNb; i++)PlainText[i] = 0;
		//memset(PlainText, 0, ByteNb);
        if (PlainText != nullptr)
            memset(PlainText, 0, ByteNb);
		delete[] PlainText;
		if (Vector != nullptr)delete[] Vector;
		if (Counter != nullptr)delete[] Counter;
	}
	catch (filesystem::filesystem_error& e)
	{
		DCipherFile::set_error(e.code().value(), e.what());
		return DCipherFile::get_error_message();
	}
	catch (std::exception& e)
	{
		DCipherFile::set_error(STDERROR, e.what());
		return DCipherFile::get_error_message();
	}
	return "";
}
