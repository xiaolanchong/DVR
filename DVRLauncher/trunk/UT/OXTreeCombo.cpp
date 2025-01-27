// Copyright � Dundas Software Ltd. 1997 - 1998, All Rights Reserved
                         
// OXTreeCombo.cpp : implementation file
//

#include "stdafx.h"
#include "OXTreeCombo.h"
#include "OXTreeCtrl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COXTreeCombo

COXTreeCombo::COXTreeCombo()
{
}

COXTreeCombo::~COXTreeCombo()
{
}


BEGIN_MESSAGE_MAP(COXTreeCombo, CComboBox)
	//{{AFX_MSG_MAP(COXTreeCombo)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COXTreeCombo message handlers

void COXTreeCombo::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	FinishEdit(TRUE);	
}

void COXTreeCombo::Init(HTREEITEM hItem,int iItem,int iSubItem)
{
	COXTreeCtrl*pTreeCtrl = (COXTreeCtrl*)GetParent();
	m_bNotifySent = FALSE;
	m_nItem = iItem;
	m_nSubItem = iSubItem;
	
	CStringArray& sa = pTreeCtrl->GetItemTextEx(hItem,iSubItem);
	for(int i=0;i < sa.GetSize();i++)
		AddString(sa[i]);
	int nCurIdx = FindString(-1,pTreeCtrl->GetItemText(hItem,iSubItem));
	if(nCurIdx != -1)
		SetCurSel(nCurIdx);
}

void COXTreeCombo::FinishEdit(BOOL bOK)
{
	CString str;
	GetWindowText(str);

	// Send Notification to parent 
	LV_DISPINFO di;
	di.hdr.hwndFrom = GetParent()->m_hWnd;
	di.hdr.idFrom = GetDlgCtrlID();
	di.hdr.code = LVN_ENDLABELEDIT;

	di.item.mask = LVIF_TEXT;
	di.item.iItem = bOK ? m_nItem : -1;
	di.item.iSubItem = m_nSubItem;
	di.item.pszText = LPTSTR((LPCTSTR)str);
	di.item.cchTextMax = str.GetLength();

	if(!m_bNotifySent)
		GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&di );
	m_bNotifySent = TRUE;
	// and close window too
	DestroyWindow();
}

BOOL COXTreeCombo::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			FinishEdit(TRUE);
			return TRUE;
		}
		else if(pMsg->wParam == VK_ESCAPE)
		{
			FinishEdit(FALSE);
			return TRUE;
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}
