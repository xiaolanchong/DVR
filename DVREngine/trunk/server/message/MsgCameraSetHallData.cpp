//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Передача данных зала камерам, сервер->камера

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   20.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "MsgCameraSetHallData.h"
//======================================================================================//
//                              class MsgCameraSetHallData                              //
//======================================================================================//

MsgCameraSetHallData::MsgCameraSetHallData(const CameraData& d, const Frames_t& AlarmFrames):
	m_DataArr(d),
	m_AlarmFrames(AlarmFrames)
{
}

MsgCameraSetHallData::~MsgCameraSetHallData()
{
}