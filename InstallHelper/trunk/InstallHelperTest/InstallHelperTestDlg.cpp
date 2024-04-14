// InstallHelperTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InstallHelperTest.h"
#include "InstallHelperTestDlg.h"

#include "../InstallHelper.h"
#include "../interface/IInstaller.h"

#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void	MoveActiveStringToAnotherList( CListBox& wndSrcList, CListBox& wndDstList )
{
	int nIndex = wndSrcList.GetCurSel();
	if( LB_ERR == nIndex )
	{
		return;
	}
	CString sMovedText;
	wndSrcList.GetText( nIndex, sMovedText);
	wndSrcList.DeleteString( nIndex );
	int NewIndex = wndDstList.AddString( sMovedText );
	wndDstList.SetCurSel( NewIndex );
	wndSrcList.SetCurSel( LB_ERR );
}

CString		GuidToString( const UUID& uid )
{
	WCHAR szStringUID[255];
	HRESULT hr = StringFromGUID2( uid, szStringUID, 255 );
	return hr == S_OK ? CString( szStringUID ) : CString();
}

bool		StringToGuid( CString sTextGuid, UUID& uid )
{
	WCHAR szStringUID[255];
	lstrcpyn( szStringUID, sTextGuid, 255 );
	HRESULT hr = CLSIDFromString( szStringUID, &uid );
	return hr == S_OK;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CInstallHelperTestDlg dialog




CInstallHelperTestDlg::CInstallHelperTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstallHelperTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInstallHelperTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AVAILABLE, m_listAvailable);
	DDX_Control(pDX, IDC_LIST_SYSTEM, m_listSystem);
	DDX_Control(pDX, IDC_COMBO_IPADDRESS, m_wndIPAddress);
}

BEGIN_MESSAGE_MAP(CInstallHelperTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST_AVAILABLE, &CInstallHelperTestDlg::OnLbnSelchangeListAvailable)
	ON_BN_CLICKED(IDC_BUTTON_ADD_TO_SYSTEM, &CInstallHelperTestDlg::OnBnClickedButtonAddToSystem)
	ON_LBN_SELCHANGE(IDC_LIST_SYSTEM, &CInstallHelperTestDlg::OnLbnSelchangeListSystem)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_FROM_SYSTEM, &CInstallHelperTestDlg::OnBnClickedButtonRemoveFromSystem)
	ON_BN_CLICKED(IDC_BUTTON_ARCHIVE_PATH, &CInstallHelperTestDlg::OnBnClickedButtonArchivePath)
	ON_BN_CLICKED(IDC_BUTTON_FREE_MEMORY, &CInstallHelperTestDlg::OnBnClickedButtonFreeMemory)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CInstallHelperTestDlg::OnBnClickedButtonRegister)
	ON_BN_CLICKED(IDC_BUTTON_UNREGISTER, &CInstallHelperTestDlg::OnBnClickedButtonUnregister)
	ON_BN_CLICKED(IDC_BUTTON_SET_ARCHIVE_PATH, &CInstallHelperTestDlg::OnBnClickedButtonSetArchivePath)
	ON_BN_CLICKED(IDC_CHECK_BRIDGE, &CInstallHelperTestDlg::OnBnClickedCheckBridge)
END_MESSAGE_MAP()


// CInstallHelperTestDlg message handlers

BOOL CInstallHelperTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	SetBridgeForConfigure( false );
	CheckDlgButton( IDC_CHECK_BRIDGE, BST_UNCHECKED );

	FillIPAddress();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInstallHelperTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CInstallHelperTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CInstallHelperTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CInstallHelperTestDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	//bool res = m_pInstaller->Unregister();
#ifdef USE_INSTALL_INTERFACE
	m_pInstaller.reset();
#else
	FreeMemory();
#endif
}

void CInstallHelperTestDlg::SelectStream( CListBox& wndWhereSelectedList, CListBox& wndDependentList )
{
	int nCurSel = wndWhereSelectedList.GetCurSel();

	if( nCurSel  != LB_ERR)
	{
		//UuidFromString((UuidString)StringUuid, Uuid) != RPC_S_OK
		TCHAR UUIDStringBuf[255];
		wndWhereSelectedList.GetText( nCurSel, UUIDStringBuf );
#ifdef USE_INSTALL_INTERFACE
		UUID uid;
		HRESULT hr = CLSIDFromString( UUIDStringBuf, &uid );
		m_pInstaller->ShowStream( hr == S_OK ? &uid : 0, GetWindowForVideo() );
#else
		CT2A AnsiUUIDStringBuf(UUIDStringBuf);
		ShowStream(  GetWindowForVideo(), AnsiUUIDStringBuf);
#endif

		wndDependentList.SetCurSel( LB_ERR );
	}
	else
	{
#ifdef USE_INSTALL_INTERFACE
		m_pInstaller->ShowStream( 0, GetWindowForVideo() );
#else
	ShowStream(  GetWindowForVideo(), "" );
#endif
	}
}

void CInstallHelperTestDlg::OnLbnSelchangeListAvailable()
{
	// TODO: Add your control notification handler code here
	SelectStream( m_listAvailable, m_listSystem );
}

void CInstallHelperTestDlg::OnBnClickedButtonAddToSystem()
{
	// TODO: Add your control notification handler code here
	MoveActiveStringToAnotherList( m_listAvailable, m_listSystem );
}

void CInstallHelperTestDlg::OnLbnSelchangeListSystem()
{
	// TODO: Add your control notification handler code here
	SelectStream( m_listSystem, m_listAvailable );
}

void CInstallHelperTestDlg::OnBnClickedButtonRemoveFromSystem()
{
	// TODO: Add your control notification handler code here
	MoveActiveStringToAnotherList( m_listSystem, m_listAvailable );
}

void CInstallHelperTestDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	for ( int i = 0; i < m_listSystem.GetCount(); ++i )
	{
		CString sTextUid;
		m_listSystem.GetText( i, sTextUid );
#ifdef USE_INSTALL_INTERFACE
		UUID uid;
		bool res = StringToGuid( sTextUid, uid );
		if( res )
		{
			m_pInstaller->AddStream( &uid );
		}
#else
	CT2A AnsiUUIDStringBuf(sTextUid);
	AddStream( AnsiUUIDStringBuf );
#endif
	}	

	CDialog::OnOK();
}

void CInstallHelperTestDlg::OnBnClickedButtonArchivePath()
{
	// TODO: Add your control notification handler code here
	const char* szArchivePath = GetArchivePath();
	MessageBoxA( GetSafeHwnd(),  szArchivePath, "", MB_OK);
}

void CInstallHelperTestDlg::OnBnClickedButtonFreeMemory()
{
	// TODO: Add your control notification handler code here
	ShowStream( 0, 0 );
	FreeMemory();
}

void CInstallHelperTestDlg::OnBnClickedButtonRegister()
{
	// TODO: Add your control notification handler code here

	GetDlgItem( IDC_CHECK_BRIDGE )->EnableWindow(FALSE);

	DWORD res;
#if 1
	res = Unregister();
	ASSERT(res == Res_Success );
	if( Res_ServerLaunched == res )
	{
		AfxMessageBox(_T("Server application has been launched. Close it and repeat"), MB_OK|MB_ICONERROR);
		return;
	}
	std::string IPAddress = GetCurrentIPAddress();
	res = Register( IPAddress.c_str(), NULL ) != 0;
	ASSERT(res == Res_Success);
#endif
#if 1
	CAUUID AllStreams;
	res = GetAvailableStreams( &AllStreams );
	ASSERT(res == Res_Success);
	for ( ULONG i = 0; i < AllStreams.cElems; ++i )
	{
		WCHAR* szStringUID;
		UuidToString( & AllStreams.pElems[i], reinterpret_cast<unsigned short**>(&szStringUID) );
		m_listAvailable.AddString(szStringUID);
		RpcStringFree(reinterpret_cast<unsigned short**>(&szStringUID));
	}
	CoTaskMemFree(AllStreams.pElems);
#endif
}

void CInstallHelperTestDlg::OnBnClickedButtonUnregister()
{
	// TODO: Add your control notification handler code here
	GetDlgItem( IDC_CHECK_BRIDGE )->EnableWindow(TRUE);
	BOOL res = Unregister() != 0;
	ASSERT(res);
}

void CInstallHelperTestDlg::OnBnClickedButtonSetArchivePath()
{
	// TODO: Add your control notification handler code here
	CDirDialog dlg;
	CString sArchiveDirectory;
#if 1
	if( dlg.DoBrowse( this ) )
	{
		sArchiveDirectory = dlg.m_strPath;
	}
#else
	sArchiveDirectory = _T("d:\\database");
#endif
	CT2A cvt(sArchiveDirectory);

	BOOL res = SetArchivePath( cvt ) != 0;
	ASSERT(res);
}

void CInstallHelperTestDlg::OnBnClickedCheckBridge()
{
	// TODO: Add your control notification handler code here
	const bool bUseBridge = 0 != IsDlgButtonChecked( IDC_CHECK_BRIDGE );
	SetBridgeForConfigure( bUseBridge );
}

void CInstallHelperTestDlg::FillIPAddress()
{
	std::vector<std::tstring> OwnIPAddresses = GetOwnIPAddress();
	for ( size_t i =0; i < OwnIPAddresses.size(); ++i )
	{
		m_wndIPAddress.AddString( OwnIPAddresses[i].c_str() );
	}
	//m_wndIPAddress.AddString( _T("216.126.0.7") );
	m_wndIPAddress.AddString( _T("127.0.0.1") );
	m_wndIPAddress.SetCurSel(0);
}

std::string CInstallHelperTestDlg::GetCurrentIPAddress()
{
	USES_CONVERSION;
	CString IPAddress;
	m_wndIPAddress.GetWindowText(IPAddress);
	return T2CA( static_cast<LPCTSTR>(IPAddress) );
}