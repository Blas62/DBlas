//***************************************************************************************
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include "DBlockCipher.h"
#include "DCipherFile.h"
//---------------------------------------------------------------------------------------
namespace blas::cryptography::cipher
{
	class DBlockCipherFile : public virtual DBlockCipher, public virtual DCipherFile
	{
	public:
		DBlockCipherFile();
		~DBlockCipherFile();
	protected:
		std::string cipher();
		std::string decipher();
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DBlockCipherFile::DBlockCipherFile() :
		DBlockCipher(),
		DCipherFile()
	{

	}
	//------------------------------------------------------------------------------------
	inline DBlockCipherFile::~DBlockCipherFile()
	{
		//zeroize();
	}

}
/*namespace blas::cryptography::cipher
{
	class DBlockCipherFile: public virtual DBlockCipher, public virtual DCipherFile
	{
	public:
		DBlockCipherFile();
		~DBlockCipherFile();
		std::string cipher();
		std::string decipher();
	protected:
	private:
	};
	//-----------------------------------------------------------------------------------
	inline DBlockCipherFile::DBlockCipherFile() :
		DBlockCipher(),
		DCipherFile()
	{

	}
	//------------------------------------------------------------------------------------
	inline DBlockCipherFile::~DBlockCipherFile()
	{

	}
}
*/
