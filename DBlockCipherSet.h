//***************************************************************************************
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include "DCipherSet.h"
#include "DBlockCipher.h"
//---------------------------------------------------------------------------------------
namespace blas::cryptography::cipher
{
	class DBlockCipherSet: public virtual DCipherSet, public virtual DBlockCipher
	{
	public:
		DBlockCipherSet();
		~DBlockCipherSet();
	protected:
		virtual void cipher() override;
		virtual void decipher() override;
		virtual void open(std::pair<uint8_t*, uint32_t>Source) override;
	};
	//--------------------------------------------------------------------------------------
	inline DBlockCipherSet::DBlockCipherSet() : DCipherSet(), DBlockCipher()
	{

	}
	//--------------------------------------------------------------------------------------
	inline DBlockCipherSet::~DBlockCipherSet()
	{
	}
}
