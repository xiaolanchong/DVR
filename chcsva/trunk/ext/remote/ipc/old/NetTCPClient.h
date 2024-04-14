//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Кроссплатформенный мультипотоковый клиент для отправки данных

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   29.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _NET_T_C_P_CLIENT_5339108888870092_
#define _NET_T_C_P_CLIENT_5339108888870092_

#include "InternetAddress.h"
#include "NetSocket.h"
#include "NetType.h"

//======================================================================================//
//                                  class NetTCPClient                                  //
//======================================================================================//

//! \brief Кроссплатформенный мультипотоковый клиент для отправки данных
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class NetTCPClient
{
public:
	NetTCPClient();
	virtual ~NetTCPClient();

	void	Send(	const InternetAddress& ServerAddress, 
					const void* pData, 
					size_t nSize) ;
};

class NetTCPLongRunClient 
{
	NetSocket	m_Socket;
public:
	NetTCPLongRunClient( const InternetAddress& ServerAddress, 
						 unsigned short ClientPort = PORT_ANY );
	void	Send( const void* pData, size_t nSize ) ;
};

#endif // _NET_T_C_P_CLIENT_5339108888870092_