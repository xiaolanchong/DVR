// ==========================================================================
// 					Class Specification : COXCalendarPopup
// ==========================================================================
// OXCalendarPopup.h: header file

// Copyright � Dundas Software Ltd. 1997 - 1998, All Rights Reserved

// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CWnd

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// Descriptions: Internal class for use with COXCalendar class
// Implement popup calendar window

/////////////////////////////////////////////////////////////////////////////
#ifndef __DATEPICKER_H
#define __DATEPICKER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include <afxdisp.h>


class COXCalendar;


class OX_CLASS_DECL COXCalendarPopup : public CWnd
{
// Construction
friend class COXCalendar;
// Data members -------------------------------------------------------------
protected:
	void InvalidateSelChange(COleDateTime& SelOld,COleDateTime& SelNew);
	void GetDayRect(int offset,CRect& rect);
	enum {dragNothing=0,dragTodayButton=1,dragNoneButton=2,
		dragGrid=3};
	int m_nDragMode;
	int m_nFirstDayOfWeek;
	CButton m_ButtonMonth;
	CString m_sNone;
	CButton m_ButtonNone;
	CRect m_rectGrid;
	CString m_sToday;
	CButton m_ButtonToday;
	CSize m_SizeWnd;
	CSize m_szCell;
	CFont m_font;
	CRect m_rectRightNavBar;
	CRect m_rectLeftNavBar;
	COleDateTime m_DatePicked;
	COleDateTime m_DateToday;
private:
	COleDateTime m_InitialSelectedDate;
	COleDateTime m_DateEnd;
	COleDateTime m_DateStart;
	COleDateTime m_CurrentMonth;
	int m_nTick;

// Member functions ---------------------------------------------------------
protected:
	BOOL Create(COXCalendar * pBaseCalendar);
	BOOL ButtonTodayHit(CPoint pt);
	BOOL ButtonNoneHit(CPoint pt);
	
	COXCalendarPopup();

	virtual ~COXCalendarPopup();
	
	
	void SetButtons();
	
	int CalcBarWidth();
	
	void DecMonth(COleDateTime& month);
	
	void IncMonth(COleDateTime& month);
	
	void DoNavigationCheck(CPoint pt);
	
	BOOL CanShowMonth(COleDateTime& month);

	int HitTestNavigation(CPoint pt);
	
	int GridHitTest(CPoint pt);
	
	void GetLastShownDate(COleDateTime& d);
	
	void GetFirstShownDate(COleDateTime& d);

	BOOL  Pick(COleDateTime& pickDate,const COleDateTime& startDate,
		const COleDateTime& endDate,CRect rectCalendar);
	void CalcCellSize();
	CSize CalcWindowSize();

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXCalendarPopup)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(COXCalendarPopup)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////
#endif
