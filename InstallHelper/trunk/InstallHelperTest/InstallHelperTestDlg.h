// InstallHelperTestDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

//#define USE_INSTALL_INTERFACE

#ifdef USE_INSTALL_INTERFACE
class IInstaller;
#endif

#define USE_STATIC_FOR_RENDER

// CInstallHelperTestDlg dialog
class CInstallHelperTestDlg : public CDialog
{
	void FillIPAddress();

	std::string GetCurrentIPAddress();

	HWND GetWindowForVideo()
	{
#ifdef USE_STATIC_FOR_RENDER
		return ::GetDlgItem( GetSafeHwnd(), IDC_STATIC_VIDEO );
#else
		return m_wndFrameRender.GetSafeHwnd();
#endif
	}

	void SelectStream( CListBox& wndWhereSelectedList, CListBox& wndDependentList );

	CWnd	m_wndFrameRender;
// Construction
public:
	CInstallHelperTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_INSTALLHELPERTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

#ifdef USE_INSTALL_INTERFACE
	std::auto_ptr<IInstaller> m_pInstaller;
#endif
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

	CListBox m_listAvailable;
	CListBox m_listSystem;
	
	afx_msg void OnLbnSelchangeListAvailable();
	afx_msg void OnBnClickedButtonAddToSystem();
	afx_msg void OnLbnSelchangeListSystem();
	afx_msg void OnBnClickedButtonRemoveFromSystem();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedButtonArchivePath();
	afx_msg void OnBnClickedButtonFreeMemory();
public:
	afx_msg void OnBnClickedButtonRegister();
public:
	afx_msg void OnBnClickedButtonUnregister();
public:
	afx_msg void OnBnClickedButtonSetArchivePath();
public:
	afx_msg void OnBnClickedCheckBridge();
public:
	CComboBox m_wndIPAddress;
};
