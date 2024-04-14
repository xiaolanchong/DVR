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

#include "chcs.h"
#include "CommonProcessFilter.h"
#include "../localvideo/LocalTier.h"

//======================================================================================//
//                              class CommonProcessFilter                               //
//======================================================================================//

CommonProcessFilter::CommonProcessFilter()
{
}

CommonProcessFilter::~CommonProcessFilter()
{
}

void CommonProcessFilter::PushImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxImageSize )
{
	// FIXME put to the client hook filter
	ILocalTier* pTier = GetLocalTier();

	if(pTier)
	{
		pTier->SetFrame( pFrame );
	}

	SendImage( pFrame, bKeyFrame, MaxImageSize );
}