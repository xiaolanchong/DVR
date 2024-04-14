//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Передача залу информации от камеры

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   14.04.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "MsgHallSetCameraData.h"

//======================================================================================//
//                              class MsgHallSetCameraData                              //
//======================================================================================//

MsgHallSetCameraData::MsgHallSetCameraData(int nCameraID, const ByteArray_t& Data):
	m_nCameraID(nCameraID),
	m_Data(Data)
{
}

MsgHallSetCameraData::~MsgHallSetCameraData()
{
}