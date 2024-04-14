//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Фильтр для передачи кадров от сервера клиенту через общий буфер
//	(клиент и сервер в одном процессе)

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   03.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _COMMON_PROCESS_FILTER_9431011354853180_
#define _COMMON_PROCESS_FILTER_9431011354853180_

#include "FramePin.h"

//======================================================================================//
//                              class CommonProcessFilter                               //
//======================================================================================//

//! \brief Фильтр для передачи кадров от сервера клиенту через общий буфер
//	(клиент и сервер в одном процессе)
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   03.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class CommonProcessFilter : public IntermediateFilter
{
public:
	CommonProcessFilter();
	virtual ~CommonProcessFilter();

	virtual void PushImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxImageSize );
};

#endif // _COMMON_PROCESS_FILTER_9431011354853180_