//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Main notebook window (tabs)for wxWidget application

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   09.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _MAIN_NOTEBOOK_3705999807869446_
#define _MAIN_NOTEBOOK_3705999807869446_

//======================================================================================//
//                                  class MainNotebook                                  //
//======================================================================================//

//! \brief Main notebook window (tabs)for wxWidget application
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   09.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class LogFrame;
class VideoFrame;
class ArchiveSplitter;

class ServerHandler;
class RenderEventHandler;
class IDVRRenderManager;

class CameraNaming;

class MainNotebook/* : public wxNotebook*/
{
	//! список изображений на закладки
//	std::auto_ptr<wxImageList>			m_pImageList;
	//! окно вкладки лога
	LogFrame*							m_pLogFrame;
	//! окно вкладки видео реального времени
	VideoFrame*							m_pVideoFrame;
	//! окно вкладки архива
	ArchiveSplitter*					m_pArchiveFrame;
	//! серверная часть
	ServerHandler*						m_pServerHandler;
	//! обработчик событий от m_pSharedWindow
	std::auto_ptr<RenderEventHandler>	m_RenderHandler;
	//! окно рендера, которое разделяется между режимами
	wxWindow*							m_pSharedWindow;

	std::auto_ptr<IDVRRenderManager>	m_pMainRender;

	std::auto_ptr<CameraNaming>			m_pNaming;
public:
	//! ctor
	//! \param pParent	parent window pointer
	//! \param nID		window id
	MainNotebook(
		wxWindow* pParentWnd, 
		DVREngine::IEngineSettingsReader* pParamReader,
		DVREngine::IEngineSettingsWriter* pParamWriter
		);
	virtual ~MainNotebook();

	//! делегирование логу
	//! отладочный поток для сервера
	boost::shared_ptr<Elvees::IMessage>		GetServerMessageStream();
	//! отладочный поток для видеоподсистемы
	boost::shared_ptr<Elvees::IMessage>		GetVideoMessageStream();
	//! отладочный поток для клиента
	boost::shared_ptr<Elvees::IMessage>		GetClientMessageStream();
	//! отладочный поток для бэкапа
	boost::shared_ptr<Elvees::IMessage>		GetBackupMessageStream();


	//! установка серверной части
	void SetServerHandler( ServerHandler* pHandler);

	void	OnTimer();

	//! all window tabs
	enum Mode
	{
		Page_Video		= 0,	//!< video window tab
		Page_Archive	= 1,	//!< archive window tab
		Page_Log		= 2		//!< log window tab
	};

	void	SetActiveWindow(  Mode WindowMode );
private:
	std::vector< std::pair<Mode, wxWindow*> >	m_ChildWindows;
	//! загрузка из ресурсов для дочерних окон
	void Init(wxWindow* pParentWnd,
		DVREngine::IEngineSettingsReader* pParamReader,
		DVREngine::IEngineSettingsWriter* pParamWriter 
		);

	//! обработчик смены активной вкладки
	void	OnPageChanged(Mode ev);

//	DECLARE_EVENT_TABLE();



	void	OnTimer( wxTimerEvent& ev );
};

#endif // _MAIN_NOTEBOOK_3705999807869446_