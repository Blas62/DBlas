//***************************************************************************************
// 0.01.00
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include "DError.h"
#include "DNetworkEnum.h"
#include <boost/asio.hpp>
#include <functional>
//---------------------------------------------------------------------------------------
namespace blas::utility::tcp::v4
{
	class DTCPv4Server: public blas::utility::DError
	{
	public:
		DTCPv4Server();
		~DTCPv4Server();
		void start(uint32_t Port, std::function<std::string (std::string)>Process);
	private:
		void session(boost::asio::ip::tcp::socket& Socket, 
			uint32_t Port, 
			std::function<std::string(std::string)>Process);
	};
	//-----------------------------------------------------------------------------------
	inline DTCPv4Server::DTCPv4Server(): DError()
	{

	}
	//-----------------------------------------------------------------------------------
	inline DTCPv4Server::~DTCPv4Server()
	{
	}
	//-----------------------------------------------------------------------------------

}

