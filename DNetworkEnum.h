//***************************************************************************************
#pragma once
//---------------------------------------------------------------------------------------
// Namespace
namespace blas::utility::network
{
	enum class DIPProtocol {
		TCP,    // Protocollo TCP
		UDP     // Protocollo UDP
	};
	enum class DIPVersion {
		IPv4,     // Versione 1
		IPv6      // Versione 2
	};
	enum class DNetworkError {
		None = 0,          // Nessun errore
		ReadError = 1,     // Errore durante la lettura dei dati
		WriteError = 2,    // Errore durante la scrittura dei dati
		CloseError = 3,    // Errore durante la chiusura della connessione
		ConnectError = 4,  // Errore durante la connessione al server
		RuntimeError = 5,  // Errore di runtime
		UnknownError = 99  // Errore sconosciuto
	};
	enum class DRESTRequestType
	{
		GET,    // Richiesta GET
		POST,   // Richiesta POST
		PUT,    // Richiesta PUT
		DEL,    // Richiesta DELETE
		PATCH   // Richiesta PATCH
	};
}
