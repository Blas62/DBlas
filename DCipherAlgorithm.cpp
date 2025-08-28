//***************************************************************************************
// Include
#include "pch.h"
#include "DCipherAlgorithm.h"
//---------------------------------------------------------------------------------------	
// Namespace
using namespace blas::cryptography::cipher;
//---------------------------------------------------------------------------------------
DCipherAlgorithm::DCipherAlgorithm() : DError()
{
	// Imposta il nome dell'algoritmo e il tipo
	set_algorithm_name("Generic");
	set_algorithm_type("Generic");

	// Inizializza la chiave
	Key = nullptr;
	KeySize = 0;
}
