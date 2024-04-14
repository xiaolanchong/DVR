//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: 
//	Реализация IStreamManager для нескольких режимов 
//	1)регистрация устройств
//	2)сервер
//	3)клиент

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   01.07.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "StreamManager.h"

#include "ServerManager.h"
#include "ClientManager.h"
#include "DeviceRegistrar.h"

//FIXME it must now about FreeStreamManager in chcsva.cpp

namespace CHCS
{
extern bool __stdcall FreeStreamManager(BaseManager* _Manager);
}
//======================================================================================//
//                               class StreamManagerImpl                                //
//======================================================================================//

StreamManagerImpl::StreamManagerImpl( BaseManager* pManager ): 
	_Manager(pManager)
{

}

StreamManagerImpl::~StreamManagerImpl() 
{
	CHCS::FreeStreamManager( _Manager );
}

bool __stdcall StreamManagerImpl::GetAvailableStreamIDs(long* pStreamIDs, size_t* pStreamNumber)
{
	std::vector<long> StreamIDs;
	if( pStreamNumber  )
	{
		size_t AvailableSlotsForNumber = *pStreamNumber;
		*pStreamNumber = 0;
		bool res = _Manager->GetStreams(StreamIDs);
		if( res )
		{
			if( !pStreamIDs )
			{
				*pStreamNumber = StreamIDs.size();
			}
			else
			{
				size_t HowMuchCopy = min( AvailableSlotsForNumber, StreamIDs.size() ); 
				*pStreamNumber = HowMuchCopy;
				std::copy( StreamIDs.begin(), StreamIDs.begin() + HowMuchCopy, pStreamIDs );
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else 
	{
		return false;
	}
}

bool __stdcall StreamManagerImpl::GetStreamByID(CHCS::IStream** ppStream, long lStream)
{
	if(!ppStream)
		return false;

	if(_Manager)
	{
		*ppStream = _Manager->StreamByID(lStream);
		return (*ppStream == NULL) ? false : true;
	}

	return false;
}

void __stdcall StreamManagerImpl::Release()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////


StreamManagerServerImpl::StreamManagerServerImpl( BaseManager* pManager ): 
	StreamManagerImpl( pManager )
{

}

bool  __stdcall StreamManagerServerImpl::StartArchiveStream(CHCS::IStream** ppStream, long lStream, INT64 startPos) 
{
	return false;
}

//////////////////////////////////////////////////////////////////////////

StreamManagerClientImpl::StreamManagerClientImpl( ClientManager* pManager ):
	StreamManagerImpl( pManager ),
	m_pManager(pManager)
{

}
bool  __stdcall StreamManagerClientImpl::StartArchiveStream(CHCS::IStream** ppStream, long lStream, INT64 startPos) 
{
	if( !ppStream || ! m_pManager)
	{
		return false;
	}
	*ppStream = m_pManager->CreateArchiveStream( lStream, startPos);
	return *ppStream != 0;
}

StreamManagerRegisterImpl::StreamManagerRegisterImpl(boost::shared_ptr<SystemSettings> ss)
{
	HRESULT hr = CoInitialize(NULL);
	DeviceRegistrar reg;
	reg.FindAndInstallDevices( *ss.get() );
	if( S_OK == hr )
	{
		CoUninitialize();
	}
}