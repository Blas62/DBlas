//***************************************************************************************
// 0.01.00
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include "DError.h"
#include "DNetworkEnum.h"
#include <boost/asio.hpp>
//---------------------------------------------------------------------------------------
namespace blas::utility::network
{
	class DServer : blas::utility::DError
	{
	public:
		DServer();
		~DServer();
		void start(DIPProtocol Protocol, DIPVersion Version, uint32_t Port);
	private:
		void session(boost::asio::ip::tcp::socket Socket);
	};
}

