//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Чтение/запись настроек системы

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "SystemSettings.h"

bool	SystemSettings::IsLocalIPAddress( ULONG IPAddress ) 
{
	PHOSTENT pHost = gethostbyname(NULL);

	std::set<ULONG>	AllAddresses;
	if(pHost)
	{
		int nAddr = 0;

		while(pHost->h_addr_list[nAddr])
		{
			ULONG addr = ((IN_ADDR*)pHost->h_addr_list[nAddr])->S_un.S_addr ;
			AllAddresses.insert( addr );
			nAddr++;
		}
	}
	// localhost is the alwayse our address
	AllAddresses.insert(inet_addr( "127.0.0.1"));
	return  AllAddresses.find( IPAddress ) != AllAddresses.end();
}