//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Взаимодействие с IDVRRender: отрисовка кадров и информации

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   09.03.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "VideoCanvas.h"

//======================================================================================//
//                                  class VideoCanvas                                   //
//======================================================================================//

VideoCanvas::VideoCanvas(LayoutCameraManager& LayoutMgr, IDVRRenderManager* pMainRender ):
	CameraCanvas( LayoutMgr, pMainRender )
{
	m_pMainRender->GetStreamRender()->BindCallback(  boost::bind(  &VideoCanvas::Draw, this ) );
}

VideoCanvas::~VideoCanvas()
{
	m_pMainRender->GetStreamRender()->UnbindCallback();
}

void	VideoCanvas::Draw()
{
	wxSize sizeWindow = GetClientSize();
	IDVRGraphicsWrapper* pRender = GetWrapper2D();
#if _DEBUG
	HANDLE h = GetProcessHeap();
	if ( !h  || !HeapValidate( h, HEAP_NO_SERIALIZE, 0 ) )
	{
		OutputDebugStringA("Help\n");
//		DebugBreak();
	}
#endif
	DrawFrames( sizeWindow, pRender, boost::bind(  &VideoCanvas::DrawCameraFrame, this, _1, _2 ) );
	RectStorage st;
	CameraMap_t::const_iterator it = m_Cameras.begin();
	for ( ; it != m_Cameras.end() ; ++it )
	{
		st.AddFramesFromOneCamera(  
			it->first,
			it->second->GetFrameUserData()
			);
	}
	DrawAlarmRects( sizeWindow, pRender, st.GetAlarmFrames() );
}

bool	VideoCanvas::DrawCameraFrame(int nCameraID, const IAreaLayout::Rect& rc)
{

	IDVRStreamRender* pRender  = m_pMainRender->GetStreamRender();

	CameraMap_t::iterator it = m_Cameras.find( nCameraID );
	if( it == m_Cameras.end() )
	{
		IDVRStreamRender::ICamera* pCam = pRender->GetCamera(nCameraID);
		if( !pCam )
		{
			return false;
		}
		boost::shared_ptr<IDVRStreamRender::ICamera> p( pCam ) ;
		it = m_Cameras.insert( std::make_pair(	nCameraID, p ) ).first;
	}
	if( it->second.get() )
	{
		return it->second->Draw( rc.x, rc.y, rc.w, rc.h );
	}
	else
	{
		return false;
	}
	return 0;
}