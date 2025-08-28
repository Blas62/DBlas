//***************************************************************************************
// Include
#include "pch.h"
#include "DTCPv4Client.h"
//---------------------------------------------------------------------------------------
// Namespace
using namespace blas::utility::tcp::v4;
using namespace boost;
using namespace boost::asio;
using namespace std;
//---------------------------------------------------------------------------------------
void DTCPv4Client::connect(const string& Host, const uint32_t Port)
{
	try
	{
		system::error_code Error;
		ip::tcp::resolver::results_type EndPoint = Resolver.resolve(Host, to_string(Port), Error);
		if (Error)throw Error;
		asio::connect(Socket, EndPoint, Error);
		if (Error)throw Error;
	}
	catch (system::error_code& e)
	{
		set_error(e.value(), e.message());
	}
}
//---------------------------------------------------------------------------------------
size_t DTCPv4Client::send(const string& Data)
{
	try
	{
		system::error_code Error;
		size_t Size = write(Socket, asio::buffer(Data), Error);
		if (Error)throw Error;
		else return Size;
	}
	catch (system::error_code& e)
	{
		set_error(e.value(), e.message());
		return 0;
	}
}
//---------------------------------------------------------------------------------------
size_t DTCPv4Client::read_some(string& Data)
{
	try
	{
		system::error_code Error;
		std::array<char, 4096> Buffer{};
		size_t Size = Socket.read_some(boost::asio::buffer(Buffer), Error);
		if (Error == boost::asio::error::eof)
		{
			Data.assign(Buffer.data(), Size);
			return Size;
		}
		if (Error)throw Error;
		Data.assign(Buffer.data(), Size);
		return Size;
	}
	catch (system::error_code& e)
	{
		set_error(e.value(), e.message());
		return 0;
	}
}
//---------------------------------------------------------------------------------------
string DTCPv4Client::read_until_close()
{
	try
	{
		system::error_code Error;
		string Result;
		while (true)
		{
			string Trunk;
			size_t Size = 0;
			Size = read_some(Trunk);
			if (Error)throw Error;
			Result += Trunk;
			if (Size == 0)break;
		}
		return Result;
	}
	catch (system::error_code& e)
	{
		set_error(e.value(), e.message());
		return "";
	}
}
//---------------------------------------------------------------------------------------
void DTCPv4Client::close()
{
	try
	{
		system::error_code Error;
		if (Socket.is_open())
		{
			Socket.shutdown(ip::tcp::socket::shutdown_both, Error); 
			Socket.close(Error); 
		}
	}
	catch (system::error_code& e)
	{
		set_error(e.value(), e.message());
	}
}
//-----------------------------------------------------------------------------------
