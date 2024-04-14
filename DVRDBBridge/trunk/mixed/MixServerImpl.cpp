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

#include "stdafx.h"
#include "MixServerImpl.h"
#include "../win/RegServerImpl.h"
#include "SqliteWrapper.h"
#include "../common/CommonUtility.h"
//#include "../DVRDBBridge.h"

//======================================================================================//
//                                 class MixServerImpl                                  //
//======================================================================================//

MixServerImpl::MixServerImpl(const std::string& sDBFilename):
	m_sDBFileName(sDBFilename)
{
}

MixServerImpl::~MixServerImpl()
{
}

void	MixServerImpl::GetCameras( std::vector<int>& CamLocalArr, std::vector<int>& CamAllArr )
{
	RegServerImpl RegVideoCfg;
	RegVideoCfg.GetCameras( CamLocalArr, CamAllArr );
}

void	MixServerImpl::AddFrames( const std::vector<DBBridge::Frame>& frames  )
try
{
	if( frames.empty() ) return;
	SQLiteDatabase db( m_sDBFileName.c_str() );

	const char* szInsert = 
#if 0
		"INSERT INTO Frames(CameraID, TimeBegin, TimeEnd, LeftBound, TopBound, RightBound, BottomBound) "
		"VALUES( ?, ?, ?, ?, ?, ?, ?);";
#else
		"INSERT INTO Frames(CameraID, TimeBegin, TimeEnd) "
		"VALUES( ?, ?, ? );";
#endif
	

	for ( size_t i = 0; i < frames.size(); ++i )
	{
		SQLiteUpdateStatement st(db, szInsert);

		st.Bind( 1, frames[i].GetCameraID() );
		st.Bind( 2, GetTimeForSql( frames[i].GetStartTime() ));
		st.Bind( 3, GetTimeForSql( frames[i].GetEndTime() ));
#if 0
		st.Bind( 4, frames[i].GetRect().x );
		st.Bind( 5, frames[i].GetRect().y );
		st.Bind( 6, (frames[i].GetRect().x + frames[i].GetRect().w) );
		st.Bind( 7, (frames[i].GetRect().y + frames[i].GetRect().h) );
#endif
		int res = st.Update();
	}
//	OutputDebugString( (boost::format("frames=%u\n")%frames.size()).str().c_str() );
}
catch(std::exception& ex)
{
	throw DBBridge::IDBServer::DBServerException(ex.what());
};

void	MixServerImpl::DeleteInfoBeforeTime( time_t BoundTime ) 
{

}