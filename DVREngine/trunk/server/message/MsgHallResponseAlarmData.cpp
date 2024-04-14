//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Передача данных для алгоритма зала, зал->сервер

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   20.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "MsgHallResponseAlarmData.h"
//======================================================================================//
//                              class MsgHallResponseAlarmData                          //
//======================================================================================//

MsgHallResponseAlarmData::MsgHallResponseAlarmData
	(const CameraData& DataForCamera, 
	const Frames_t& FramesForRender,
	const Frames_t& FramesForDatabase):
	m_DataForCamera(DataForCamera),
	m_FramesForRender( FramesForRender ) ,
	m_FramesForDatabase(FramesForDatabase)
{
}

MsgHallResponseAlarmData::~MsgHallResponseAlarmData()
{
}