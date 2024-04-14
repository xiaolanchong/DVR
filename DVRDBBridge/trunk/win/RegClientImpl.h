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
#ifndef _REG_CLIENT_IMPL_8818925307197196_
#define _REG_CLIENT_IMPL_8818925307197196_

#include "../interface/IDBClient.h"

//======================================================================================//
//                                 class RegClientImpl                                  //
//======================================================================================//

//! \brief Реализация на реестре
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   20.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class RegClientImpl : public DBBridge::IDBClient
{
public:
	RegClientImpl();
	virtual ~RegClientImpl();

//	virtual void GetCameraArr(std::vector<int>& CameraIDArr) ;
	virtual void GetFrames( time_t StartTime, time_t EndTime, std::vector<DBBridge::Frame>& FrameArr ); 

	virtual void GetCameraArr(std::vector<std::pair<int, std::string> >& CameraIDArr) ;
	virtual void SetCameraDescription( int CameraID, const std::string& sNewCameraDesc ) ;
};

#endif // _REG_CLIENT_IMPL_8818925307197196_