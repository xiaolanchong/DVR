//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Not available

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   22.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "CameraCanvas.h"
#include "../res/Resource.h"
#include "RenderStub/DVRRenderStub.h"
#include "layout/LayoutManager.h"
#include "IMenuHandler.h"

const wxColour	c_clBorder	( 64,  64,  64);
const wxColour	c_clAlarm	( 255, 0,   0);
const wxColour	c_clText	( 255, 255, 255);
const wxColour	c_clNoImage ( 255, 0,   0 );

const int		c_AlarmBorderWidth = 1;
const int		c_FrameBorderWidth = 1;

//======================================================================================//
//                                  class CameraCanvas                                  //
//======================================================================================//

CameraCanvas::CameraCanvas(	 LayoutCameraManager& LayoutMgr, 
						     IDVRRenderManager*	pMainRender):
   m_LayoutMgr( LayoutMgr ),
   m_pMainRender(pMainRender) ,
   m_pMenuHandler(0)
{
}

CameraCanvas::~CameraCanvas()
{
	
}

wxWindow*	CameraCanvas::GetWindow()
{
	return m_pMainRender->GetWindow();
}

IDVRGraphicsWrapper* CameraCanvas::GetWrapper2D()
{
	return m_pMainRender->GetWrapper();
}

void CameraCanvas::OnRButtonHandler( wxMouseEvent& ev )
{
	wxSize s = GetClientSize();
	LayoutCameraManager::WindowClick_t Ids = m_LayoutMgr.ProcessRClick( ev.GetX(), ev.GetY(), s.GetWidth(), s.GetHeight() );
	const std::vector<int>&  IdArr = boost::get<0>(Ids);
	if( IdArr.empty() ) return;
	m_nWindowID = boost::get<2>(Ids);
	wxMenu menu;
	boost::format fmt(_("Camera %d"));

	int nID = ID_Menu_Camera_1;
	if( LayoutCameraManager::InvalidCameraIndex == boost::get<1>(Ids) )
	{
		menu.AppendCheckItem( nID, _("None"), wxT("") );  
		menu.Check( nID, true );
	}
	else 
		menu.Append( nID, _("None"), wxT("") );

	for( size_t i = 0; i < IdArr.size(); ++i )
	{
		std::string s = (fmt % IdArr[i]).str();
		int nID = ID_Menu_Camera_1 + static_cast<int>(i) + 1;
		if( i == boost::get<1>(Ids) )
		{
			menu.AppendCheckItem( nID, s.c_str(), wxT("") );  
			menu.Check( nID, true );
		}
		else
			menu.Append( nID, s.c_str(), wxT("") );
	}

	if( m_pMenuHandler )
	{
		m_pMenuHandler->AddItemsToMenu( &menu, 1000 + ID_Menu_Camera_1 );
	}
	GetWindow()->PopupMenu( &menu, ev.GetX(), ev.GetY() );

	//Refresh();
}

void CameraCanvas::OnMenuHandler( wxCommandEvent& ev )
{
	int nIndex = ev.GetId() - ID_Menu_Camera_1;

	if( nIndex >= 1000 && m_pMenuHandler )
	{
		m_pMenuHandler->ProcessMenuItem( ev.GetId() );
	}
	if( nIndex == 0 )
		m_LayoutMgr.ProcessCameraMenu( m_nWindowID, LayoutCameraManager::InvalidCameraIndex );
	else
		m_LayoutMgr.ProcessCameraMenu( m_nWindowID, nIndex - 1);
	OnLayoutChange();
//	Refresh();
}

void CameraCanvas::OnSizeHandler( wxSizeEvent& ev  )
{
	wxSize sizeWindow = ev.GetSize();
	IDVRGraphicsWrapper* pRender = GetWrapper2D();
	pRender->SetViewport( sizeWindow.GetWidth(), sizeWindow.GetHeight() );
}

//////////////////////////////////////////////////////////////////////////

float Pixel2RelX( size_t nPixel, const wxSize&  sizeWindow)
{
	return nPixel * 1.0f/ sizeWindow.GetWidth();
}

float Pixel2RelY( size_t nPixel, const wxSize&  sizeWindow)
{
	return nPixel * 1.0f/ sizeWindow.GetHeight();
}

void	CameraCanvas::DrawFrames( wxSize sizeWindow, IDVRGraphicsWrapper* pRender, 
									boost::function2<bool, int, IAreaLayout::Rect> f  )
{
	ClearWindow( pRender );

	std::vector<std::pair<int, IAreaLayout::Rect> > LayoutRects;
	std::vector< IAreaLayout::Rect >				EmptyRects;
	m_LayoutMgr.GetLayout( LayoutRects, EmptyRects, sizeWindow.x, sizeWindow.y );

	std::auto_ptr<IDVRGraphicsWrapper::IFont> pFont( pRender->CreateFont( "AnyName" ) );
	for ( size_t i =0; i < LayoutRects.size(); ++i )
	{
		IAreaLayout::Rect & rc = LayoutRects[i].second;
		bool  res = f(  LayoutRects[i].first, rc );
		if(!res )
		{
			DrawFaultFrame( pRender, rc.x, rc.y, rc.x + rc.w, rc.y + rc.h );
		}

		IDVRGraphicsWrapper::Rect rcDraw( rc.x, rc.y, rc.x + rc.w, rc.y + rc.h );

		std::string sCameraName = GetCameraName( LayoutRects[i].first );
		pRender->DrawText(	pFont.get(),
							sCameraName.c_str(), 
							rcDraw,
							IDVRGraphicsWrapper::TA_NorthWest ,
							wxColour(255, 255, 255)
							);
	}
	
	//! пустые кадры, если не назначена камера
	std::auto_ptr<IDVRGraphicsWrapper::IPen>	pPen(pRender->CreatePen( c_clBorder , c_FrameBorderWidth ));
	for ( size_t i = 0; i < EmptyRects.size(); ++i  )
	{
		IAreaLayout::Rect & rc = EmptyRects[i];
		DrawEmptyFrame( pRender, sizeWindow, rc.x, rc.y, rc.x + rc.w, rc.y + rc.h );
	}

}

std::string CameraCanvas::GetCameraName(int CameraID)
{
	std::string s;
#if 0 // memleak
//	 s = boost::lexical_cast<std::string>(CameraID);
	 std::stringstream interpreter;
	 return s;
#else
	char Buf[255];
	itoa( CameraID, Buf, 10 ) ;
	s = Buf;
#endif
	return s;
}

template<typename T> T clamp(T b, T t, T v)
{
	if( v < b ) 
	{
		return b;
	}
	else if( v > t )	
	{
		return t;
	}
	return v;
}

IAreaLayout::Rect	TransformToAlarmRect( const IAreaLayout::Rect& rcCamera, const DBBridge::Frame::Rect& rcAlarm )
{
	const float MaxX = 1.0f;
	const float MaxY = 1.0f;

	float w = clamp( 0.0f, 1.0f, rcAlarm.w/MaxX ) * rcCamera.w;
	float h = clamp( 0.0f, 1.0f, rcAlarm.h/MaxY ) * rcCamera.h;
	float x = clamp( 0.0f, 1.0f, rcAlarm.x/MaxX ) * rcCamera.w + rcCamera.x;
	float y = clamp( 0.0f, 1.0f, rcAlarm.y/MaxY ) * rcCamera.h + rcCamera.y;
	return IAreaLayout::Rect( x, y, w , h );
}

void	CameraCanvas::DrawAlarmRects( 
									 wxSize sizeWindow, 
									 IDVRGraphicsWrapper* pRender, 
									 const Frames_t& CameraAlarmRects )
{
	std::vector<std::pair<int, IAreaLayout::Rect> > LayoutRects;
	std::vector< IAreaLayout::Rect >				EmptyRects;
	m_LayoutMgr.GetLayout( LayoutRects, EmptyRects, sizeWindow.x, sizeWindow.y );
	//! связь многих со многим (много камер, много ситуаций)
	//! составляем словарь всех отображаемых прямоугольников, упорядоченных по камере
	std::multimap<int, IAreaLayout::Rect> RectMap;
	for ( size_t i =0; i < LayoutRects.size(); ++i )
	{
		RectMap.insert( std::make_pair( LayoutRects[i].first, LayoutRects[i].second ) );
	}

	std::auto_ptr<IDVRGraphicsWrapper::IPen>	pPen(pRender->CreatePen( c_clAlarm , c_AlarmBorderWidth ));

	for ( size_t i =0; i < CameraAlarmRects.size(); ++i )
	{
		//! массив областей для одной камеры
		std::pair<	std::multimap<int, IAreaLayout::Rect>::const_iterator,
			std::multimap<int, IAreaLayout::Rect>::const_iterator >
			pIt = RectMap.equal_range( CameraAlarmRects[i].GetCameraID() );
		//! по каждому аларму определим набор, в какую область отображать
		for(std::multimap<int, IAreaLayout::Rect>::const_iterator it = pIt.first; it != pIt.second ; ++it )
		{
			const IAreaLayout::Rect & rc = it->second;
			IAreaLayout::Rect rcScreen = TransformToAlarmRect( rc, CameraAlarmRects[i].GetRect() );
			pRender->DrawRect(	pPen.get(),
								IDVRGraphicsWrapper::Rect(
									rcScreen.x, 
									rcScreen.y, 
									rcScreen.x + rcScreen.w, 
									rcScreen.y + rcScreen.h
								)
							);
		/*	pRender->DrawLine( pPen.get(), 
				rcScreen.x, 
				rcScreen.y, 
				rcScreen.x + rcScreen.w, 
				rcScreen.y + rcScreen.h
				);*/
		}
	}
}

void	CameraCanvas::DrawFaultFrame(IDVRGraphicsWrapper* pRender, float x0, float y0, float x1, float y1)
{
	std::auto_ptr<IDVRGraphicsWrapper::IPen>	pPen(pRender->CreatePen( c_clBorder , c_FrameBorderWidth ));
//	std::auto_ptr<IDVRGraphicsWrapper::IBrush>	pBrush(pRender->CreateBrush( wxColour(0, 0, 255) ));
	std::auto_ptr<IDVRGraphicsWrapper::IFont>	pFont(pRender->CreateFont( "Arial" ));

//	pRender->FillRect( pBrush.get(),	IDVRGraphicsWrapper::Rect( x0, y0, x1, y1 ) );
	pRender->DrawRect( pPen.get(),		IDVRGraphicsWrapper::Rect( x0, y0, x1, y1) );
	pRender->DrawText( pFont.get(), _("No image"), 
						IDVRGraphicsWrapper::Rect( x0, y0, x1, y1), 
						IDVRGraphicsWrapper::TA_Center,
						c_clNoImage);
}

void	CameraCanvas::DrawEmptyFrame(IDVRGraphicsWrapper* pRender, wxSize sizeWindow, float x0, float y0, float x1, float y1)
{
	std::auto_ptr<IDVRGraphicsWrapper::IPen>	pPen(pRender->CreatePen( c_clBorder , c_FrameBorderWidth ));
	pRender->DrawRect(	pPen.get(),
		IDVRGraphicsWrapper::Rect(
		x0, 
		y0, 
		x1 - Pixel2RelX( 1, sizeWindow ), 
		y1 - Pixel2RelY( 1, sizeWindow )
		)
		);
}

void	CameraCanvas::ClearWindow(IDVRGraphicsWrapper* pRender)
{
	wxColour cl = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	pRender->Clear( cl );
}