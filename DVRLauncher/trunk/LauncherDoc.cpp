// LauncherDoc.cpp : implementation of the CLauncherDoc class
//

#include "stdafx.h"
#include "CSLauncher.h"

#include <shlwapi.h>
#include <shlobj.h>
#include <winsock2.h>

#include "LauncherDoc.h"
#include "MainFrm.h"
#include <afxpriv.h>

#include "TabViews/LauncherView.h"
#include "TabViews/SettingsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HtmlLog::Severity ToLogSeverity(int nSeverity)
{
	HtmlLog::Severity sev;
	switch(nSeverity)
	{
	case Elvees::IMessage::mt_debug_info :	sev = HtmlLog::sev_debug;	break;
	case Elvees::IMessage::mt_info :		sev = HtmlLog::sev_info;	break;
	case Elvees::IMessage::mt_warning :		sev = HtmlLog::sev_warning; break;
	case Elvees::IMessage::mt_error :		sev = HtmlLog::sev_error;	break;
	default :							sev = HtmlLog::sev_unknown;	break;
	}
	return sev;
}

class FileLogCallback: public SimpleLogCB
{
	CyclicLog	m_log;
	static const size_t LogFileNumberInDirectory = 50;
	static const size_t MaxLogFileSizeInBytes = 256 * 1024;
public:
	FileLogCallback( CString sCommonDirectory, CString sSystemName ):
		m_log( 
			sCommonDirectory, sSystemName, 
			LogFileNumberInDirectory, MaxLogFileSizeInBytes )
	{
	}
	
	void OnReceiveMessage( int nSeverity, LPCWSTR szMessage )
	{
		HtmlLog::Severity sev = ToLogSeverity( nSeverity );
		__time64_t timeNow = _time64(0);
		m_log.AddRecord( sev, timeNow, szMessage );
	}
};

// CLauncherDoc

IMPLEMENT_DYNCREATE(CLauncherDoc, CDocument)

BEGIN_MESSAGE_MAP(CLauncherDoc, CDocument)
END_MESSAGE_MAP()

// CLauncherDoc construction/destruction

CLauncherDoc::CLauncherDoc()
{
	// TODO: add one-time construction code here
}
CLauncherDoc::~CLauncherDoc()
{
}
// CLauncherDoc diagnostics

#ifdef _DEBUG
void CLauncherDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLauncherDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


void ReleaseNone( CLauncherView* p )
{
}

boost::shared_ptr<IDebugOutput>	
	CLauncherDoc::CreateDebugOutput( CCreateContext& cc, CString sTabName, bool bActiveTab )
{
	CMainFrame * pFrame = STATIC_DOWNCAST( CMainFrame,cc.m_pCurrentFrame );
	cc.m_pNewViewClass	= RUNTIME_CLASS(CLauncherView);
	CLauncherView* pNewView = (CLauncherView*)pFrame->CreateView( &cc, AFX_IDW_PANE_FIRST + 1 );
	pNewView->SendMessage( WM_INITIALUPDATE );	
	boost::shared_ptr<SelectionTag> VideoTagZ( new SelectionTag(sTabName) );
	pNewView->SetTag( VideoTagZ );
	pFrame->AddTab( VideoTagZ );

	if( bActiveTab )
	{
		pFrame->ChangeView( pNewView );
	}
	CString sCommonDir = GetCommonLogDirectory();
	pNewView->SetCallback( boost::shared_ptr<SimpleLogCB>( new FileLogCallback
						( sCommonDir, sTabName ) ) );

	boost::shared_ptr<IDebugOutput> pDbg( pNewView , &ReleaseNone);
	m_DebugArr.push_back( pDbg );
	// release by document
	return pDbg;
}

void	CLauncherDoc::DeleteMainView()
{
	POSITION pos = GetFirstViewPosition();
	ASSERT(pos);
	CView* pView = GetNextView(pos);

	//RemoveView( pView );
	pView->ShowWindow(SW_HIDE);
}

bool	CLauncherDoc::InitializeData( CCreateContext& cc )
{
	bool res = CreateEnvironment(cc);
	if( !res )
	{
		return false;
	}
#ifdef ENABLE_SETTINGS
	CreateSettingsWindow(cc);
#endif
	DeleteMainView();
	return true;
}

int		CLauncherDoc::GetMaxLineNumber()
{
	CSettingsView* pView = GetView<CSettingsView>();
	return pView ? pView->GetMaxLineNumber() : 100;
}

CString CLauncherDoc::GetCommonLogDirectory() const
{
	TCHAR szBuf[MAX_PATH];
	GetModuleFileName( GetModuleHandle(NULL), szBuf, MAX_PATH );
	PathRemoveFileSpec( szBuf );
	PathAppend( szBuf, L"SystemLogFiles" );
	return CString(szBuf);
}

void CLauncherDoc::OnCloseDocument()
{
	CLauncherView* pView = GetView< CLauncherView >( );
	ASSERT(pView);
//	m_MainLog->AddRecord( HtmlLog::sev_info, _time64(0), L"Application exited normally" );
	DestroyEnvironment();

	CDocument::OnCloseDocument();
}

void CLauncherDoc::CreateSettingsWindow(CCreateContext& cc)
{
	CMainFrame * pFrame = STATIC_DOWNCAST( CMainFrame,cc.m_pCurrentFrame );
	cc.m_pNewViewClass	= RUNTIME_CLASS(CSettingsView);
	CSettingsView* pSetView = (CSettingsView*)pFrame->CreateView( &cc, AFX_IDW_PANE_FIRST + 1 );
	pSetView->SendMessage( WM_INITIALUPDATE );	
	boost::shared_ptr<SelectionTag> SettingTag( new SelectionTag(_T("Settings")) );
	pSetView->SetTag( SettingTag );
	pFrame->AddTab( SettingTag );
}

bool CLauncherDoc::CreateEnvironment( CCreateContext& cc)
{
#if 0
	boost::shared_ptr<IDebugOutput>	pDbg;

	pDbg = CreateDebugOutput(cc, _T("Launcher 1"), true);
	pDbg->Print( IDebugOutput::mt_debug_info, "Dump 000" );

	pDbg = CreateDebugOutput(cc, _T("Launcher 2"));
	pDbg->Print( IDebugOutput::mt_info, "Dump 001" );

	pDbg = CreateDebugOutput(cc, _T("Launcher 3"));
	pDbg->Print( IDebugOutput::mt_warning, "Dump 002" );
	pDbg->Print( IDebugOutput::mt_error, "Dump 002" );
#endif
	return true;
}

void CLauncherDoc::DestroyEnvironment()
{

}