//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: –азделитель - основное окно режима архива

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

//! \brief обработчик событи€ разделител€
//! \version 1.0
//! \date 09-08-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class SplitterEventHandler : public  wxEvtHandler 
{
private:

	//! обработчик нажати€  мыши
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

//! \brief –азделитель - основное окно режима архива
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   07.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveSplitter : public wxPanel, 
						private ArchiveControlPanelCallback
{
	//! окно с разделением
	wxSplitterWindow*		m_pSplitterWnd;

	//! дочерненее окно слева
	ArchiveFrame*			m_pArchiveFrame;
	//! дочерненее окно справа
	ArchiveControlPanel*	m_pArchiveControl;

public:
	
	//! загрузка дочерних окон из ресурсов
	virtual void						Init( const std::vector<int>& AvailableCameras );
	//! создание режима GL рендера
	virtual RenderEventCallback*		InitRender( IDVRRenderManager* pMainRender );
	//! деинициализаци€ рендера
	virtual void						UninitRender();

	//! получить окно отображени€ видео с камер
	//! \return указатель на окно
	CameraFrame*						GetCameraFrame();

	//! установить обработчик пользовательского меню
	//! \param pMenuHandler 
	void	SetMenuHandler( IMenuHandler* pMenuHandler);

	virtual void					OnActivate(bool bActive);

	ArchiveSplitter();
	virtual ~ArchiveSplitter();

private:

	//! новое врем€ от рендера архива (за данное врем€ кадры)
	//! \param NewArchiveTime врем€ дл€ отображение пользователю
	void	OnTimeCallbackFromVideo( time_t NewArchiveTime );

	//! обработчик запроса нового времени от пользовател€
	//! \param NewArchiveTime новое врем€  дл€ позицировани€
	void	OnArchiveTimeChanged( time_t NewArchiveTime );

	//! обработчик старта работы с архивом
	//! \param StartTime начальное врем€ показа
	//! \param EndTime конечное врем€
	//! \return список периодов, где было движение
	virtual boost::shared_ptr<Frames_t> 	OnStartArchive( time_t StartTime, time_t EndTime );

	//! обработчки проигрывани€ архива
	virtual void						OnPlayArchive() ;
	//! обработчки паузы архива
	virtual void						OnPauseArchive();
	//! обработчки остановки архива
	virtual void						OnStopArchive() ;

	//! получить данные по ситуаци€м за период
	//! \param StartTime	начало периода времени
	//! \param EndTime		конец периода 
	//! \return список периодов, где было движение
	boost::shared_ptr<Frames_t>	GetAlarmDataForPeriod( time_t StartTime, time_t EndTime ) const;

	//! получить текущие выбранные камеры дл€ просмотра
	//! \param CameraIDArr списко идент-ров
	void	GetCurrentSelectedCameras(std::vector<int>& CameraIDArr);
};

#endif // _ARCHIVE_SPLITTER_4331408732181676_