//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Camera analyzer thread actions

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "CameraThread.h"
#include "message\MsgCameraRequestData.h"
#include "message\MsgCameraResponseData.h"
#include "message\MsgCameraSetHallData.h"
#include "message\IMessageDispatcher.h"
#include "PlatSleep.h"
#include "grabber/IVideoProvider.h"
#include "../interface/IArchiveUserData.h"

void	SetProtectionKeyName( Elvees::ICameraAnalyzer* pInt, const std::string& sName )
{
	dvralgo::TProtectionDeviceName protectionDevice;
	protectionDevice.data = sName.c_str();
	pInt->SetData( &protectionDevice );
}

//======================================================================================//
//                                  class CameraThread                                  //
//======================================================================================//

CameraThread::CameraThread(	boost::shared_ptr<Elvees::IMessage> pMsg, 
							int nCameraID,
							IMessageDispatcher* pDsp,
							boost::shared_ptr<IVideoProvider>		pVideoReader,
							const CameraSettings& CamSettings ) : 
	IMessageStream(pDsp),
	m_nCameraID(nCameraID),
	m_pVideoReader(pVideoReader),
	BaseQueueThread(pMsg),
	BaseMessageProcessor( pMsg,  (boost::format("CameraThread#%d") % m_nCameraID ).str() ),
	m_FpsCounter( CamSettings.m_FpsPeriod ) ,
	m_bInvalidImageMode(false)
{
	m_Msg.Print(	Elvees::IMessage::mt_debug_info, 
		boost::format("[CameraThread#%d]LoadAlgo=%u") % m_nCameraID % (CamSettings.m_LaunchAlgo? 1 : 0) );
	BinaryData  BinParamsForAlgo;
	bool res = dvralgo::SetDefaultParameters( & BinParamsForAlgo, pMsg.get() );
	UNREFERENCED_PARAMETER( res ) ;
	if( CamSettings.m_LaunchAlgo )
	{
		m_pCameraAnalyzer = boost::shared_ptr<Elvees::ICameraAnalyzer>
			( dvralgo::CreateCameraAnalyzer(m_nCameraID, &BinParamsForAlgo, 0, pMsg.get() ) );
		if( !m_pCameraAnalyzer.get() ) 
		{
			m_Msg.Print(	Elvees::IMessage::mt_error, 
						boost::format("[CameraThread#%d]Failed to create ICameraAnalyzer interface") % m_nCameraID );
			return;
		}
		SetProtectionKeyName( m_pCameraAnalyzer.get(), CamSettings.m_ProtectionDeviceName );
	}
	m_Msg.Print( Elvees::IMessage::mt_debug_info, boost::format("[CameraThread#%d]Started") % m_nCameraID  );
	std::auto_ptr<ControlledThread> p( new ControlledThread( boost::bind( &CameraThread::DoAction, this ), 1 ) ) ;
	m_Thread = p;
}

CameraThread::~CameraThread(  )
{
	m_Thread.reset();
}

static int RecordExceptionInfo(...)
{
	return EXCEPTION_EXECUTE_HANDLER;
}

bool	CameraThread::Handled_ThreadProc()
{	
	__try
	{
		return DoAction();
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

bool CameraThread::DoAction()
try
{
	if( m_pCameraAnalyzer.get() )
	{
//		if( m_Pause.Tick() )
		{
			ProcessImage();
		}
	}
	else
	{
		PlatSleep( 100 );
	}
	ProcessQueue();

	return false;
}
catch(...)
{
	m_Msg.Print( Elvees::IMessage::mt_error, 
		boost::format("[CameraThread#%d]Unknown exception" ) % m_nCameraID );
	return false;
}

void	CameraThread::ProcessImage()
try
{
	const ubyte* pBytes = 0;
	ulong		 nTime = 0;
	bool res = m_pVideoReader->Read( pBytes, nTime );
	if(	res && 
		m_pCameraAnalyzer.get() &&
		!m_FrameFilter.IsEqual( nTime ) 
		)
	{
	//	m_Msg.Print( Elvees::IMessage::mt_debug_info, "[CameraThread::DoAction]Done.." );
		m_bInvalidImageMode = false;
		bool bRes = m_pCameraAnalyzer->Process( &m_pVideoReader->GetHeader(), pBytes, g_Timer.GetTime() );
		if( !bRes )
		{
			m_Msg.Print( Elvees::IMessage::mt_error, 
				boost::format("[CameraThread#%d]ICameraAnalyzer::Process failed" ) % m_nCameraID );
		}
		dvralgo::TFromCameraToHall data;
		bRes = m_pCameraAnalyzer->GetData(&data);
		if( !bRes )
		{
			m_Msg.Print( Elvees::IMessage::mt_error, 
				boost::format("[CameraThread#%d]ICameraAnalyzer::GetData failed" ) % m_nCameraID );
		}
		else
		{
#if 0
			m_Msg.Print( Elvees::IMessage::mt_debug_info, 
				boost::format("[CameraThread#%d]MsgCameraResponseData sent" ) % m_nCameraID );
#endif
			m_pDispatcher->SendMessage( this, ThreadMessage_t(new MsgCameraResponseData( m_nCameraID, data.data.second ) )  );
		}

		m_FpsCounter.IncrementFrame();
		
	}
	else
	{
		m_bInvalidImageMode = true;
		SetFramesToVideo( Frames_t() );
	}
	boost::optional<double> fps = m_FpsCounter.GetFPS();
	if( fps.get_ptr() )
	{
		m_Msg.Print( Elvees::IMessage::mt_debug_info, 
			boost::format("[CameraThread#%d]fps=%.1lf" ) % m_nCameraID % fps.get() );
	}
}
catch ( Elvees::VideoReaderException& ex ) 
{
	m_Msg.Print( Elvees::IMessage::mt_error, boost::format("[CameraThread::DoAction]VideoReaderException=%s") % ex.what() );
};

void	CameraThread::ProcessQueue()
{
	::ProcessQueue( this, m_Queue );
}

void CameraThread::OnMessage( ThreadMessage_t pMsg )
{
	MessageQueueAutoEnter<ThreadMessage_t> mq( m_Queue );
	mq.Push( pMsg );
}

void	CameraThread::Process( MsgCameraResponseData*		/*p */)
{
	//! такого сообщения быть не должно
	_ASSERTE(false);

	m_Msg.Print( Elvees::IMessage::mt_error, 
		boost::format("[CameraThread#%d]MsgCameraResponseData-inadmissible message" ) % m_nCameraID );
}

void	CameraThread::Process( MsgCameraSetHallData*		p )
{
	//m_Msg.Print( Elvees::IMessage::mt_debug_info, boost::format("[CameraThread#%d]MsgCameraSetHallData") % m_nCameraID );

	SetHallDataToAlgorithm( p->GetHallData() );
	if( !m_bInvalidImageMode )
	{
		SetFramesToVideo( p->GetFrames() );
	}
}

void	CameraThread::SetHallDataToAlgorithm( const CameraData& CamDataFromHall)
{
	if(!m_pCameraAnalyzer.get() )
	{
		return;
	}
//	const CameraData& CamDataFromHall = AllCamDataFromHall->GetHallData();
	std::vector<int> AllCamDataIDArr;
	CamDataFromHall.GetCameraIDArr( AllCamDataIDArr );
	if( std::find( AllCamDataIDArr.begin(), AllCamDataIDArr.end(), GetCameraID()  ) == AllCamDataIDArr.end() )
	{
		// no data for this camera
		return;
	}
	const ByteArray_t BinDataFromHall = CamDataFromHall.GetData( GetCameraID() );
	dvralgo::TFromHallToCamera data( std::make_pair( m_nCameraID,BinDataFromHall ) );
	bool bRes = m_pCameraAnalyzer->SetData( &data );
	if( !bRes )
	{
		m_Msg.Print( Elvees::IMessage::mt_error, 
			boost::format("[CameraThread#%d]ICameraAnalyzer::SetData failed" ) % m_nCameraID );
	}
}

void	CameraThread::SetFramesToVideo( const Frames_t& AlarmFrames)
{
	std::vector<DBBridge::Frame::Rect>	FrameRects;
	for( size_t i =0; i < AlarmFrames.size(); ++i )
	{
		if( AlarmFrames[i].GetCameraID() == GetCameraID() )
		{
			FrameRects.push_back( AlarmFrames[i].GetRect() );
		}
	}

	if( !FrameRects.empty() )
	{
		boost::shared_ptr<IArchiveUserData> p( CreateArchiveUserDataInterface() );
		std::vector<unsigned char> RawData;
		bool res = p->Serialize( FrameRects, time(0), RawData  );
		_ASSERTE(res);

		m_pVideoReader->SetUserDataToVideo( &RawData[0], RawData.size() );
	}
	else
	{
		m_pVideoReader->SetUserDataToVideo( NULL, 0 );
	}
}