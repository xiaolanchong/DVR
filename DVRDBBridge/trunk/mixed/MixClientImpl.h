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
#ifndef _MIX_CLIENT_IMPL_9471521228314448_
#define _MIX_CLIENT_IMPL_9471521228314448_

#include "../interface/IDBClient.h"

//======================================================================================//
//                                 class MixClientImpl                                  //
//======================================================================================//

//! \brief Смешанная имплементация - реестр + sqlite БД
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   21.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class MixClientImpl : public DBBridge::IDBClient
{
	std::string m_sDBFileName;
public:
	MixClientImpl(const std::string& sDBFileName);
	virtual ~MixClientImpl();

	virtual void GetCameraArr(std::vector<std::pair<int, std::string> >& CameraIDArr) ;
	virtual void SetCameraDescription( int CameraID, const std::string& sNewCameraDesc ) ;
	virtual void GetFrames( time_t StartTime, time_t EndTime, std::vector<DBBridge::Frame>& FrameArr ); 
};

#endif // _MIX_CLIENT_IMPL_9471521228314448_