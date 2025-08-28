//***************************************************************************************
// Include
#include "pch.h"
#include "DCipherFile.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace std;
using namespace std::filesystem;
using namespace blas::cryptography::cipher;
//---------------------------------------------------------------------------------------
void DCipherFile::cipher_file(path Plain, path Cipher)
{
	try
	{
		// Imposta i nomi dei file di origine e destinazione
		set_file_name(Plain, Cipher);

		// Apre i file di input/output
		open_file();

		// Esegue la cifratura (funzione virtuale pura)
		cipher();

		// Chiude i file
		close_file();
	}
	catch (filesystem_error& e)
	{
		// Gestione errori: salva codice e messaggio
		set_error(e.code().value(), e.what());
		// Restituisce il messaggio d'errore
		// return get_error_message();
	}
}
//---------------------------------------------------------------------------------------
void DCipherFile::decipher_file(path Cipher, path Plain)
{
	try
	{
		// Imposta i nomi dei file sorgente e destinazione
		set_file_name(Cipher, Plain);

		// Apre i file
		open_file();

		// Esegue la decifratura del contenuto
		decipher();

		// Chiude i file
		close_file();
	}
	catch (filesystem_error& e)
	{
		// Gestione errori: salva codice e messaggio
		set_error(e.code().value(), e.what());
		// Restituisce il messaggio d'errore
		// return get_error_message();
	}
}
//---------------------------------------------------------------------------------------
void DCipherFile::close_file()
{
	try
	{
		// Chiude i flussi di input e output
		In.close();
		Out.close();
		
		// Rinomina il file temporaneo come file di destinazione
		rename(TmpName, DstName);

		// Rimuove il file sorgente originale
		remove(SrcName);

	}
	catch (filesystem_error& e)
	{
		// Gestione errori: salva codice e messaggio
		set_error(e.code().value(), e.what());
		// Restituisce il messaggio d'errore
		// return get_error_message();
	}
}
//---------------------------------------------------------------------------------------
void DCipherFile::open_file()
{
	try
	{
		// Apre il file sorgente in modalità binaria per la lettura
		In.open(SrcName, ifstream::binary);

		// Abilita eccezioni su errore di output
		Out.exceptions(ofstream::failbit | ofstream::badbit);

		// Apre il file temporaneo in modalità binaria per la scrittura
		Out.open(TmpName, ofstream::binary);

	}
	catch (filesystem_error& e)
	{
		// Gestione errori: salva codice e messaggio
		set_error(e.code().value(), e.what());
		// Restituisce il messaggio d'errore
		// return get_error_message();
	}
}
//---------------------------------------------------------------------------------------
void DCipherFile::set_file_name(path NewSrcName, path NewDstName)
{
	try
	{
		// Assegna il percorso del file sorgente
		SrcName = NewSrcName;

		// Calcola la dimensione del file sorgente
		set_source_size();

		// Assegna il percorso del file di destinazione
		DstName = NewDstName;

		// Crea il percorso del file temporaneo modificando l'estensione
		TmpName = DstName;
		TmpName.replace_extension(TmpExt);

	}
	catch (filesystem_error& e)
	{
		// Gestione errori: salva codice e messaggio
		set_error(e.code().value(), e.what());
		// Restituisce il messaggio d'errore
		// return get_error_message();
	}
}
