//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ����������� - �������� ���� ������ ������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   07.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_SPLITTER_4331408732181676_
#define _ARCHIVE_SPLITTER_4331408732181676_

class ArchiveFrame;
class ArchiveControlPanel;
class RenderEventCallback;
class CameraFrame;
class IMenuHandler;

#include "ArchiveControlPanel.h"

//! \brief ���������� ������� �����������
//! \version 1.0
//! \date 09-08-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class SplitterEventHandler : public  wxEvtHandler 
{
private:

	//! ���������� �������  ����
	void OnDoubleClick( wxSplitterEvent& ev )
	{
		// prohibit the event
		ev.Veto();
	}

	DECLARE_EVENT_TABLE();
};

//======================================================================================//
//                                class ArchiveSplitter                                 //
//======================================================================================//

//! \brief ����������� - �������� ���� ������ ������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   07.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveSplitter : public wxPanel, 
						private ArchiveControlPanelCallback
{
	//! ���� � �����������
	wxSplitterWindow*		m_pSplitterWnd;

	//! ���������� ���� �����
	ArchiveFrame*			m_pArchiveFrame;
	//! ���������� ���� ������
	ArchiveControlPanel*	m_pArchiveControl;

public:
	
	//! �������� �������� ���� �� ��������
	virtual void						Init( const std::vector<int>& AvailableCameras );
	//! �������� ������ GL �������
	virtual RenderEventCallback*		InitRender( IDVRRenderManager* pMainRender );
	//! ��������������� �������
	virtual void						UninitRender();

	//! �������� ���� ����������� ����� � �����
	//! \return ��������� �� ����
	CameraFrame*						GetCameraFrame();

	//! ���������� ���������� ����������������� ����
	//! \param pMenuHandler 
	void	SetMenuHandler( IMenuHandler* pMenuHandler);

	virtual void					OnActivate(bool bActive);

	ArchiveSplitter();
	virtual ~ArchiveSplitter();

private:

	//! ����� ����� �� ������� ������ (�� ������ ����� �����)
	//! \param NewArchiveTime ����� ��� ����������� ������������
	void	OnTimeCallbackFromVideo( time_t NewArchiveTime );

	//! ���������� ������� ������ ������� �� ������������
	//! \param NewArchiveTime ����� �����  ��� �������������
	void	OnArchiveTimeChanged( time_t NewArchiveTime );

	//! ���������� ������ ������ � �������
	//! \param StartTime ��������� ����� ������
	//! \param EndTime �������� �����
	//! \return ������ ��������, ��� ���� ��������
	virtual boost::shared_ptr<Frames_t> 	OnStartArchive( time_t StartTime, time_t EndTime );

	//! ���������� ������������ ������
	virtual void						OnPlayArchive() ;
	//! ���������� ����� ������
	virtual void						OnPauseArchive();
	//! ���������� ��������� ������
	virtual void						OnStopArchive() ;

	//! �������� ������ �� ��������� �� ������
	//! \param StartTime	������ ������� �������
	//! \param EndTime		����� ������� 
	//! \return ������ ��������, ��� ���� ��������
	boost::shared_ptr<Frames_t>	GetAlarmDataForPeriod( time_t StartTime, time_t EndTime ) const;

	//! �������� ������� ��������� ������ ��� ���������
	//! \param CameraIDArr ������ �����-���
	void	GetCurrentSelectedCameras(std::vector<int>& CameraIDArr);
};

#endif // _ARCHIVE_SPLITTER_4331408732181676_