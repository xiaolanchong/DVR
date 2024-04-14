//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ðåàëèçàöèÿ IDBServer

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   16.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _D_B_SERVER_IMPL_5611947964141285_
#define _D_B_SERVER_IMPL_5611947964141285_

#include "../interface/IDBServer.h"
//#include "OdbcConnection.h"

//======================================================================================//
//                                  class DBServerImpl                                  //
//======================================================================================//

//! \brief ðåàëèçàöèÿ IDBServer
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   16.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class DBServerImpl : public DBBridge::IDBServer
{
	std::tstring m_sConnectionString;
public:
	DBServerImpl(const std::string& sConnectionString);
	virtual ~DBServerImpl();

	virtual void	GetCameras( std::vector<int>& CamLocalArr, std::vector<int>& CamAllArr );
	virtual void	AddFrames( const std::vector<DBBridge::Frame>& frames  ) ;
	virtual void	DeleteInfoBeforeTime( time_t BoundTime ) ;
};

#endif // _D_B_SERVER_IMPL_5611947964141285_