//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Main application class for wxWidget application

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   09.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "MainFrame.h"
//#include "..\common\wxCustomToolBarXmlHandler.h"
//#include "..\common\wxCustomBitmapButtonXmlHandler.h"
#include "../server/ServerHandler.h"
#include "../common/MessagePluggable.h"
#include "../../../DVREngine/trunk/server/MessageStdOut.h"
#include "CommandParam.h"

const bool c_bLaunchAlgo = 
#ifdef LAUNCH_ALGO
true
#else
false
#endif
;

bool IsFileExists(LPCTSTR sFileName)
{
	// NOTE : can be replaced by CRT 'stat'
	FILE* ProbedHandle = 0;
	/*errno_t res = */fopen_s( &ProbedHandle, sFileName, "rb" );
	if( ProbedHandle )
	{
		fclose( ProbedHandle );
	}
	return /*res &&*/ (ProbedHandle != NULL);
}

int	GetLanguageID()
{
	struct 
	{
		LANGID	WinLanguageID;
		int		wxLanguageID;
	}
	KnownLCIDs[2] = 
	{
		{MAKELANGID( LANG_RUSSIAN, SUBLANG_DEFAULT ), wxLANGUAGE_RUSSIAN },
		{MAKELANGID( LANG_ENGLISH, SUBLANG_DEFAULT ), wxLANGUAGE_ENGLISH }
	};

	CRegKey keyDVR;
	LONG res;
	res = keyDVR.Open( HKEY_LOCAL_MACHINE, _T("Software\\ElVEES\\DVR") );
	if( ERROR_SUCCESS != res )
	{
		return wxLANGUAGE_DEFAULT;
	}

	DWORD LangID = 0;
	res = keyDVR.QueryDWORDValue(_T("LangLCID"), LangID);
	if( ERROR_SUCCESS == res)
	{
		for ( size_t i = 0; i < sizeof(KnownLCIDs)/sizeof(KnownLCIDs[0]); ++i )
		{
			if( KnownLCIDs[i].WinLanguageID == LangID )
			{
				return KnownLCIDs[i].wxLanguageID;
			}
		}
	}
	return wxLANGUAGE_DEFAULT;
}

//======================================================================================//
//                                  class MainApp	                                   //
//======================================================================================//

// Define a new application type, each program should derive a class from wxApp
class MainApp : public wxApp
{
	std::auto_ptr<ServerHandler>	m_pServer;
	wxLocale						m_Locale;
//	CommandParam					m_CmdParam;
public:
	virtual bool OnInit();

private:
	void		AddCustomBitmapHandler();

	MainFrame*	LaunchServer(bool bLaunchAlgorithm);
};

IMPLEMENT_APP_NO_MAIN(MainApp);

void ErrorBox( const char* szDesc )
{
	wxMessageDialog dlg( NULL, szDesc, "Error", wxOK|wxICON_ERROR);
	dlg.ShowModal();
}

bool MainApp::OnInit()
{
#ifndef UNIX_RELEASE
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	//FIXME : relative path
	char buf[4096];
	_getcwd(buf, 4096);
	std::string sResPath(buf);
	sResPath += 
#ifndef UNIX_RELEASE
		//"\\res\\DVRClient";
		"\\res";
#else
		"/res/DVRClient";
#endif

	bool bRes = false;
	bRes = m_Locale.Init( /*wxLANGUAGE_DEFAULT*/GetLanguageID() );
	if(!bRes)
	{
//		ErrorBox( "NLS initialization failed");
//		return false;
	}
	wxLocale::AddCatalogLookupPathPrefix(sResPath.c_str());

	// Initialize the catalogs we'll be using
	m_Locale.AddCatalog(wxT("DVRClient"));

	wxInitAllImageHandlers();
	AddCustomBitmapHandler();
	//NOTE : we use 24b images for toolbars, so switch off remap
	// see wxSystemOptions::msw.remap & wxToolBar95
	wxSystemOptions::SetOption(wxT("msw.remap"), 0);
	wxFileSystem::AddHandler(new wxZipFSHandler);
	wxXmlResource::Get()->InitAllHandlers();

	std::string sXmlResFileName( sResPath );
	sXmlResFileName += 
#ifndef UNIX_RELEASE
		"\\";
#else
		"/";
#endif
	;
	sXmlResFileName += ("DVRClient.xrs"); 
	bRes = wxXmlResource::Get()->Load( sXmlResFileName.c_str() ); 
	//NOTE: wxXmlResource::Load returns true even the resource file does not exists!
	if( !IsFileExists(sXmlResFileName.c_str()) || !bRes)
	{
		// no resources, no localization
		ErrorBox("Application's resources not found");
		return false;
	}
	//_ASSERTE(res);
	try
	{
//#define SERVER_STDOUT
#ifdef SERVER_STDOUT
		const char * szStdOut =
#if 1
			"conout$"
#else
			"1.txt"
#endif
			;
		//////////////////////////////////////////////////////////////////////////
		::AllocConsole();
		::freopen(szStdOut, "w", stdout);
		::freopen(szStdOut, "w", stderr);
		::fprintf(stdout, "stdout: ok\n");
		::fprintf(stderr, "stderr: ok\n");
		boost::shared_ptr<MessageStdOut> MsgServerStd( new MessageStdOut );
#endif //SERVER_STDOUT
		
		CommandParam CmdParam;
		if(!CmdParam.ProcessCommandLine( argv, argc ) )
		{
			return false;
		}

		MainFrame* frame = LaunchServer( CmdParam.LaunchAlgorithm() );

		frame->SetServerHandler( m_pServer.get() );
		frame->Show();
	}
	catch ( std::exception& ex ) 
	{
		ErrorBox(ex.what());
		return false;
	}
	return true;
}

void MainApp::AddCustomBitmapHandler()
{
//	wxXmlResource::Get()->AddHandler( new wxCustomToolBarXmlHandler );
//	wxXmlResource::Get()->AddHandler( new wxCustomBitmapButtonXmlHandler );
}

MainFrame* MainApp::LaunchServer( bool bLaunchAlgorithm)
{
	
	//! launch the server before the client
	boost::shared_ptr<MessagePluggable> MsgVideo( new MessagePluggable );
	boost::shared_ptr<MessagePluggable> MsgServer( new MessagePluggable );
	boost::shared_ptr<MessagePluggable> MsgClient( new MessagePluggable );
	boost::shared_ptr<MessagePluggable> MsgBackup( new MessagePluggable );

	DVREngine::ServerStartSettings ss;

	ss.m_bLaunchAlgo		= bLaunchAlgorithm;
	ss.m_bLaunchBackup		= false;
	ss.m_pBackupMsg			= MsgBackup;
	ss.m_pServerMsg			= MsgServer;
	ss.m_pVideoMsg			= MsgVideo;
	ss.m_VideoSystemMode	= CHCS_MODE_SERVER;

	m_pServer = std::auto_ptr<ServerHandler>( new ServerHandler( ss, MsgClient, false ) );

	MainFrame *frame = new MainFrame( 
		m_pServer->GetParamReader(), 
		m_pServer->GetParamWriter() 
		);

	boost::shared_ptr<Elvees::IMessage>	pVideoMsg  = frame->GetVideoMessageStream();
	boost::shared_ptr<Elvees::IMessage>	pServerMsg = frame->GetServerMessageStream();
	boost::shared_ptr<Elvees::IMessage>	pClientMsg = frame->GetClientMessageStream();
	boost::shared_ptr<Elvees::IMessage>	pBackupMsg = frame->GetBackupMessageStream();

	MsgVideo->Set( pVideoMsg );
	MsgServer->Set(
#ifdef SERVER_STDOUT
		MsgServerStd
#else
		pServerMsg 
#endif
		);
	MsgClient->Set( pClientMsg );
	MsgBackup->Set( pBackupMsg );

	return frame;
}

static int RecordExceptionInfo(...)
{
	return EXCEPTION_EXECUTE_HANDLER;
}

int __stdcall WinMain(  
			HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR lpCmdLine,
			int nCmdShow
			)
{
	__try
	{
		::wxEntry( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
	}
	__except( RecordExceptionInfo(GetExceptionInformation(), "main thread") )
	{

	}
}