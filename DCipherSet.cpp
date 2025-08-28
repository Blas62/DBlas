//***************************************************************************************
// Include
#include "pch.h"
#include "DCipherSet.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace std;
using namespace blas::cryptography::cipher;
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> DCipherSet::cipher_set(pair<uint8_t*, uint32_t> Plain)
{
	try
	{
		open(Plain);
		cipher();
		clear();
		return Output;
	}
	catch (exception const& e)
	{
		set_error(1, e.what());
		return make_pair((uint8_t*)get_error_message().c_str(), get_error_code());
	}
}
//---------------------------------------------------------------------------------------
pair<uint8_t*, uint32_t> DCipherSet::decipher_set(pair<uint8_t*, uint32_t> Cipher)
{
	try
	{
		open(Cipher);
		decipher();
		clear();
		return Output;
	}
	catch (exception const& e)
	{
		set_error(1, e.what());
		return make_pair((uint8_t*)get_error_message().c_str(), get_error_code());
	}
}
//---------------------------------------------------------------------------------------
void DCipherSet::clear()
{

}

