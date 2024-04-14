//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Кроссплатформенная ф-я приостановка потока на мс

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   04.04.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "PlatSleep.h"

void PlatSleep(unsigned int ms)
{
#ifdef UNIX_RELEASE
	usleep( ms * 1000 );
#else
	Sleep(ms);
#endif
}