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
#ifndef _LOCAL_CLIENT_STREAM_6376936735334221_
#define _LOCAL_CLIENT_STREAM_6376936735334221_

#include "../../Stream.h"
#include "../../FrmBuffer.h"

using namespace CHCS;
//======================================================================================//
//                               class LocalClientStream                                //
//======================================================================================//

//! \brief Видеопоток для клиента в режиме реалтайм - передача кадра от сервера черех общий буфер
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   13.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class LocalClientStream :	public CStream,
							public CFrmBuffer::Allocator
{
public:
	static CStream*  CreateStream( long StreamID, const UUID& DeviceUID );
private:
	LocalClientStream( long StreamID, const UUID& DeviceUID);

	virtual bool Start()
	{
		return true;
	}

	virtual void Stop()
	{
		// not impl
	}

	virtual bool IsStarted() 
	{
		// not impl
		return true;
	}

	virtual long SeekTo(INT64 timePos) 
	{
		// not impl
		return -1;
	}

	virtual bool GetNextFrame(IFrame** ppFrame, unsigned uTimeout) ;

	virtual long AddRef()
	{
//		long lRefCount = ::InterlockedIncrement(&m_refCount);
//		return lRefCount;
		return CStream::AddRef();
	}

	virtual void Delete()
	{
#ifdef USE_SHIT_LIST
		delete this;
#endif
	}

	virtual long AllocatorSourceID()
	{
		return GetID(); // CStream::GetID()
	}

	virtual ~LocalClientStream();

};

#endif // _LOCAL_CLIENT_STREAM_6376936735334221_