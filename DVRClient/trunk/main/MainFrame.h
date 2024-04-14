//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Main frame for wxWidget application

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   09.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _MAIN_FRAME_6850624129279629_
#define _MAIN_FRAME_6850624129279629_


//======================================================================================//
//                                   class MainFrame                                    //
//======================================================================================//

//! \brief Main frame for wxWidget application
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.03.2006
//! \version 2.0
//! \bug 
//! \todo 

class MainNotebook;
class ServerHandler;

class MainFrame: public wxFrame
{
	//! содержит все вкладки
	std::auto_ptr<MainNotebook>	m_pBook;

	//! обработчик выбора вкладок в m_pBook, его нельзя поместить в сам m_pBook
	void	OnModeVideo(wxCommandEvent& ev);
	void	OnModeArchive(wxCommandEvent& ev);
	void	OnModeLog(wxCommandEvent& ev);

	void	AddTextToMainToolBar(wxToolBar* pMainToolBar);
public:
	MainFrame(
		DVREngine::IEngineSettingsReader* pParamReader,
		DVREngine::IEngineSettingsWriter* pParamWriter 
		);
	virtual ~MainFrame();

	//! делегирование m_pBook
	//! отладочный поток для сервера
	boost::shared_ptr<Elvees::IMessage>		GetServerMessageStream();
	//! отладочный поток для видеоподсистемы
	boost::shared_ptr<Elvees::IMessage>		GetVideoMessageStream();
	//! отладочный поток для клиента
	boost::shared_ptr<Elvees::IMessage>		GetClientMessageStream();
	//! отладочный поток для бэкапа
	boost::shared_ptr<Elvees::IMessage>		GetBackupMessageStream();

	//! загрузка из ресурсов для дочерних окон
	void Init();
	//! установка серверной части
	void SetServerHandler( ServerHandler* pHandler);

private:
	DECLARE_EVENT_TABLE()

	void OnTimer( wxTimerEvent& ev); 
	//! таймер для чтения тревожных ситуаций из сервера
	wxTimer		m_AlarmTimer;
};

#endif // _MAIN_FRAME_6850624129279629_