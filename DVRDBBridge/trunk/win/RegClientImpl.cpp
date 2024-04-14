//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Ðåàëèçàöèÿ íà ðååñòðå

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   20.04.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "RegClientImpl.h"
#include "VideoConfigRegImpl.h"
#include "../common/CommonUtility.h"

//======================================================================================//
//                                 class RegClientImpl                                  //
//======================================================================================//

RegClientImpl::RegClientImpl()
{
}

RegClientImpl::~RegClientImpl()
{
}

void RegClientImpl::GetCameraArr(std::vector<std::pair<int, std::string> >& CameraIDArr) 
try
{
	CameraIDArr.clear();
	VideoConfigRegImpl VideoConfig;
	DBBridge::GetAllCamerasWithName(&VideoConfig, CameraIDArr);
}
catch ( std::exception& ex ) 
{
	throw DBBridge::IDBClient::DBClientException(ex.what());
};

void RegClientImpl::SetCameraDescription( int CameraID, const std::string& sNewCameraDesc )
{
	UNREFERENCED_PARAMETER(CameraID);
	UNREFERENCED_PARAMETER(sNewCameraDesc);
}

void RegClientImpl::GetFrames( time_t /*StartTime*/, time_t /*EndTime*/, std::vector<DBBridge::Frame>& /*FrameArr*/ )
{
	// not implemented, in this mode can't save any data
}