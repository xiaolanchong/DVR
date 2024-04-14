//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Видеопоток для клиента в режиме реалтайм - передача кадра от сервера

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   13.04.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "LocalClientStream.h"
#include "LocalTier.h"

//======================================================================================//
//                               class LocalClientStream                                //
//======================================================================================//

LocalClientStream::LocalClientStream( long StreamID, const UUID& DeviceUID):
	CStream( DeviceUID, 0 ) ,
	CFrmBuffer::Allocator(5)
{
	SetID( StreamID ); 
}

LocalClientStream::~LocalClientStream()
{
}

bool LocalClientStream::GetNextFrame(IFrame** ppFrame, unsigned uTimeout) 
{
	if( !ppFrame )
	{
		return false;
	}
	*ppFrame = GetLocalTier()->GetFrame( GetID() );
	return NULL != *ppFrame;
//	return 0;
}

CStream*  LocalClientStream::CreateStream( long StreamID, const UUID& StreamUID )
{
	return new LocalClientStream( StreamID, StreamUID );
}