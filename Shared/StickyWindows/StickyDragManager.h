/*! \file StickyDragManager.h
\brief CStickyDragManager header file

Based on sample Creating Sticky Windows by Jim Strong http://www.codeguru.com/Cpp/W-D/docking/article.php/c4589/ \n
Created: 2005-08-09 by Kirill V. Lyadvinsky \n
Modified: \n
*/

#ifndef _STICKYDRAGMANAGER_H
#define _STICKYDRAGMANAGER_H

//! Structure of registered windows
struct regedWindows
{
  HWND        hWnd;
	POINTS      offset;
  WNDPROC     wndproc;        //!< Remember here old WndProc
};

//! \brief Manage sticked windows
//! Usage: Create CStickyDragManager instance. Select main docker window using SetDocker.
//! Then add any windows you want using AddWindow method.
class CStickyDragManager
{
public:
	CStickyDragManager();
	virtual ~CStickyDragManager();

  //! Adds new window to manager (to m_managedWindows actually)
  void AddWindow(HWND hWnd);
  //! Sets main docker window. User can dock other windows only to that window.
	inline void SetDocker(HWND hDockerWnd) {m_windowDocker = hDockerWnd; }
  //! Throws in the desktop window for good measure.
	inline void AddDesktopWindow() { HWND hWnd = ::GetDesktopWindow(); AddWindow(hWnd); m_exclusionWindow = hWnd; }

protected:
  void Init(HWND hWnd = 0);
  vector<HWND> *SnapMove(LPRECT activeRect, vector<RECT>& dockedRects);
  void SnapMoveRect(LPRECT r);
  void SnapSize(LPRECT r);
  void GetDockedWindows(HWND hWnd, unsigned int index, vector<HWND> &windows);
  void StartTrack();

  inline bool IsCloseTo(int ms, int ss) {	return ((ms > (ss - m_slack)) && (ms < (ss + m_slack))) ? true : false;	}
  inline bool IsDocker() { return (m_windowDocker == m_activeWindow); }
  inline vector<HWND>* DockedWindows() {	return &m_dockedWindows; }
  inline vector<HWND>* UnDockedWindows() {	return &m_undockedWindows; }
  inline HWND GetActiveWindow() { return m_activeWindow; }

  static LRESULT CALLBACK StickyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	RECT   m_lastDragRect;
	POINT  m_dragPos;
	int    m_slack;
	bool   m_stickFlags[4];
	int    m_stickPos[4];
	HWND   m_activeWindow;
	HWND   m_exclusionWindow;
	vector<HWND> m_undockedWindows;
	vector<regedWindows> m_registeredWindows;
	vector<HWND> m_dockedWindows;
	vector<bool> m_windowDock;
	vector<bool> m_windowDockProcess;  
  HWND   m_windowDocker;
};

#endif //_STICKYDRAGMANAGER_H
