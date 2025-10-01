//***************************************************************************************
// 1.00.00
#pragma once
//---------------------------------------------------------------------------------------
// Include
#include "DError.h"
#include <boost/asio.hpp>
//---------------------------------------------------------------------------------------
namespace blas::utility::tcp::v4
{
	class DTCPv4Client: blas::utility::DError
	{
	public:
		DTCPv4Client();
		~DTCPv4Client();
		void connect(const std::string& Host, const uint32_t Port);
		void connect(const std::pair<std::string, uint16_t>Server);
		std::size_t send(const std::string& data);
		std::size_t read_some(std::string& out);
		std::string read_until_close();
		void close();
		bool is_open() const;
	private:
		boost::asio::io_context IO; // Contesto I/O di Asio (anche se sincrono, serve)
		boost::asio::ip::tcp::resolver Resolver; // Resolver per convertire host/port in endpoint
		boost::asio::ip::tcp::socket Socket; // Socket TCP bloccante
	};
	//-----------------------------------------------------------------------------------
	inline DTCPv4Client::DTCPv4Client() :
		DError(),
		IO(),
		Resolver(IO),
		Socket(IO)
	{
	}
	//-----------------------------------------------------------------------------------
	inline DTCPv4Client::~DTCPv4Client()
	{
		close();
	}
	//-----------------------------------------------------------------------------------
	inline void DTCPv4Client::connect(const std::pair<std::string, uint16_t>Server)
	{
		connect(Server.first, Server.second);
	}
	//-----------------------------------------------------------------------------------
}
