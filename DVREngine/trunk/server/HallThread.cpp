//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Hall analyzer thread actions

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "HallThread.h"
#include "message/MsgHallRequestAlarmData.h"
#include "message/MsgHallResponseAlarmData.h"
#include "message/MsgHallSetCameraData.h"
#include "message/IMessageDispatcher.h"
#include "PlatSleep.h"
#include "GlobalTimer.h"

#include "stub/HallAnalyzerStub.h"


//======================================================================================//
//                                   class HallThread                                   //
//======================================================================================//

HallThread::HallThread(boost::shared_ptr<Elvees::IMessage> pMsg, 
					   IMessageDispatcher* pDsp
					    ) : 
	IMessageStream(pDsp),
	BaseQueueThread(pMsg),
	BaseMessageProcessor( pMsg, "HallThread"),
	m_pAnalyzer ( new HallAnalyzerStub() )
{

}

HallThread::~HallThread()
{
}

void HallThread::StartThread(const std::vector<int>& CameraIDArr, bool bLaunchAlgo  )
{
	m_CameraIDArr = CameraIDArr; 

	boost::shared_ptr<Elvees::IMessage> pMsg = m_Msg.Get();

	BinaryData data;
	bool res = dvralgo::SetDefaultParameters( &data, pMsg.get() );
	if (!res)
	{
		m_Msg.Print(	Elvees::IMessage::mt_error, 
			boost::format("[HallThread]SetDefaultParameters failed"));
		return;
	}
	if( bLaunchAlgo )
	{
		m_pAnalyzer = std::auto_ptr<Elvees::IHallAnalyzer>( dvralgo::CreateHallAnalyzer(&data, pMsg.get()) ); 
	}
	if( !m_pAnalyzer.get() )
	{
		MessageImpl msg (pMsg);
		msg.Print(	Elvees::IMessage::mt_error, 
			boost::format("[HallThread]Failed to create IHallAnalyzer interface") );
		m_pAnalyzer.reset( new HallAnalyzerStub );
		return;
	}
	std::auto_ptr<ControlledThread> p( new ControlledThread( boost::bind( &HallThread::Handled_ThreadProc, this ), 1 ) );
	m_Thread = p;
	m_Msg.Print( Elvees::IMessage::mt_debug_info, "[HallThread]Started" );
}

static int RecordExceptionInfo(...)
{
	return EXCEPTION_EXECUTE_HANDLER;
}

bool	HallThread::Handled_ThreadProc()
{	
	__try
	{
		return DoAction();
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("Hall server thread"))
#ifndef _DEBUG
		, EXCEPTION_EXECUTE_HANDLER
#endif
		)
	{
		return false;
	}
}

bool HallThread::DoAction()
try
{
	PlatSleep( 10 );
	ProcessQueue();

	return false;
}
catch(...)
{
	m_Msg.Print( Elvees::IMessage::mt_error, boost::format("[HallThread]Unknown exception" )  );
	return false;
}

void	HallThread::ProcessQueue()
{
	::ProcessQueue( this, m_Queue );
}

void HallThread::OnMessage( boost::shared_ptr<IThreadMessage> pMsg )
{
	MessageQueueAutoEnter<ThreadMessage_t> mq( m_Queue );
	mq.Push( pMsg );
}

void	HallThread::Process( MsgHallSetCameraData*	p	 )
{
#if 0 // log message for debug
	m_Msg.Print( Elvees::IMessage::mt_debug_info, "[HallThread]MsgHallSetCameraData" );
#endif
	int nCameraID = p->GetCameraID();
	const ByteArray_t & InputCamData = p->GetCameraData();
	std::vector<int> CameraIDArr;
	dvralgo::TFromCameraToHall data;
	data.data = std::make_pair( nCameraID, InputCamData );
	bool res = m_pAnalyzer->SetData( &data );
	if(!res)
	{
		_ASSERTE(false);
	}
	
	res = m_pAnalyzer->Process( g_Timer.GetTime() );
	if(!res)
	{
		_ASSERTE(false);
	}
}

void	HallThread::Process( MsgHallRequestAlarmData*	p )
{
	UNREFERENCED_PARAMETER(p);
#if 0 // log message for debug
	m_Msg.Print( Elvees::IMessage::mt_debug_info, "[HallThread]MsgHallRequestAlarmData" );
#endif
	Frames_t framesForDatabase, framesForRender;
	CameraData OutputCamData;

	GetFrames(framesForDatabase, framesForRender);
#if 0 // log message for debug
	m_Msg.Print( Elvees::IMessage::mt_debug_info,
				boost::format("[HallThread]framesForDatabase#=%u, framesForRender#=%u") 
				 % framesForDatabase.size() 
				 % framesForRender.size() );
#endif
	GetCameraData( m_CameraIDArr, OutputCamData );
	m_pDispatcher->SendMessage( this, ThreadMessage_t( new MsgHallResponseAlarmData(OutputCamData, framesForRender, framesForDatabase) ) );
}

void	AddAlarmRect( 
					 const Elvees::ICameraMotionEvent::MotionRectArr& RectArr, 
					 Frames_t& frames, 
					 int nSensorID 
					 ) ;

void	HallThread::GetFrames(  Frames_t& framesForDatabase,  Frames_t& framesForRender )
{
	dvralgo::TEventPtrs			EventPtr;

	bool res = m_pAnalyzer->GetData( &EventPtr );
	if(!res)
	{
		_ASSERTE(false);
	}
	
	framesForRender.clear();
	framesForDatabase.clear();
	const Elvees::IEventPtrArr& events = EventPtr.data;
	for ( size_t i = 0; i < events.size(); ++i )
	{
		Elvees::ICameraMotionEvent* pCamMotionEvent = dynamic_cast<Elvees::ICameraMotionEvent*> (events[i].get());
		if( pCamMotionEvent )
		{
			const Elvees::ICameraMotionEvent::MotionRectArr& RectArrForLastSecond =  pCamMotionEvent->GetLastSecondMotionRectangles();
			AddAlarmRect( RectArrForLastSecond, framesForRender, pCamMotionEvent->SensorId() );
			if( pCamMotionEvent->IsClosed() )
			{
				const Elvees::ICameraMotionEvent::MotionRectArr& RectArrAllFrames =  pCamMotionEvent->GetMotionRectangles();
				AddAlarmRect( RectArrAllFrames, framesForDatabase, pCamMotionEvent->SensorId() );
			}
		}
	}
#if 0
	frm.push_back( DBBridge::Frame( 1, 0, 0, DBBridge::Frame::Rect(50, 75, 100, 120 ) ) );
	frm.push_back( DBBridge::Frame( 2, 0, 0, DBBridge::Frame::Rect(75, 100, 120, 150 ) ) );
#endif
}

void	HallThread::GetCameraData( const std::vector<int>& CamArr, CameraData& cd )
{
	cd.ClearData();
	for ( size_t i = 0; i < CamArr.size(); ++i )
	{
		dvralgo::TFromHallToCamera	toCamera( std::make_pair( CamArr[i], BinaryData() ) );
		bool res = m_pAnalyzer->GetData( &toCamera );
		if(!res)
		{
			_ASSERTE(false);
		}
		cd.AddData( CamArr[i], toCamera.data.second );
	}
}

void	AddAlarmRect( const Elvees::ICameraMotionEvent::MotionRectArr& RectArr, Frames_t& frames, int nSensorID ) 
{
	for ( Elvees::ICameraMotionEvent::MotionRectArr::const_iterator it = RectArr.begin();
		it != RectArr.end() ; ++it )
	{
		DBBridge::Frame OneFrame(	
			nSensorID, 
			it->time, it->time, 
			DBBridge::Frame::Rect(	it->x1, it->y1, it->x2 - it->x1, it->y2 - it->y1 ) 
			);
		frames.push_back( OneFrame );
	}
}