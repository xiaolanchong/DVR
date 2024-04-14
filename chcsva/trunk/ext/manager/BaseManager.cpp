//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Базовый класс - основные операции для реализации интерфейса IStreamManager

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.06.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "BaseManager.h"
#include "ManagerWindow.h"
#include "../ImageGraph/FramePin.h"

// TODO can it be take out off here?
Elvees::CUsesWinsock _WinsockInitializeSentry;

std::tstring UIDtoString( const UUID& uid )
{
	UUID MutableUID = uid;
	UuidString stUID;
	UuidToString( &MutableUID, &stUID );
	std::tstring s( (const TCHAR*)stUID );
	RpcStringFree( &stUID );
	return s;
}

//======================================================================================//
//                                  class BaseManager                                   //
//======================================================================================//

BaseManager::BaseManager(boost::shared_ptr<SystemSettings> pVideoConfig, bool bServerMode, DWORD WorkingMode)
	: m_shutdownEvent(false)
	, m_startedEvent(false)
	, xvidcore(TEXT("xvidcore.dll"))
	, xvidvfw(TEXT("xvidvfw.dll"))
	, m_bServerMode( bServerMode ) 
	, m_WorkingMode( WorkingMode )
	, m_pVideoConfig( pVideoConfig )
{
}

BaseManager::~BaseManager()
{
}

//////////////////////////////////////////////////////////////////////////

bool BaseManager::IsStreamAlreadyAttached(long StreamID)
{
	CHCS::IStream* p = StreamByID( StreamID );
	if( p )
	{
		p->Release();
		return true;
	}
	return false;
}

/////////////// stream collection stuff //////////////////

CHCS::IStream* BaseManager::StreamByID(long lStreamID)
{
	return m_StreamList.StreamByID( lStreamID );
}

CHCS::IStream* BaseManager::StreamByUID(UUID& StreamUID, long lPin)
{
	return m_StreamList.StreamByUID( StreamUID, lPin );
}


bool BaseManager::GetStreams( std::vector<long>& StreamIDs )
{
	return m_StreamList.GetStreams( StreamIDs );
}

//////////// execution control  //////////////////////////

bool BaseManager::Start()
{
	return Elvees::CThread::Start();
}

bool BaseManager::IsStarted()
{
	return m_startedEvent.Wait(0);
}

void BaseManager::WaitStarted()
{
	HANDLE h[2];

	h[0] = m_startedEvent.GetEvent();
	h[1] = GetHandle(); // CThread;

//	WaitForMultipleObjects(2, h, FALSE, INFINITE);

	if(WaitForMultipleObjects(2, h, FALSE, 60000) == WAIT_TIMEOUT)
		Elvees::Output(Elvees::TWarning, TEXT("Manager starting too long time"));
}

void BaseManager::ShutDown()
{
	m_shutdownEvent.Set();
	Wait();
}

////////// main thread function /////////////

int BaseManager::Run()
{
	Elvees::Output(Elvees::TInfo, TEXT("> Manager started"));

	CoInitialize(NULL);

	InitializeWork();

	//////////////////////////////////////////////////////////////////////////
	// Create windows

	std::auto_ptr<ManagerWindow> pManagerWindow;

	if( UseGUIForDebugPurpose() )
	{
		pManagerWindow = std::auto_ptr<ManagerWindow>( new ManagerWindow( IsServer() ) );
	}

	UpdateStreamListInWindow( pManagerWindow.get() );

	m_startedEvent.Set();

	//////////////////////////////////////////////////////////////////////////
	// Main loop

	MSG msg;
	DWORD dwRet;
	HANDLE hShutdown = m_shutdownEvent.GetEvent();

	while(1)
	{
		dwRet = MsgWaitForMultipleObjects(1, &hShutdown, FALSE, INFINITE, QS_ALLINPUT);

		if(dwRet == WAIT_OBJECT_0 + 1)
		{
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT)
				{
					//	m_shutdownEvent.Set();
					break;
				}

				if( !pManagerWindow.get() || !pManagerWindow->IsDialogMessage( &msg )  )
				{
					TranslateMessage(&msg); 
					DispatchMessage(&msg);
				}
			}
		}
		else if(dwRet == WAIT_OBJECT_0) // Shutdown
		{
			pManagerWindow.reset();
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	UninitializeWork();

	CoUninitialize();

	Elvees::Output(Elvees::TInfo, TEXT("> Manager stopped"));
	m_startedEvent.Reset();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void	BaseManager::UpdateStreamListInWindow( ManagerWindow* pWindow )
{
	const std::vector<StreamList::StreamInfo>& Arr = 	m_StreamList.GetStreamInfoArr();

	std::vector<StreamInfo > StreamArr;

	for( size_t i =0; i < Arr.size(); ++i )
	{
		StreamInfo info;
		info.m_bIsLocal		= Arr[i].m_bIsLocal;
		info.m_DevicePin	= Arr[i].m_DevicePin;
		info.m_pStream		= Arr[i].m_pStream.get();
		info.m_stMoniker	= Arr[i].m_stMoniker;
		info.m_StreamID		= Arr[i].m_StreamID;
		info.m_StreamUID	= Arr[i].m_StreamUID;

		StreamArr.push_back( info );
	}

	if( pWindow )
	{
		pWindow->SetStreams( StreamArr );
	}

}

//////////////////////////////////////////////////////////////////////////

LPCTSTR  BaseManager::GetVideoBasePath()
{
	return m_stBasePath;
}




SystemSettings&	BaseManager::GetSystemSettings()
{
	return *m_pVideoConfig.get();
}