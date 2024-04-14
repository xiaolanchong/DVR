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
#include "VideoFrame.h"
#include "SettingsDialog.h"
//======================================================================================//
//                                   class VideoFrame                                   //
//======================================================================================//

VideoFrame::VideoFrame( 
				const std::vector<int>& AvailableCameras,
				DVREngine::IEngineSettingsReader* pParamReader,
				DVREngine::IEngineSettingsWriter* pParamWriter
					   ):
	CameraFrame(  AvailableCameras ),
	m_pParamWriter(pParamWriter),
	m_pParamReader(pParamReader)
{
}

VideoFrame::~VideoFrame()
{
	m_pCanvas = std::auto_ptr<VideoCanvas>();
}

BEGIN_EVENT_TABLE(VideoFrame, CameraFrame) 
	EVT_TOOL( XRCID("ToolButton_Video_Settings"),  VideoFrame::OnSettings )
END_EVENT_TABLE() 

wxSizer*	VideoFrame::GetControlPanelSizer()
{
/*	wxTextCtrl* pWnd = XRCCTRL( *this, "Edit_Zoom", wxTextCtrl );
	_ASSERTE(pWnd);
	return pWnd->GetContainingSizer();*/
	return 0;
}

RenderEventCallback*	VideoFrame::InitRender( IDVRRenderManager* pMainRender )
{
	m_pCanvas = std::auto_ptr<VideoCanvas>(new VideoCanvas( m_LayoutMgr, pMainRender ));
	return m_pCanvas.get();
}

void		VideoFrame::UninitRender()
{
	m_pCanvas = std::auto_ptr<VideoCanvas>();
}

void	VideoFrame::Init( )
{
	CameraFrame::Init();

	std::vector<size_t> LayoutSizes;
	m_LayoutMgr.GetLayoutCellNumber(LayoutSizes);
	std::vector<int> CamIDArr;
	m_LayoutMgr.GetCameras( CamIDArr );
	const size_t CameraNumber = CamIDArr.size();

	std::multimap<size_t, size_t> IndexToSizeMap;
	for ( size_t i =0; i < LayoutSizes.size(); ++i )
	{
		IndexToSizeMap.insert( std::make_pair(LayoutSizes[i], i  ) );
	}
	typedef std::multimap<size_t, size_t>::const_iterator IndexItr_t;
	std::pair<IndexItr_t, IndexItr_t> p = IndexToSizeMap.equal_range( CameraNumber );
	int		LayoutIndex = 0;
	size_t	ArrangedCameraNumber = 0;
	if( p.second == IndexToSizeMap.end() )
	{
		// camera number is greater then layouts' one, arrange not all cameras
		_ASSERTE( LayoutSizes.size() );
		LayoutIndex = LayoutSizes.size() - 1;
		ArrangedCameraNumber = LayoutSizes.back();
	}	
	else if( p.first->first == CameraNumber )
	{
		//! if two numbers equal exactly, use first
		LayoutIndex			 = p.first->second;
		ArrangedCameraNumber = p.first->first;
	}
	else
	{
		// ÿ÷ååê áîëüøå ÷åì êàìåð
		LayoutIndex				= p.second->second;
		ArrangedCameraNumber	= CameraNumber;
	}

	_ASSERTE( ArrangedCameraNumber <= CameraNumber ); 
	m_LayoutMgr.SetLayoutPattern(LayoutIndex);

	std::vector<int> ArrangeCamArr( CamIDArr.begin(), CamIDArr.begin() + ArrangedCameraNumber );
/*	for ( size_t i = 0; i <  ArrangedCameraNumber; ++i )
	{
		m_LayoutMgr.SelectCameraInLayout( i, i );
	}*/
	m_LayoutMgr.SelectCameraInLayout( ArrangeCamArr );

	m_pViewToolBar->ToggleTool( XRCID("ToolButton_ViewMode_1"),						false);
	m_pViewToolBar->ToggleTool( XRCID("ToolButton_ViewMode_1") + LayoutIndex,		true);
}
//////////////////////////////////////////////////////////////////////////

void	VideoFrame::OnSettings(wxCommandEvent &ev)
{
	SettingsDialog dlg( m_pParamReader, m_pParamWriter ) ;

	bool res = wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("Dlg_Settings") );
	_ASSERTE_DESC(res, "Failed to load system settings' dialog");

	dlg.Init();
	dlg.ShowModal();

}