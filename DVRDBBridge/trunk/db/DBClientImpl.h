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
#ifndef _D_B_CLIENT_IMPL_9910667434938928_
#define _D_B_CLIENT_IMPL_9910667434938928_

#include "../interface/IDBClient.h"
//#include "OdbcConnection.h"

//======================================================================================//
//                                  class DBClientImpl                                  //
//======================================================================================//

//! \brief ðåàëèçàöèÿ IDBClient
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   16.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class DBClientImpl : public DBBridge::IDBClient
{
	std::string m_sConnectionString;
public:
	DBClientImpl(const std::string& sConnection);
	virtual ~DBClientImpl();
private:
	virtual void GetCameraArr(std::vector<std::pair<int, std::string> >& CameraIDArr) ;
	virtual void SetCameraDescription( int CameraID, const std::string& sNewCameraDesc ) ;
	virtual void GetFrames( time_t StartTime, time_t EndTime, std::vector<DBBridge::Frame>& FrameArr ); 
};

#endif // _D_B_CLIENT_IMPL_9910667434938928_