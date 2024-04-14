//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
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
	//! �������� ��� �������
	std::auto_ptr<MainNotebook>	m_pBook;

	//! ���������� ������ ������� � m_pBook, ��� ������ ��������� � ��� m_pBook
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

	//! ������������� m_pBook
	//! ���������� ����� ��� �������
	boost::shared_ptr<Elvees::IMessage>		GetServerMessageStream();
	//! ���������� ����� ��� ���������������
	boost::shared_ptr<Elvees::IMessage>		GetVideoMessageStream();
	//! ���������� ����� ��� �������
	boost::shared_ptr<Elvees::IMessage>		GetClientMessageStream();
	//! ���������� ����� ��� ������
	boost::shared_ptr<Elvees::IMessage>		GetBackupMessageStream();

	//! �������� �� �������� ��� �������� ����
	void Init();
	//! ��������� ��������� �����
	void SetServerHandler( ServerHandler* pHandler);

private:
	DECLARE_EVENT_TABLE()

	void OnTimer( wxTimerEvent& ev); 
	//! ������ ��� ������ ��������� �������� �� �������
	wxTimer		m_AlarmTimer;
};

#endif // _MAIN_FRAME_6850624129279629_