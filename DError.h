//***************************************************************************************
// Pragma
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include <string>
//---------------------------------------------------------------------------------------
// Define
#define STDERROR 1
#define UNKERROR 99
//---------------------------------------------------------------------------------------
// Namespace
namespace blas
{
	namespace utility
	{
		class DError
		{
		public:
			DError();
			~DError();
			virtual void clear();
			virtual uint8_t get_error_code();
			virtual std::string get_error_message();
			virtual bool is_error();
			virtual void set_error(const uint8_t NewCode);
			virtual void set_error(const uint8_t NewCode, std::string NewMessage);
		protected:
		private:
			uint8_t ErrorCode;
			std::string ErrorMessage;
		};
		//-------------------------------------------------------------------------------
		inline DError::DError()
		{
			clear();
		}
		//-------------------------------------------------------------------------------
		inline DError::~DError()
		{

		}
		//-------------------------------------------------------------------------------
		inline uint8_t DError::get_error_code()
		{
			return ErrorCode;
		}
		//-------------------------------------------------------------------------------
		inline std::string DError::get_error_message()
		{
			return ErrorMessage;
		}
		//-------------------------------------------------------------------------------
		inline bool DError::is_error()
		{
			return (ErrorMessage != "");
		}
	}
}

