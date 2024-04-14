//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Отображает сетку времени с разрешением в 1 мин

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   06.06.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "MinuteTimeLineRender.h"
//======================================================================================//
//                              class MinuteTimeLineRender                              //
//======================================================================================//

MinuteTimeLineRender::MinuteTimeLineRender(size_t IntermediateMarkNumber):
	ITimeLineRender(c_SecondsInMinute / IntermediateMarkNumber,	1.0f  )
{
}

MinuteTimeLineRender::~MinuteTimeLineRender()
{
}