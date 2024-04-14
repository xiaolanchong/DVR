//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Main archive tab window

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   14.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_FRAME_7709752320116841_
#define _ARCHIVE_FRAME_7709752320116841_

#include "../common/CameraFrame.h"
#include "ArchiveCanvas.h"


class VideoSeeker;

//======================================================================================//
//                                  class ArchiveFrame                                  //
//======================================================================================//

//! \brief Main archive tab window
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.03.2006
//! \version 2.0
//! \bug 
//! \todo 

class ArchiveFrame : public CameraFrame
{

	//! ������� ��� ������� �������
	std::auto_ptr<ArchiveCanvas>		m_pCanvas;

	//! �-� ��������� ������ ��� ��������� ������� ������ (�� ������� ���������������)
	boost::function1< void, time_t>		m_fnOnTimeChanged;
public:
	ArchiveFrame(const std::vector<int>& AvailableCameras, boost::function1< void, time_t>);
	virtual ~ArchiveFrame();

	//! �������� �������� ���� �� ��������
	virtual void						Init(  );
	//! �������� ������ GL �������
	virtual RenderEventCallback*		InitRender( IDVRRenderManager* pMainRender );
	//! ��������������� �������
	virtual void						UninitRender();
	//! ������� ���� �������/���������
	//! \param bActive 
	virtual void					OnActivate(bool bActive);

	void	SetMenuHandler( IMenuHandler* pMenuHandler)
	{
		m_pCanvas->SetMenuHandler( pMenuHandler );
	}

	void	GetCurrentSelectedCameras(std::vector<int>& CameraIDArr)
	{
		m_LayoutMgr.GetSelectedCameras( CameraIDArr );
	}

	void	AcquireArchive( time_t StartTime, time_t EndTime);
	void	ReleaseArchive();
	void	PlayArchive();
	void	PauseArchive();
	void	StopArchive();
	void	SeekArchive(time_t NewArchiveTime);
private:
	//! �������� ������ ���� ������
	//! \return ��������� �� ����
	virtual wxSizer*	GetControlPanelSizer();

	//! �-� ��������� ������ ��� ��������� ������ �������
	//! \param NewTime ����� ����� ������
	void				OnTimeCallback( time_t NewTime ); 
#ifdef OLD
	//! ���������� seek bar, ����� ������������ ������������ �����
	void				OnSeekTimeChange();
	//! ���������� ������� ����� �����������
	//! \param CurrentTime	������� �����
	void				SetCurrentTimeText( time_t CurrentTime );

	//! �������� ����� �����������
	void				InvalidateCurrentTime();


#endif
private:
//	void ShowPanel( bool bShow );

	virtual void PreFullScreen(bool bShow) ;

	DECLARE_EVENT_TABLE();
};

#endif // _ARCHIVE_FRAME_7709752320116841_