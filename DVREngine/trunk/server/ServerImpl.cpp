//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: реализация серверной части

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   16.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ServerImpl.h"

#include "message/MsgCameraRequestData.h"
#include "message/MsgHallSetCameraData.h"
#include "message/MsgHallRequestAlarmData.h"
#include "message/MsgHallResponseAlarmData.h"
#include "message/MsgCameraSetHallData.h"

#include "state/ServerThreadState.h"

#include "tier/ClientServerTier.h"
#include "PlatSleep.h"
#include "ImageProcessPause.h"

#include "ActivityRecord.h"

namespace DVREngine
{

IServer* CreateServerInterface(	const ServerStartSettings & m_ServerSettings)
{
	return new ServerImpl( m_ServerSettings );
}

}

//! получить имя ключа зашиты
//! \param UserKeyName имя, заданное пользователем 
//! \return имя ключа
std::string GetProtectionDeviceName(const std::string& UserKeyName)
{	
	if( !UserKeyName.empty() )
	{
		return UserKeyName;
	}
	LPCTSTR const szRegistryPath = _T("Software\\ElVEES\\DVR");
	LPCTSTR const szParamName	 = _T("ProtectDev");

	CRegKey keyDevice;
	LONG res = keyDevice.Open( HKEY_LOCAL_MACHINE, szRegistryPath );
	if( res != ERROR_SUCCESS )
	{
		return std::tstring();
	}

	TCHAR szDeviceName[4096];
	ULONG NameSize = 4096;
	res = keyDevice.QueryStringValue( szParamName, szDeviceName, &NameSize ); 
	if( res == ERROR_SUCCESS )
	{
		return std::tstring( szDeviceName, szDeviceName + NameSize );
	}
	else
	{
		return std::tstring();
	}
}

//======================================================================================//
//                                   class ServerImpl                                   //
//======================================================================================//

ServerImpl::ServerImpl( const DVREngine::ServerStartSettings& m_ServerSettings )
try
:
	BaseQueueThread( m_ServerSettings.m_pServerMsg ),
	m_ServerSettings( m_ServerSettings ) 

{
	if( m_ServerSentry.AlreadyLaunched() )
	{
//		throw ServerAlreadyLaunchedException("Server application has already launched");
	}

	std::auto_ptr<ServerThreadState> pState( ServerThreadState::GetStartState(this) );
	m_pState = pState;

	std::auto_ptr<ControlledThread>	p( new ControlledThread( boost::bind(&ServerImpl::Handled_ServerThreadProc, this), 1 ) );
	m_Thread = p;
}
catch( ServerVideoFactory::ServerVideoException& ex )
{
	throw ServerException( ex.what() );
}
catch ( DBBridge::IDBServer::DBServerException& ex ) 
{
	throw ServerException( ex.what() );
};

ServerImpl::~ServerImpl()
{
	m_Thread.reset();
	m_FrameActivityStatistics.reset();
}

void	ServerImpl::SendMessage(IMessageStream* pSender, ThreadMessage_t pMsg)
{
	UNREFERENCED_PARAMETER(pSender);
	MessageQueueAutoEnter<ThreadMessage_t> mq( m_Queue );
	mq.Push( pMsg );
}

static int RecordExceptionInfo(...)
{
	return EXCEPTION_EXECUTE_HANDLER;
}

bool	ServerImpl::Handled_ServerThreadProc()
{	
	__try
	{
		return ThreadProc();
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("Main server thread"))
#ifndef _DEBUG
		, EXCEPTION_EXECUTE_HANDLER
#endif
		)
	{
		return false;
	}
}

bool	ServerImpl::ThreadProc()
{
	HRESULT hr = CoInitialize(0);
	std::auto_ptr<ServerThreadState> pState = m_pState->Process();
	if(pState.get()) 
	{
		m_pState = pState;
	}

	//! PORTABILITY
	PlatSleep(20);
	if( S_OK == hr )
	{
		CoUninitialize();
	}
	return false;
}

void	ServerImpl::SendCameraDataToHall(int nCameraID, const ByteArray_t& CamData)
{
//	m_Msg.Print( Elvees::IMessage::mt_debug_info, "[ServerImpl]MsgHallRequestAlarmData" );
#if 0 //log message for debug
	m_Msg.Print( Elvees::IMessage::mt_debug_info, 
		boost::format("[ServerImpl]MsgHallSetCameraData sent" ) );
#endif
	if( !m_HallServer.get() )
	{
		return;
	}

	static_cast<IMessageStream*>(m_HallServer.get())->OnMessage( ThreadMessage_t( new MsgHallSetCameraData( nCameraID, CamData ) ) );
}

void	ServerImpl::SendRequestToHall()
{
	if( !m_HallServer.get() )
	{
		return;
	}

	static_cast<IMessageStream*>(m_HallServer.get())->OnMessage( ThreadMessage_t( new MsgHallRequestAlarmData(  ) ) );
}

void	ServerImpl::SendHallData( const CameraData& CamData, const Frames_t& AlarmFrames )
{
	if( !m_CameraServer.get() )
	{
		return;
	}

	static_cast<IMessageStream*>(m_CameraServer.get())->OnMessage( ThreadMessage_t( new MsgCameraSetHallData( CamData, AlarmFrames ) ) );
}

//////////////////////////////////////////////////////////////////////////


void	ServerImpl::ReceiveHallData( const CameraData& CamData, const Frames_t& FramesForRender)
{
#if 0 // log message for debug
	m_Msg.Print( Elvees::IMessage::mt_debug_info,
		boost::format("[ServerImpl]framesForDatabase#=%u, framesForRender#=%u") 
		% FramesForDatabase.size() 
		% FramesForRender.size() );
#endif

	SetFrames( FramesForRender );
	WriteToDatabase( /*FramesForDatabase*/ FramesForRender );
	SendHallData( CamData, FramesForRender );
}

void	ServerImpl::SetFrames(const Frames_t& Alarms)
{
	GetClientServerTier()->SetCurrentFrames( Alarms );
}

void	ServerImpl::WriteToDatabase(const Frames_t& Alarms)
{
	m_FrameActivityStatistics->ProcessFrames( Alarms );
}

void	ServerImpl::LaunchThreads()
try
{
	boost::shared_ptr< DBBridge::IDBServer > pDBServer( DBBridge::CreateDBServer(0) );

#if 1 // тест
	std::vector<int> CamAll;
	pDBServer->GetCameras(m_CameraIDArr, CamAll);
#else
	m_CameraIDArr.push_back( 1 );
	//	m_CameraIDArr.push_back( 2 );
#endif

	m_CameraServer.reset( new CameraServer( m_ServerSettings.m_VideoSystemMode, m_ServerSettings.m_pServerMsg, m_ServerSettings.m_pVideoMsg, this ) );
	m_HallServer.reset( new HallThread( m_ServerSettings.m_pServerMsg, this ) );

	m_FrameActivityStatistics.reset( new ActivityRecord( pDBServer ) );


	if( m_ServerSettings.m_bLaunchBackup )
	{
		m_pBackupWrapper.reset( new BackupImpl( m_ServerSettings.m_pBackupMsg ) );
	}
	std::sort(m_CameraIDArr.begin(), m_CameraIDArr.end() );

	if( !m_CameraIDArr.empty() ) 
	{
		CameraSettings CamSettings;
		CamSettings.m_LaunchAlgo				= m_ServerSettings.m_bLaunchAlgo;
		CamSettings.m_FpsPeriod					= static_cast<unsigned int>(m_CameraIDArr.size()) * 60;
		CamSettings.m_ProtectionDeviceName		= GetProtectionDeviceName( m_ServerSettings.m_ProtectionKeyName );
		if( m_ServerSettings.m_bLaunchAlgo )
		{
			m_CameraServer->Start( m_CameraIDArr, CamSettings );
			m_HallServer->StartThread( m_CameraIDArr, m_ServerSettings.m_bLaunchAlgo  );
		}
		m_Msg.Print( Elvees::IMessage::mt_info, boost::format("[ServerImpl]Registry KeyName=%s")%CamSettings.m_ProtectionDeviceName );
		m_Msg.Print( Elvees::IMessage::mt_debug_info, "[ServerImpl]Started" );
	}
	else
	{
		m_Msg.Print( Elvees::IMessage::mt_warning, "[ServerImpl]No cameras in the system" );
	}


}
catch ( std::exception&) 
{
	throw;
};

void	ServerImpl::DumpError( const std::string& ErrorDesc )
{
	m_ServerSettings.m_pServerMsg->Print( Elvees::IMessage::mt_error, ErrorDesc.c_str() );
}