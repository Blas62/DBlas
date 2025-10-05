//***************************************************************************************
// Include
#include "pch.h"
#include "DError.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace std;
using namespace blas::utility;
//---------------------------------------------------------------------------------------
void DError::set_error(uint8_t NewCode)
{
	ErrorCode = NewCode;
	ErrorMessage = "Generic Error";
}
//---------------------------------------------------------------------------------------
void DError::set_error(uint8_t NewCode, string NewMessage)
{
	ErrorCode = NewCode;
	ErrorMessage = NewMessage;
}
//---------------------------------------------------------------------------------------
void DError::clear_error()
{
	ErrorCode = 0;
	ErrorMessage = "";
}