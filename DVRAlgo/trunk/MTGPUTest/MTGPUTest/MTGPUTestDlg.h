// MTGPUTestDlg.h : header file
//

#pragma once

#include "OneThreadWindow.h"
#include "GPUTestParameters.h"

// CMTGPUTestDlg dialog
class CMTGPUTestDlg : public CDialog
{
 // Construction
	CWnd	m_wndRender;
public:
	CMTGPUTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MTGPUTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnRun();

	std::vector< boost::shared_ptr<OneThreadWindow> > cameras;
	std::vector< boost::shared_ptr<CWnd> > renderWindows;

public:
	afx_msg void OnDestroy();
	boost::shared_ptr<GPUTestParameters> m_GPUTestParams;
public:
	afx_msg void OnBnClickedCheck1();
public:
	afx_msg void OnBnClickedCheckRunShaders();
private:
	BOOL m_runShaders;
	BOOL m_readBack;
	afx_msg void OnBnClickedCheckReadBack();
};
