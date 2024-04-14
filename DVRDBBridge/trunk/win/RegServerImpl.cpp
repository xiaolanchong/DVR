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
#include "RegServerImpl.h"
#include "VideoConfigRegImpl.h"
#include "../DVRDBBridge.h"
#include "../common/CommonUtility.h"

//======================================================================================//
//                                 class RegServerImpl                                  //
//======================================================================================//

RegServerImpl::RegServerImpl()
{
}

RegServerImpl::~RegServerImpl()
{
}

void	RegServerImpl::GetCameras( std::vector<int>& CamLocalArr, std::vector<int>& CamAllArr )
try
{
	VideoConfigRegImpl RegVideoCfg;
	DBBridge::IVideoConfig* pVideoCfg = &RegVideoCfg;
	GetServerCameraList( pVideoCfg, CamLocalArr, CamAllArr );
}
catch ( std::exception& ex ) 
{
	throw DBBridge::IDBServer::DBServerException(ex.what());
}

void	RegServerImpl::AddFrames( const std::vector<DBBridge::Frame>& frames  )
{
	UNREFERENCED_PARAMETER(frames);
	// not implemented
}

void	RegServerImpl::DeleteInfoBeforeTime( time_t BoundTime ) 
{
	UNREFERENCED_PARAMETER(BoundTime);
	// no any time info
}