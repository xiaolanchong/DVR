//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������ ����������� � ���������� � ������ ������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   10.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_CANVAS_2097692965820325_
#define _ARCHIVE_CANVAS_2097692965820325_

#include "../Common/CameraCanvas.h"
//#include "AlarmArchiveData.h"

//======================================================================================//
//                                 class ArchiveCanvas                                  //
//======================================================================================//

//! \brief ������ ����������� � ���������� � ������ ������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   10.03.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveCanvas : public CameraCanvas
{
	//! ��������� ������ � ������ ������
	void	Draw( );

	IDVRArchiveRender*	m_pArchiveRender;

	//! ����� ���������������
	bool	m_bArchiveStarted;

	//! ������� ��� ��������� ������� ������, \see BindTimeCallback
	boost::function1<void, time_t> m_fnTimeCallback;
public:
	ArchiveCanvas(
		LayoutCameraManager& LayoutMgr, 
		IDVRRenderManager*	pMainRender, 
		boost::function1<void, time_t> fnTimeCallback 
		);
	virtual ~ArchiveCanvas();

	//! ������ ������ � �������
	//! \param pStartTime	����� ������
	//! \param pEndTime		����� �����
	//! \param AlarmData	���-� � ������
	void	StartArchive( time_t StartTime, time_t EndTime);
	//! ��������� ������ � �������
	void	ReleaseArchive();

	//! ������ ������ ������������
	void	PlayArchive();
	//! ������ �����
	void	PauseArchive();
	//! ���������� �����
	void	StopArchive();

	//! �������� ������� ����� ������
	//! \return ������� ����� ������
	time_t	GetArchiveTime();

	//! ����������� ����� �� �����
	//! \param NewTime ����� ��������� ������
	void	SeekArchive(time_t NewTime);

	//! ���������� �-� ��������� ������ ��� ��������� ������ ������� ������
	//! \param fn �-�
	void	BindTimeCallback( boost::function1<void, time_t> fn);
private:

	//! ��������/������� ������ �� �����
	virtual void OnLayoutChange();
};

#endif // _ARCHIVE_CANVAS_2097692965820325_