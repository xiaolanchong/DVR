//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
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
	//! ������ ����������� �� ��������
//	std::auto_ptr<wxImageList>			m_pImageList;
	//! ���� ������� ����
	LogFrame*							m_pLogFrame;
	//! ���� ������� ����� ��������� �������
	VideoFrame*							m_pVideoFrame;
	//! ���� ������� ������
	ArchiveSplitter*					m_pArchiveFrame;
	//! ��������� �����
	ServerHandler*						m_pServerHandler;
	//! ���������� ������� �� m_pSharedWindow
	std::auto_ptr<RenderEventHandler>	m_RenderHandler;
	//! ���� �������, ������� ����������� ����� ��������
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

	//! ������������� ����
	//! ���������� ����� ��� �������
	boost::shared_ptr<Elvees::IMessage>		GetServerMessageStream();
	//! ���������� ����� ��� ���������������
	boost::shared_ptr<Elvees::IMessage>		GetVideoMessageStream();
	//! ���������� ����� ��� �������
	boost::shared_ptr<Elvees::IMessage>		GetClientMessageStream();
	//! ���������� ����� ��� ������
	boost::shared_ptr<Elvees::IMessage>		GetBackupMessageStream();


	//! ��������� ��������� �����
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
	//! �������� �� �������� ��� �������� ����
	void Init(wxWindow* pParentWnd,
		DVREngine::IEngineSettingsReader* pParamReader,
		DVREngine::IEngineSettingsWriter* pParamWriter 
		);

	//! ���������� ����� �������� �������
	void	OnPageChanged(Mode ev);

//	DECLARE_EVENT_TABLE();



	void	OnTimer( wxTimerEvent& ev );
};

#endif // _MAIN_NOTEBOOK_3705999807869446_