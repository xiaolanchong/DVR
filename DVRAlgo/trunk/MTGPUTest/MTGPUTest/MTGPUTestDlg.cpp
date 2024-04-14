z// MTGPUTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MTGPUTest.h"
#include "MTGPUTestDlg.h"

#include "CgTestWindow.h"

#include "../../Grabber/Grabber.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMTGPUTestDlg dialog

CMTGPUTestDlg::CMTGPUTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMTGPUTestDlg::IDD, pParent)
	, m_runShaders(FALSE)
	, m_readBack(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMTGPUTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_RUN_SHADERS, m_runShaders);
	DDX_Check(pDX, IDC_CHECK_READ_BACK, m_readBack);
}

BEGIN_MESSAGE_MAP(CMTGPUTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_RUN, &CMTGPUTestDlg::OnBnClickedBtnRun)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_RUN_SHADERS, &CMTGPUTestDlg::OnBnClickedCheckRunShaders)
	ON_BN_CLICKED(IDC_CHECK_READ_BACK, &CMTGPUTestDlg::OnBnClickedCheckReadBack)
END_MESSAGE_MAP()


// CMTGPUTestDlg message handlers

BOOL CMTGPUTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CheckDlgButton( IDC_CHECK_RUN_SHADERS, BST_CHECKED );
	CheckDlgButton( IDC_CHECK_READ_BACK, BST_CHECKED );

	m_GPUTestParams = boost::shared_ptr< GPUTestParameters >( new GPUTestParameters );
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMTGPUTestDlg::OnPaint()
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
HCURSOR CMTGPUTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMTGPUTestDlg::OnBnClickedBtnRun()
{

	// Create and launch camera threads.

	//InitCameraGrabber();
	
	int cameraNum = 8;
	for (int i = 0; i < cameraNum; i++)
	{
		boost::shared_ptr<Elvees::ICameraGrabber> grabber = 
			boost::shared_ptr<Elvees::ICameraGrabber>( Elvees::CreateChcsvaCameraGrabber( i, 0 ) );
		
		if ( grabber.get() == 0)
			break;

		cameraNum = Elvees::GetNumberOfAvailablesCameras();

		CString wndClassName = AfxRegisterWndClass( 0 );
		boost::shared_ptr<CWnd> renderWindow = boost::shared_ptr<CWnd>( new CWnd );
		BOOL res = renderWindow->CreateEx( 
			0, wndClassName, TEXT("Thread Window"), 
			WS_POPUP|WS_VISIBLE| WS_CAPTION, 
			CRect( 0, 0, 400, 400 ), this, 0  );
		res;
		cameras.push_back( boost::shared_ptr<OneThreadWindow> ( new CgTestWindow( renderWindow.get(), grabber, m_GPUTestParams ) ) );
		renderWindows.push_back( renderWindow );
	}
	
	if( !cameras.empty() )
		cameras[0]->BeginStarting();

	for ( size_t i = 0; i < cameras.size(); i++ )
	{
		cameras[i]->Start();
	}

}

void CMTGPUTestDlg::OnDestroy()
{
	CDialog::OnDestroy();

	for( size_t i = 0; i < cameras.size(); ++i )
	{
		cameras[i]->Stop();
		cameras[i].reset();
	}

	Elvees::DeleteChcsvaCameraGrabber();
}




void CMTGPUTestDlg::OnBnClickedCheck1()
{

}

void CMTGPUTestDlg::OnBnClickedCheckRunShaders()
{
	UpdateData();
	m_runShaders ? m_GPUTestParams->runShaders = true : m_GPUTestParams->runShaders = false;	
}

void CMTGPUTestDlg::OnBnClickedCheckReadBack()
{
	UpdateData();
	m_readBack ? m_GPUTestParams->readBack = true : m_GPUTestParams->readBack = false;
}
