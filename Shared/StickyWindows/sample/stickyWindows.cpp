#include "stdafx.h"
//#include "resource.h"		// main symbols
#include "../StickyDragManager.h"

class CStickyWindowsApp : public CWinApp
{
public:
	CStickyWindowsApp()
	{
	}
	virtual BOOL InitInstance()
	{
		CRect pr(20, 20, 220, 220);
		for (int i = 0; i < 5; i++, pr += CPoint(20, 20))
		{
			CString title;
			if (i == 0)
				title = "Main window";
      else if (i == 1) title = "Master video";
      else if (i == 2) title = "Slave video";
      else if (i == 3) title = "Alarms list";
      else if (i == 4) title = "Target info";

      m_windows[i].CreateEx((i!=0)?WS_EX_TOOLWINDOW:0, AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW, 0, GetSysColorBrush(COLOR_BTNFACE), 0),
        title, WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME, pr, (i==0)?CWnd::GetDesktopWindow():&m_windows[0], 0);
      if (i == 0) smgr.SetDocker(m_windows[i]);
      smgr.AddWindow(m_windows[i]);
		}
    smgr.AddDesktopWindow();
		m_pMainWnd = &m_windows[0];
		return TRUE;
	}
	CWnd m_windows[8];
  CStickyDragManager smgr;
};

CStickyWindowsApp theApp;

