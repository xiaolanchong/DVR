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
#include "stdafx.h"
#include "MainFrame.h"
#include "MainNotebook.h"
#include "../res/Resource.h"

const int TIMER_ID = 0x143;

//======================================================================================//
//                                   class MainFrame                                    //
//======================================================================================//

MainFrame::MainFrame(
					 DVREngine::IEngineSettingsReader* pParamReader,
					 DVREngine::IEngineSettingsWriter* pParamWriter 
					 ) :
	wxFrame( NULL, wxID_ANY, _("O2KE-Client"), 
			wxDefaultPosition, 
			wxSize(800, 600), 
			wxDEFAULT_FRAME_STYLE, wxT("frame")),
	m_pBook(0),
	m_AlarmTimer( this, TIMER_ID )
{
	char szBuf[MAX_PATH];
	GetModuleFileNameA(0, szBuf, MAX_PATH);
	std::string s (szBuf);
	s += ";0"; //+ boost::lexical_cast<std::string>(IDI_DVRCLIENT);
	wxIcon ico( s.c_str(), wxBITMAP_TYPE_ICO, 16, 16 );
	SetIcon(ico);

	m_AlarmTimer.Start(100);

	m_pBook.reset(new MainNotebook( this, pParamReader, pParamWriter ) );
	wxStatusBar* pBar = CreateStatusBar();

	wxToolBar* pMainToolBar = wxXmlResource::Get()->LoadToolBar( this, wxT("ToolBar_MainMode") );
	_ASSERTE(pMainToolBar);

	SetToolBar( pMainToolBar );
	AddTextToMainToolBar(pMainToolBar);
	

	pMainToolBar->ToggleTool( XRCID("ToolButton_Mode_Video"), true );
}

MainFrame::~MainFrame()
{
	
}

void	MainFrame::AddTextToMainToolBar(wxToolBar* pMainToolBar)
{
	// styles for text 
	pMainToolBar->SetWindowStyle( wxTB_FLAT|wxTB_HORZ_TEXT|wxTB_TEXT  ); 


	/*
	wxDesigner не добавляет текста для кнопок панелей инструментов
	если в главной панели добавятся режимы, то их описание вводиться ниже
	*/
	struct 
	{
		int			Id;
		const char* Name;
		bool		bEnable;
	} Tools[] = 
	{
		{ XRCID("ToolButton_Mode_Video"),	_("Video")	, true	},
		{ XRCID("ToolButton_Mode_Archive"), _("Archive"), true	},
		{ XRCID("ToolButton_Mode_Log"),		_("Log")	, false/*true*/	}
	};

	for ( size_t i = 0; i < sizeof(Tools)/sizeof(Tools[0]); ++i )
	{
		wxToolBarToolBase* pTool = pMainToolBar->RemoveTool(  Tools[i].Id );
		if( Tools[i].bEnable )
		{
	
			wxToolBarToolBase*  z	 = pMainToolBar->AddTool( 
											pTool->GetId(), 
											Tools[i].Name, 
											pTool->GetNormalBitmap() ,
											pTool->GetDisabledBitmap(),
											pTool->GetKind(),
											pTool->GetShortHelp()
											);
			_ASSERTE(z);
		}
		delete pTool;
	}
	pMainToolBar->Realize();
}

boost::shared_ptr<Elvees::IMessage>		MainFrame::GetServerMessageStream()
{
	return m_pBook->GetServerMessageStream();
}
boost::shared_ptr<Elvees::IMessage>		MainFrame::GetVideoMessageStream()
{
	return m_pBook->GetVideoMessageStream();
}
boost::shared_ptr<Elvees::IMessage>		MainFrame::GetClientMessageStream()
{
	return m_pBook->GetClientMessageStream();
}
boost::shared_ptr<Elvees::IMessage>		MainFrame::GetBackupMessageStream()
{
	return m_pBook->GetBackupMessageStream();
}

void MainFrame::Init()
{
	//m_pBook->Init();
}

void MainFrame::SetServerHandler( ServerHandler* pHandler)
{
	m_pBook->SetServerHandler( pHandler );
}

// ----------------------------------------------------------------------------
// wxWidgets macros
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_TIMER(TIMER_ID,				 MainFrame::OnTimer)

	EVT_TOOL( XRCID("ToolButton_Mode_Video"),	MainFrame::OnModeVideo )
	EVT_TOOL( XRCID("ToolButton_Mode_Archive"), MainFrame::OnModeArchive )
	EVT_TOOL( XRCID("ToolButton_Mode_Log"),		MainFrame::OnModeLog )
END_EVENT_TABLE()

// Arrgh... some copy-paste stuff

void	MainFrame::OnModeVideo(wxCommandEvent& ev)
{
	m_pBook->SetActiveWindow( MainNotebook::Page_Video );
	GetToolBar()->ToggleTool( XRCID("ToolButton_Mode_Video"), true );
	GetToolBar()->ToggleTool( XRCID("ToolButton_Mode_Archive"), false );
	GetToolBar()->ToggleTool( XRCID("ToolButton_Mode_Log"), false );
}

void	MainFrame::OnModeArchive(wxCommandEvent& ev)
{
	m_pBook->SetActiveWindow( MainNotebook::Page_Archive );
	GetToolBar()->ToggleTool( XRCID("ToolButton_Mode_Video"),	false );
	GetToolBar()->ToggleTool( XRCID("ToolButton_Mode_Archive"), true );
	GetToolBar()->ToggleTool( XRCID("ToolButton_Mode_Log"),		false );
}

void	MainFrame::OnModeLog(wxCommandEvent& ev)
{
	m_pBook->SetActiveWindow( MainNotebook::Page_Log );
	GetToolBar()->ToggleTool( XRCID("ToolButton_Mode_Video"),	false );
	GetToolBar()->ToggleTool( XRCID("ToolButton_Mode_Archive"), false );
	GetToolBar()->ToggleTool( XRCID("ToolButton_Mode_Log"),		true );
}

//////////////////////////////////////////////////////////////////////////

void	MainFrame::OnTimer( wxTimerEvent& ev )
{  
	m_pBook->OnTimer(  );
}