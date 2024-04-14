//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: SOCKADDR îáåðòêà

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   28.06.2006
*/                                                                                      //
//======================================================================================//
#include "chcs.h"
#include "InternetAddress.h"
//======================================================================================//
//                                class InternetAddress                                 //
//======================================================================================//

InternetAddress::InternetAddress( const SOCKADDR_IN& address):
	SOCKADDR_IN(address)
{
	//memcpy( this, &address, sizeof(SOCKADDR_IN)); 
}

InternetAddress::InternetAddress(
	unsigned long address,
	unsigned short port)
{
	memset( this, 0, sizeof(InternetAddress) );
	this->sin_family = AF_INET;
	this->sin_port   = htons(port);
	this->sin_addr.s_addr = htonl(address);
}

InternetAddress::InternetAddress(
	LPCTSTR lpAddress,
	unsigned short port)
{
	memset( this, 0, sizeof(InternetAddress) );
	const char *pAddress;

#ifndef UNICODE
	pAddress = lpAddress;
#else
	CHAR stAddress[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, lpAddress, lstrlen(lpAddress) + 1,
		stAddress, MAX_PATH, NULL, NULL);

	pAddress = stAddress;
#endif

	sin_family = AF_INET;
	sin_port   = htons(port);
	sin_addr.s_addr = inet_addr(pAddress);

	if(this->sin_addr.s_addr == INADDR_NONE)
	{
		// address wasn't dotted ip, try to resolve name...
		HOSTENT *pHostEnt = ::gethostbyname(pAddress);

		if(pHostEnt)
			CopyMemory(&sin_addr, pHostEnt->h_addr_list[0], pHostEnt->h_length);
	}
}

//////////////////////////////////////////////////////////////////////////

std::string InternetAddressToString (const InternetAddress& Addr)
{
	char	s[64];

	memset(s, 0, 64 * sizeof(char) );

	_snprintf ( s, sizeof( s )/sizeof(char), ("%i.%i.%i.%i:%i"), 
		Addr.sin_addr.S_un.S_un_b.s_b1, 
		Addr.sin_addr.S_un.S_un_b.s_b2, 
		Addr.sin_addr.S_un.S_un_b.s_b3, 
		Addr.sin_addr.S_un.S_un_b.s_b4, 
		ntohs(Addr.sin_port)
		);

	return std::string(s);
}