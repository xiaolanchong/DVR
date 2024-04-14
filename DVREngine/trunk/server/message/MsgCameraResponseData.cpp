//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Передача данных для зала, камера->сервер

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   20.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "MsgCameraResponseData.h"
//======================================================================================//
//                                class MsgCameraSetData                                //
//======================================================================================//

MsgCameraResponseData::MsgCameraResponseData(int nCameraID, const ByteArray_t& Arr):
	m_nCameraID(nCameraID),
	m_CameraArr(Arr)
{
}

MsgCameraResponseData::~MsgCameraResponseData()
{
}