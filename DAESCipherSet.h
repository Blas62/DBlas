//***************************************************************************************
// 1.00.00
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include "DBlockCipherSet.h"
#include "DAESCipher.h"
//---------------------------------------------------------------------------------------
namespace blas::cryptography::cipher
{
	class DAESCipherSet: public DBlockCipherSet, public DAES
	{
	public:
		DAESCipherSet();
		DAESCipherSet(uint32_t NewKeySize);
		~DAESCipherSet();
	};
	//-----------------------------------------------------------------------------------
	inline DAESCipherSet::DAESCipherSet() : DBlockCipherSet(), DAES()
	{

	}
	//-----------------------------------------------------------------------------------
	inline DAESCipherSet::DAESCipherSet(uint32_t NewKeySize): DBlockCipherSet(), DAES(NewKeySize)
	{
	}
	//-----------------------------------------------------------------------------------
	inline DAESCipherSet::~DAESCipherSet()
	{
	}
}
