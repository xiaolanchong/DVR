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
#ifndef _VIDEO_CANVAS_3052168141003239_
#define _VIDEO_CANVAS_3052168141003239_

#include "../common/CameraCanvas.h"


//======================================================================================//
//                                  class VideoCanvas                                   //
//======================================================================================//

//! \brief Взаимодействие с IDVRRender: отрисовка кадров и информации
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   09.03.2006
//! \version 1.0
//! \bug 
//! \todo 

class VideoCanvas : public CameraCanvas
{
	Frames_t					m_Alarms;

	typedef std::map< int, boost::shared_ptr< IDVRStreamRender::ICamera > > CameraMap_t;
	CameraMap_t	m_Cameras;
	bool	DrawCameraFrame(int nCameraID, const IAreaLayout::Rect& rc);
	void	Draw( );
public:
	VideoCanvas(	LayoutCameraManager& LayoutMgr, IDVRRenderManager* pMainRender );
	virtual ~VideoCanvas();

	void	SetFrames( const Frames_t& ca )
	{
		m_Alarms = ca;
		m_pMainRender->GetWindow()->Refresh();
	}
};

#endif // _VIDEO_CANVAS_3052168141003239_