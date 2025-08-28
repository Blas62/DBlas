//***************************************************************************************
// Include
#include "pch.h"
#include "DCryptoTestTools.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace blas::utility;
using namespace blas::cryptography::cipher;
using namespace blas::cryptography::analysis;
using namespace std::filesystem;
using namespace std;
//---------------------------------------------------------------------------------------
uint8_t* DCryptoTestTools::create_test_random_string(const uint32_t Length)
{
	uint8_t* String = allocate_memory<uint8_t>(Length);
	srand((uint32_t)time(NULL));
	for (uint32_t i = 0; i < Length; i++)String[i] = rand() % 256;
	return String;
}
//---------------------------------------------------------------------------------------
pair<DMode,path> DCryptoTestTools::encryption_oracle(DBlockCipherFile& Cipher, 
	path FileName, const uint32_t BlockLength)
{
	// Inizializza il seme del generatore dei numeri casuali
	srand((uint32_t)time(NULL));

	// Dichiara le variabili per il prembolo e il suffisso casuali
	uint32_t MaxBTLength = 10;
	uint32_t BTLength;

	// Crea il preambolo compreso di lunghezza tra 5 e 10 caratteri
	BTLength = rand() % (MaxBTLength / 2) + (MaxBTLength / 2);
	uint8_t* Prefix = create_test_random_string(BTLength);
	uint8_t* Suffix = create_test_random_string(BTLength);

	// Crea il file temporaneo da cifrare
	path TempFile = FileName;
	TempFile.replace_extension(".ext");
	ofstream Out(TempFile, ofstream::binary);
	ifstream In(FileName, ifstream::binary);
	char c;
	Out << Prefix;
	while (In.get(c))Out << c;
	Out << Suffix;
	In.close();
	Out.close();

	// Sceglie il modo di cifratura
	DMode Mode = ECB;
	srand((uint32_t)time(NULL));
	uint32_t RndMode = rand() % 2;
	switch (RndMode)
	{
	case 0: Mode = ECB;
		Cipher.set_mode(Mode);
		break;
	case 1: Mode = CBC;
		Cipher.set_mode(Mode);
		uint8_t* IV = create_test_random_string(BlockLength);
		Cipher.set_init_vector(IV);
		break;
	}

	// Cifra il file
	path CipherText = FileName;
	CipherText.replace_extension(".enc");
	Cipher.cipher_file(TempFile, CipherText);

	// Libera la memoria
	deallocate_memory(Prefix);
	deallocate_memory(Suffix);

	// Restituisce il modo di cifratura
	return make_pair(Mode,CipherText);
}
