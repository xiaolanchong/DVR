//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс документа для запуска DVREngine

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   06.07.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "DVRLauncherDoc.h"
#include "../res/resource.h"

//======================================================================================//
//                                class CDVRLauncherDoc                                 //
//======================================================================================//

CDVRLauncherDoc::CDVRLauncherDoc()
{

}

CDVRLauncherDoc::~CDVRLauncherDoc()
{
}

IMPLEMENT_DYNCREATE(CDVRLauncherDoc, CLauncherDoc)

void CDVRLauncherDoc::InitCommandOption()
try
{
	LPWSTR *szArglist;
	int nArgs;

	std::vector<std::string> Params;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	m_Option.reset( new CommandOption( szArglist, nArgs ) );	
}
catch(std::exception&)
{

};

bool CDVRLauncherDoc::IsCommandParameterExists( const char* Name) const
{
	if( m_Option.get() )
	{
		return m_Option->OptionExists( Name );
	}
	else
	{
		return false;
	}
}

bool CDVRLauncherDoc::CreateEnvironment(CCreateContext& cc)
{
	InitCommandOption();

	if( m_Option.get() && m_Option->ShowHelpMessage() )
	{
		m_Option->OutputDescription();
		return false;
	}

	bool bNoAlgo		= IsCommandParameterExists( "noalgo" );
	bool bNoBackup		= IsCommandParameterExists( "nobackup" );
	bool bNoArchive		= IsCommandParameterExists( "noarchive" );
	bool bNoNetArchive	= IsCommandParameterExists( "nonetarchive" );
	bool bNoNetServer	= IsCommandParameterExists( "nonetserver" );
	bool bUseGUI		= IsCommandParameterExists( "gui" );
	bool bNoDebug		= IsCommandParameterExists( "nodebug" );

	boost::shared_ptr<IDebugOutput> pVideoDbg	= CreateDebugOutput( cc, _T("Video"), true );
	boost::shared_ptr<IDebugOutput> pServerDbg	= CreateDebugOutput( cc, _T("Server"), false );
	boost::shared_ptr<IDebugOutput> pBackupDbg	= CreateDebugOutput( cc, _T("Backup"), false );

	pServerDbg->Print( IDebugOutput::mt_info, !bNoAlgo? "Launch algorithms" : "No algorithm"  );

	DWORD dwVideoMode = CHCS_MODE_SERVER;

	if( bNoArchive )
	{
		pServerDbg->Print( IDebugOutput::mt_info, "CHCS_MODE_STORE disabled" );
		dwVideoMode &= ~CHCS_MODE_STORE;
	}
	if( !bUseGUI )
	{
		pServerDbg->Print( IDebugOutput::mt_info, "CHCS_MODE_GUI disabled" );
		dwVideoMode &= ~CHCS_MODE_GUI;
	}
	if( !bNoDebug )
	{
		pServerDbg->Print( IDebugOutput::mt_info, "CHCS_MODE_DEBUG enabled" );
		dwVideoMode |= CHCS_MODE_DEBUG;
	}
	if( bNoNetArchive )
	{
		pServerDbg->Print( IDebugOutput::mt_info, "CHCS_MODE_NETARCHIVE disabled" );
		dwVideoMode &= ~CHCS_MODE_NETARCHIVE;
	}
	if( bNoNetServer )
	{
		pServerDbg->Print( IDebugOutput::mt_info, "CHCS_MODE_NETSERVER disabled" );
		dwVideoMode &= ~CHCS_MODE_NETSERVER;
	}

	DVREngine::ServerStartSettings ss;

	ss.m_bLaunchAlgo	= !bNoAlgo;
	ss.m_bLaunchBackup	= !bNoBackup;
	ss.m_pBackupMsg		= pBackupDbg;
	ss.m_pServerMsg		= pServerDbg;
	ss.m_pVideoMsg		= pVideoDbg;
	ss.m_VideoSystemMode = dwVideoMode  ;
	std::string s;
	if( m_Option.get() )
	{
		s = m_Option->OptionValue( "keyname" );
		std::string ss( "User defined key name=" );
		ss += s;
		pServerDbg->Print( IDebugOutput::mt_info, ss.c_str() );
	}
	ss.m_ProtectionKeyName = s;
#if 1
	try
	{
		m_pServer = boost::shared_ptr<DVREngine::IServer>( DVREngine::CreateServerInterface(ss) );
		return true;
	}
	catch( DVREngine::IServer::ServerAlreadyLaunchedException&  )
	{
		AfxMessageBox( IDS_SERVER_ALREADY_LAUNCHED , MB_OK|MB_ICONERROR );
		return false;
	}
	catch( DVREngine::IServer::ServerException& ex )
	{
		MessageBoxA(AfxGetMainWnd()->GetSafeHwnd(), ex.what(), "Fatal error", MB_OK|MB_ICONERROR );
		// allow to the user to read error messages
		return true;
	}
#endif
}

void CDVRLauncherDoc::DestroyEnvironment()
{
	m_pServer.reset();
}