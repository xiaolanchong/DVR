//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: класс взаимодействия с БД - получение списка камер, чтение из архива т.д.

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   31.01.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "DBHandler.h"
#include "../../../DVRDBBridge/trunk/interface/IDBClient.h"

//======================================================================================//
//                                   class DBHandler                                    //
//======================================================================================//

DBHandler::DBHandler()
{
}

DBHandler::~DBHandler()
{
}

void DBHandler::GetCameraArr( std::vector<int>& CameraArrID )
try
{
	boost::shared_ptr<DBBridge::IDBClient> pClient (
		DBBridge::CreateDBClient( 0 )
		)
		;

	std::vector< std::pair<int, std::string> > CamsWithNames;
	pClient->GetCameraArr(CamsWithNames);

	for ( size_t i = 0; i < CamsWithNames.size(); ++i )
	{
		CameraArrID.push_back( CamsWithNames[i].first );
	}
	std::sort( CameraArrID.begin(), CameraArrID.end() );
}
catch ( DBBridge::IDBClient::DBClientException& ex ) 
{

};

void	DBHandler::GetAlarmData( time_t StartTime, time_t EndTime, Frames_t& AlarmDataArr )
try
{
	boost::shared_ptr<DBBridge::IDBClient> pClient (
		DBBridge::CreateDBClient( 0 )
		)
		;
	pClient->GetFrames( StartTime, EndTime, AlarmDataArr );
}
catch ( DBBridge::IDBClient::DBClientException& ex ) 
{
	throw std::runtime_error(ex.what());
};