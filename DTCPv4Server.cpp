//***************************************************************************************
// Include
#include "pch.h"
#include "DTCPv4Server.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace blas::utility::tcp::v4;
//---------------------------------------------------------------------------------------
void DTCPv4Server::start(uint32_t Port, 
	function<string(string)>Process)
{
	try
	{
		system::error_code Error;
		io_context IO;
		ip::tcp::acceptor Acceptor(IO, ip::tcp::endpoint(ip::tcp::v4(), Port));
		cout << "Server is running on port " << Port << endl;
		bool Continue = true;
		while (Continue)
		{
			ip::tcp::socket Socket(IO);
			Acceptor.accept(Socket);
			cout << "Client connected!\n";
			session(Socket, Port, Process);
		}

	}
	catch (system::error_code& e)
	{
		set_error(e.value(), e.message());
	}
}
//---------------------------------------------------------------------------------------
void DTCPv4Server::session(boost::asio::ip::tcp::socket& Socket, uint32_t Port, function<string(string)>Process)
{
	try
	{
		//system::error_code Error;
		asio::streambuf Buffer;
		asio::read_until(Socket, Buffer, "\0'");
		string Request;
		istream Input(&Buffer);
		getline(Input, Request);
		string Answer = Process(Request);
		asio::write(Socket, asio::buffer(Answer));
	}
	catch (system::error_code& e)
	{
		set_error(e.value(), e.message());
	}
}
