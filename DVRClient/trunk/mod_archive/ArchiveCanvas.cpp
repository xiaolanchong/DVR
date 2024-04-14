//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Рендер изображений и информации в режиме архива

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   10.03.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "ArchiveCanvas.h"
#include "../common/DBHandler.h"

//======================================================================================//
//                                 class ArchiveCanvas                                  //
//======================================================================================//

ArchiveCanvas::ArchiveCanvas(
							 LayoutCameraManager& LayoutMgr, 
							 IDVRRenderManager*	pMainRender,
							 boost::function1<void, time_t> fnTimeCallback
							 ):
	CameraCanvas( LayoutMgr, pMainRender ),
	m_pArchiveRender( pMainRender->GetArchiveRender() ) ,
	m_bArchiveStarted(false),
	m_fnTimeCallback( fnTimeCallback )
{
	_ASSERTE(pMainRender);
	_ASSERTE(m_pArchiveRender);
	pMainRender->GetArchiveRender()->BindCallback ( boost::bind(  &ArchiveCanvas::Draw, this ) );
}

ArchiveCanvas::~ArchiveCanvas()
{
	m_pMainRender->GetArchiveRender()->UnbindCallback();
}

//! класс рисования и накопления рамок в режиме архива
class DrawRectangleEx 
{
	IDVRArchiveRender* m_pRender;
	//typedef std::map<int, std::vector<DBBridge::Frame::Rect> > FrameMap_t;
	mutable std::set<int> m_ProcessMap;
	
	RectStorage	m_Storage;

	void	AddFramesFromOneCamera( int nCameraID, const std::vector<unsigned char>& UserData ) 
	{
		m_Storage.AddFramesFromOneCamera(nCameraID, UserData);
	}
public:
	bool operator() ( int nCameraID, const IAreaLayout::Rect& rc ) 
	{
		//return false;
		std::vector<unsigned char> UserFrameData ;
		bool res = m_pRender->DrawFrame( nCameraID, rc.x, rc.y, rc.w, rc.h  );
		if( !res )
		{
			return false;
		}

		if( m_ProcessMap.find( nCameraID ) == m_ProcessMap.end() )
		{
			m_ProcessMap.insert( nCameraID );
			m_pRender->GetFrameUserData( nCameraID, UserFrameData );
			AddFramesFromOneCamera( nCameraID, UserFrameData );
		}
		return true;
	}

	DrawRectangleEx(IDVRArchiveRender* pRender) : 
		m_pRender(pRender)
	{}

	const Frames_t& GetAlarmFrames() 
	{
		return m_Storage.GetAlarmFrames();
	}
};

//! ничего не рисовать
struct DrawNothing
{
	bool operator () (int , const IAreaLayout::Rect& )
	{
		return false;
	}
};

//! нарисовать тестовую 200*200 картинку
class	DrawTestBitmap
{
	IDVRGraphicsWrapper* m_pRender; 
	std::vector<unsigned char>	m_TestBmp;
public:
	DrawTestBitmap( IDVRGraphicsWrapper* pRender) : 
		m_pRender(pRender),
		m_TestBmp( 200 * 200 * 3, 0x80 )
	{
		std::fill_n( m_TestBmp.begin() + 200 * 200 * 3/2,   200 * 200 * 3/2, 0xff );
	}

	bool operator () (int nCameraID, const IAreaLayout::Rect& rc)
	{
		return m_pRender->DrawBitmap( 
			IDVRGraphicsWrapper::Rect( rc.x, rc.y, rc.x + rc.w, rc.y + rc.h ), 
			200, 200, 24, 
			&m_TestBmp[0] 
		);
	}
};

void	ArchiveCanvas::Draw(  )
{
	IDVRGraphicsWrapper* pRender = GetWrapper2D();
	wxSize sizeWindow = GetClientSize();

	if(m_bArchiveStarted)
	{
		DrawRectangleEx ArchiveFrameRender(m_pArchiveRender) ;
		DrawFrames( sizeWindow, pRender, boost::ref(ArchiveFrameRender) );
		DrawAlarmRects( sizeWindow, pRender, ArchiveFrameRender.GetAlarmFrames() );

		time_t ArchiveTime = m_pArchiveRender->GetArchivePosition();
		m_fnTimeCallback(  ArchiveTime );
#if 0
		std::vector<int>	SelectedCameras;
		m_LayoutMgr.GetSelectedCameras( SelectedCameras );
		DrawAlarmRects( sizeWindow, pRender, FramesForCamera );
#endif
	}
	else
	{
		//ClearWindow( pRender );
		DrawFrames( sizeWindow, pRender, 
#if 1	// test for the archive render
			DrawNothing() 
#else
			DrawTestBitmap(pRender)
#endif
			);
	}
}

void	ArchiveCanvas::OnLayoutChange()
{

}

void	ArchiveCanvas::StartArchive( time_t StartTime, time_t EndTime )
{
	std::vector<int> SelectedCameras;
#if 0
	SelectedCameras.push_back(1);
	SelectedCameras.push_back(2);
#else
	m_LayoutMgr.GetSelectedCameras( SelectedCameras );
#endif

	m_pArchiveRender->AcquireArchive( StartTime, EndTime, SelectedCameras );
	m_bArchiveStarted = true;	
}

void	ArchiveCanvas::ReleaseArchive()
{
	m_bArchiveStarted = false;
	m_pArchiveRender->ReleaseArchive();
}

void	ArchiveCanvas::PlayArchive()
{
	m_pArchiveRender->Play();
}

void	ArchiveCanvas::PauseArchive()
{
	m_pArchiveRender->Pause();
}

void	ArchiveCanvas::StopArchive()
{
	m_pArchiveRender->Stop();
}

void	ArchiveCanvas::SeekArchive(time_t NewTime)
{
	m_pArchiveRender->Seek( NewTime );
}

time_t	ArchiveCanvas::GetArchiveTime()
{
	_ASSERTE(m_pArchiveRender);
	return (m_pArchiveRender->GetArchivePosition());
}