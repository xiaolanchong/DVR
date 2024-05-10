//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ðåàëèçàöèÿ IDBClient

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   16.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _OWN_I_P_ADDRESS_5344849942039038_
#define _OWN_I_P_ADDRESS_5344849942039038_

#include <stdexcept>
//======================================================================================//
//                                  class OwnIPAddress                                  //
//======================================================================================//

//! \brief ðåàëèçàöèÿ IDBClient
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   16.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class WinSock
{
	int err;
public:
	WinSock() : err( -1 )
	{
		WSADATA wsaData;
		err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	}
	~WinSock ()
	{
		if( !err ) WSACleanup();
	}
};

inline bool IsValidIPAddress( unsigned long addr)
{
	/*
		1.0.0.1 to 126.255.255.254 
		128.1.0.1 to 191.255.255.254 
		192.0.1.1 to 223.255.254.254 
	*/

	struct 
	{
		const char* Start;
		const char* End;
	} ValidRanges [] = 
	{
		{ "1.0.0.1",   "126.255.255.254"  },
		{ "127.0.0.1", "127.0.0.254"	  },
		{ "128.1.0.1", "191.255.255.254"  },
		{ "192.0.1.1", "223.255.254.254"  }
	};
	
	unsigned long RealAddr			= ntohl ( addr );
	for ( size_t i =0; i < sizeof(ValidRanges)/sizeof(ValidRanges[0]); ++i )
	{
		unsigned long NetOrderStartIP	= inet_addr( ValidRanges[i].Start );
		unsigned long StartIP			= ntohl (NetOrderStartIP);
		unsigned long NetOrderEndIP		= inet_addr( ValidRanges[i].End   );
		unsigned long EndIP				= ntohl (NetOrderEndIP);
		
		if( StartIP <= RealAddr && RealAddr <= EndIP )
		{
			return true;
		}
	}
	return false;
}

inline	void AddDebugIP( std::vector<unsigned long>& IPList )
{
	unsigned long addr;
	addr = inet_addr( "1.0.5.0");
	IPList.push_back( addr );
	addr = inet_addr( "0.1.0.4");
	IPList.push_back( addr );
}

inline std::vector<unsigned long> GetFullIPAddressList()
{
	std::vector<unsigned long> IPFullList;
	PHOSTENT pHost = gethostbyname(NULL);
	if(pHost)
	{
		int nAddr = 0;
		while(pHost->h_addr_list[nAddr])
		{
			const IN_ADDR* AddrStruct = ((IN_ADDR*)pHost->h_addr_list[nAddr]);
			//unsigned long addr = AddrStruct->s_addr;
			IPFullList.push_back( AddrStruct->s_addr );
			nAddr++;
		}
	}
	unsigned long LocalHost;
	LocalHost = inet_addr( "127.0.0.1");
	if( std::find( IPFullList.begin(), IPFullList.end(), LocalHost ) == IPFullList.end() )
	{
		IPFullList.push_back( LocalHost );
	}
	
#ifdef _DEBUG
	AddDebugIP(IPFullList);
#endif
	return IPFullList;
}

inline std::vector<std::tstring> GetOwnIPAddress() 
{
	USES_CONVERSION;


	std::vector<unsigned long>		IPFullList = GetFullIPAddressList() ;
	std::vector<std::tstring>	IPValidList;
	for ( size_t i =0; i < IPFullList.size(); ++i )
	{
		if ( IsValidIPAddress( IPFullList[i] ) )
		{
			IN_ADDR inaddr;
			inaddr.s_addr = IPFullList[i];
			LPCSTR szIPAddress = inet_ntoa(inaddr);
			IPValidList.push_back( std::tstring(CA2CT(szIPAddress)) );
		}
	}
	
	if( IPValidList.empty() )
	{
		throw std::runtime_error("Failed to get the IP address");
	}
	return IPValidList;
}

#endif // _OWN_I_P_ADDRESS_5344849942039038_