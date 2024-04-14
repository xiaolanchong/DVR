//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Video mode tab window

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   14.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _VIDEO_FRAME_4987744442145570_
#define _VIDEO_FRAME_4987744442145570_

#include "../common/CameraFrame.h"
#include "VideoCanvas.h"


//======================================================================================//
//                                   class VideoFrame                                   //
//======================================================================================//

//! \brief Video mode tab window
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   24.03.2006
//! \version 2.0
//! \bug 
//! \todo 

class VideoFrame : public CameraFrame
{
	//! обертка режима рендера реального видео
	std::auto_ptr<VideoCanvas>		m_pCanvas;

	DVREngine::IEngineSettingsReader* m_pParamReader;
	DVREngine::IEngineSettingsWriter* m_pParamWriter;

	virtual void					Init(  );
public:
	VideoFrame(const std::vector<int>& AvailableCameras, 
				DVREngine::IEngineSettingsReader* pParamReader,
				DVREngine::IEngineSettingsWriter* pParamWriter
				);
	virtual ~VideoFrame();

	//! установить тревожн-е ситуации
	void	SetFrames( const Frames_t& ca ) 
	{  
#if 1
		if(m_pCanvas.get())
		{
			m_pCanvas->SetFrames( ca );
		}
#endif
	};

	//! инициализация режима рендера
	virtual RenderEventCallback*	InitRender( IDVRRenderManager* pMainRender );
	//! деинициализация рендера
	virtual void					UninitRender();

	void	SetMenuHandler( IMenuHandler* pMenuHandler)
	{
		m_pCanvas->SetMenuHandler( pMenuHandler );
	}
	
private:
	//! получить панель управления справа
	virtual wxSizer*	GetControlPanelSizer();

	void	OnSettings( wxCommandEvent& ev );

	DECLARE_EVENT_TABLE();
};

#endif // _VIDEO_FRAME_4987744442145570_