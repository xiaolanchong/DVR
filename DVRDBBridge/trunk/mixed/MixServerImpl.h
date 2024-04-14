//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Смешанная имплементация - реестр + sqlite БД

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   21.04.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MIX_SERVER_IMPL_7669861790498190_
#define _MIX_SERVER_IMPL_7669861790498190_

#include "../interface/IDBServer.h"

//======================================================================================//
//                                 class MixServerImpl                                  //
//======================================================================================//

//! \brief Смешанная имплементация - реестр + sqlite БД
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   21.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class MixServerImpl : public DBBridge::IDBServer
{
	std::string m_sDBFileName;
public:
	MixServerImpl(const std::string& sDBFilename);
	virtual ~MixServerImpl();

	virtual void	GetCameras( std::vector<int>& CamLocalArr, std::vector<int>& CamAllArr );
	virtual void	AddFrames( const std::vector<DBBridge::Frame>& frames  ) ;
	virtual void	DeleteInfoBeforeTime( time_t BoundTime ) ;
};

#endif // _MIX_SERVER_IMPL_7669861790498190_