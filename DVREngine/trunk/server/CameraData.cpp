//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс-хранилище всех данных от камер для сообщения MsgHallRequestAlarmData

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.01.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "CameraData.h"

//======================================================================================//
//                                   class CameraData                                   //
//======================================================================================//

CameraData::CameraData() 
{
}

CameraData::~CameraData()
{
}

void	CameraData::AddData( int nCameraID, const ByteArray_t& BinData )
{
	//! ensure we do not rewrite data
//	_ASSERTE( m_CameraData.find( nCameraID ) == m_CameraData.end() );
	m_CameraData[ nCameraID ] = BinData;
}

void	CameraData::ClearData()
{
	m_CameraData.clear();
}

const	ByteArray_t& CameraData::GetData( int nCameraID ) const
{
	std::map<int, ByteArray_t>::const_iterator it = m_CameraData.find( nCameraID );
	if( m_CameraData.end() == it ) throw CameraDataException("No such camera id");
	return it->second;
}

void	CameraData::GetCameraIDArr( std::vector<int>& CameraIDArr ) const
{
	CameraIDArr.clear();
	std::transform( m_CameraData.begin(), m_CameraData.end(),
					std::back_inserter( CameraIDArr ), 
					Extract1st() );
}