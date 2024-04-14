//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация на реестре

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   20.04.2006
*/                                                                                      //
//======================================================================================//
#ifndef _REG_SERVER_IMPL_2288984509514211_
#define _REG_SERVER_IMPL_2288984509514211_

#include "../interface/IDBServer.h"

//======================================================================================//
//                                 class RegServerImpl                                  //
//======================================================================================//

//! \brief Реализация на реестре
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   20.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class RegServerImpl : public DBBridge::IDBServer
{
public:
	RegServerImpl();
	virtual ~RegServerImpl();

	virtual void	GetCameras( std::vector<int>& CamLocalArr, std::vector<int>& CamAllArr );

	virtual void	AddFrames( const std::vector<DBBridge::Frame>& frames  ) ;

	virtual void	DeleteInfoBeforeTime( time_t BoundTime ) ;
};

#endif // _REG_SERVER_IMPL_2288984509514211_