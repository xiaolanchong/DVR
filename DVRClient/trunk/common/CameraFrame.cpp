//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Common mode frame

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   14.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "CameraFrame.h"
#include "../res/Resource.h"
//======================================================================================//
//                                  class CameraFrame                                   //
//======================================================================================//

CameraFrame::CameraFrame(const std::vector<int>& AvailableCameras):
	m_pViewToolBar(0),
	m_LayoutMgr( AvailableCameras ),
	m_LayoutKeeper("Video")
{
	m_LayoutMgr.SetScale(0);
	m_LayoutMgr.SetLayoutPattern(0);
}

CameraFrame::~CameraFrame()
{
	
}

BEGIN_EVENT_TABLE(CameraFrame, wxPanel) 
	EVT_TOOL( XRCID("ToolButton_ViewMode_FullScreen"),  CameraFrame::OnFullScreen )

	EVT_TOOL( XRCID("ToolButton_ViewMode_1"), CameraFrame::OnViewMode )
	EVT_TOOL( XRCID("ToolButton_ViewMode_2"), CameraFrame::OnViewMode )
	EVT_TOOL( XRCID("ToolButton_ViewMode_3"), CameraFrame::OnViewMode )
	EVT_TOOL( XRCID("ToolButton_ViewMode_4"), CameraFrame::OnViewMode )
	EVT_TOOL( XRCID("ToolButton_ViewMode_5"), CameraFrame::OnViewMode )
	EVT_TOOL( XRCID("ToolButton_ViewMode_6"), CameraFrame::OnViewMode )

	EVT_TOOL( XRCID("ToolButton_StretchMode_1"), CameraFrame::OnStretchMode )
	EVT_TOOL( XRCID("ToolButton_StretchMode_2"), CameraFrame::OnStretchMode )
	EVT_TOOL( XRCID("ToolButton_StretchMode_3"), CameraFrame::OnStretchMode )	

	
END_EVENT_TABLE() 

void CameraFrame::Init()
{
	m_pViewToolBar = XRCCTRL( *this, wxT("ToolBar_ViewMode"), wxToolBar );
	_ASSERTE(m_pViewToolBar);

	m_pRenderContainerWnd = XRCCTRL( *this, wxT("ID_PANEL"), wxPanel );

	m_pViewToolBar->ToggleTool( XRCID("ToolButton_ViewMode_1"),		true);
	m_pViewToolBar->ToggleTool( XRCID("ToolButton_StretchMode_1"),	true);

	//HACK
	SetMinSize( wxSize(500, 400) );
}

void	CameraFrame::SetRenderWindow( wxWindow* pRenderWindow, bool bSet )
{
	if(!bSet)
	{
		wxSizer* p = m_pRenderContainerWnd->GetSizer();
		if(p)
		{
			p->Detach( pRenderWindow );
		//	delete p;
		}
		m_pRenderContainerWnd->SetSizer( 0 );
	}
	else
	{
	
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	topsizer->Add(
		pRenderWindow,
		1,            // make vertically stretchable
		wxEXPAND |    // make horizontally stretchable
		wxALL,        //   and make border all around
		0 );         // set border width to 0

	m_pRenderContainerWnd->SetSizer( topsizer, false );      // use the sizer for layout
	m_pRenderContainerWnd->Layout();
	}
}

void CameraFrame::OnViewMode(wxCommandEvent& ev)
{
	long nID = ev.GetId();
	int PrevToggled = -1;
	for( long i = XRCID("ToolButton_ViewMode_1"); i <= XRCID("ToolButton_ViewMode_6"); ++i )
	{
		if( m_pViewToolBar->GetToolState( i ) && i != nID )
		{
			PrevToggled = i - XRCID("ToolButton_ViewMode_1");
		}
		m_pViewToolBar->ToggleTool( i, i == nID );
	}

	std::vector<int> Snapshot;
	m_LayoutMgr.GetLayoutSnapshot(Snapshot);

	if( PrevToggled != -1 )
	{
		m_LayoutKeeper.SaveLayout( PrevToggled, Snapshot );
	}

	int NewLayoutIndex = nID - XRCID("ToolButton_ViewMode_1");
	m_LayoutMgr.SetLayoutPattern(NewLayoutIndex);

	std::vector<int> PrevSelectedCamIDArr;
	m_LayoutKeeper.LoadLayout( NewLayoutIndex, PrevSelectedCamIDArr );
	if( !PrevSelectedCamIDArr.empty() )
	{
		m_LayoutMgr.SelectCameraInLayout(  PrevSelectedCamIDArr );
	}
}

void CameraFrame::OnStretchMode(wxCommandEvent& ev)
{
	long nID = ev.GetId();
	for( long i = XRCID("ToolButton_StretchMode_1"); i <= XRCID("ToolButton_StretchMode_3"); ++i )
	{
		m_pViewToolBar->ToggleTool( i, i == nID );
	}
	m_LayoutMgr.SetScale(nID - XRCID("ToolButton_StretchMode_1"));
//	m_pCanvas->Refresh();
}

void CameraFrame::OnFullScreen(wxCommandEvent& ev)
{
	bool bFullScreen = ev.IsChecked();
	wxFrame* p = dynamic_cast< wxFrame* >( GetParent() );
	_ASSERTE(p);
	wxStatusBar*	pStatus		= p->GetStatusBar();
	wxToolBar*		pToolbar	= p->GetToolBar(); 
	if( pStatus )
	{
		pStatus->Show( !bFullScreen );
	}
	if( pToolbar )
	{
		pToolbar->Show( !bFullScreen );
	}

	long nStyle = p->GetWindowStyle();
	const long c_Styles = (wxCAPTION | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxBORDER | wxTHICK_FRAME);
	nStyle = bFullScreen ? nStyle & ~c_Styles : nStyle | c_Styles;
	p->SetWindowStyle(nStyle );
	PreFullScreen(bFullScreen);
	p->Maximize(bFullScreen);
	p->Layout();
	p->Refresh();
}

//////////////////////////////////////////////////////////////////////////

void		CameraFrame::Invalidate()
{
	Refresh();
}


