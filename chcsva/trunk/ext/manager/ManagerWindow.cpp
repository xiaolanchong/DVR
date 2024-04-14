//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Управлением окном отображения списка потоков

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "ManagerWindow.h"
#include "../../resource.h"
#include "../../DlgShowStream.h"

// Windows sends this message when the taskbar is created. This can 
// happen if it crashes and Windows has to restart it.
const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(TEXT("TaskbarCreated"));

//======================================================================================//
//                                 class ManagerWindow                                  //
//======================================================================================//

ManagerWindow::ManagerWindow(bool bServerMode):
	m_bServerMode(bServerMode),
	m_hServerWnd(0)
{
	InitCommonControls();
	m_hServerWnd = CreateDialogParam(_Module.GetModuleInstance(),
		MAKEINTRESOURCE(IDD_MANAGER), NULL, ManagerWndProc, (LPARAM)this);

	if(!m_hServerWnd)
		Elvees::Output(Elvees::TError, TEXT("Fail create manager window"));
}

ManagerWindow::~ManagerWindow()
{
	if(::IsWindow(m_hServerWnd))
	{
		::ShowWindow(m_hServerWnd, SW_HIDE);
		NOTIFYICONDATA IconData = { sizeof(NOTIFYICONDATA) };
		IconData.uID = 1;
		IconData.hWnd = m_hServerWnd;
		::Shell_NotifyIcon(NIM_DELETE, &IconData);
		::DestroyWindow(m_hServerWnd);
	}
}

void ManagerWindow::SetStreams( const std::vector< StreamInfo >& StreamArr )
{
	{
	Elvees::CCriticalSection::Owner lock(m_listSection);
	m_StreamArr = StreamArr;
	}
	SendMessage(m_hServerWnd, WM_UPDATE_STREAM_LIST, 0, 0);
}

class FindStreamPred
{
	const UUID	m_streamUID;
	const long	m_DevicePin;
public:
	bool operator()( const StreamInfo& info ) const
	{
		return	info.m_StreamUID == m_streamUID &&
				info.m_DevicePin == m_DevicePin;	
	}

	FindStreamPred( const UUID& streamUID, long DevicePin ):
		m_streamUID( streamUID ),
		m_DevicePin( DevicePin )
	{
	}
};

CHCS::IStream*	ManagerWindow::StreamByUID(const UUID& streamUID, long lPin) const
{
	std::vector<StreamInfo>::const_iterator it = 
		std::find_if( m_StreamArr.begin(), m_StreamArr.end(), FindStreamPred(streamUID, lPin) );

	if( it != m_StreamArr.end() )
	{
		return it->m_pStream;
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// manager window
//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK ManagerWindow::ManagerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = TRUE;
	ManagerWindow* pThis = (ManagerWindow*)(LONG_PTR)GetWindowLong(hWnd, GWL_USERDATA);

	// Restore taskbar icon if it crashes
	if(message == WM_TASKBARCREATED)
	{
		// Add icon to tray..
		NOTIFYICONDATA IconData;
		IconData.cbSize = sizeof(NOTIFYICONDATA);
		IconData.uID = 1;
		IconData.hWnd = hWnd;
		IconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		IconData.hIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(
			!pThis->IsServerWindow() ? IDI_MAIN_ICON_CLIENT : IDI_MAIN_ICON));
		IconData.uCallbackMessage = WM_TRAYICON;
		lstrcpy(IconData.szTip, TEXT("Manager"));

		::Shell_NotifyIcon(NIM_ADD, &IconData);
		return TRUE;
	}

	switch(message)
	{
	case WM_INITDIALOG:
		{
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)lParam);
			pThis = reinterpret_cast<ManagerWindow*>(lParam);

			HICON hIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(
				!pThis->IsServerWindow() ? IDI_MAIN_ICON_CLIENT : IDI_MAIN_ICON));

			::SendMessage(hWnd, WM_SETICON, FALSE, (LPARAM)hIcon);
			::SendMessage(hWnd, WM_SETICON, TRUE, (LPARAM)hIcon);

			HWND hWndStreamList = ::GetDlgItem(hWnd, IDC_STREAMS_LIST);

			// Ex style

			ListView_SetExtendedListViewStyle(hWndStreamList,
				ListView_GetExtendedListViewStyle(hWndStreamList) | LVS_EX_FULLROWSELECT);

			ListView_SetBkColor(hWndStreamList, GetSysColor(COLOR_MENU));
			ListView_SetTextBkColor(hWndStreamList, GetSysColor(COLOR_MENU));

			LVCOLUMN column;
			column.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
			column.fmt = LVCFMT_LEFT;

			column.cx = 25;
			column.pszText = TEXT("ID");
			ListView_InsertColumn(hWndStreamList, 0, &column);

			column.cx = 100;
			column.pszText = TEXT("IP");
			ListView_InsertColumn(hWndStreamList, 1, &column);

			column.cx = 225;
			column.pszText = TEXT("UID");
			ListView_InsertColumn(hWndStreamList, 2, &column);

			column.cx = 35;
			column.pszText = TEXT("Pin");
			ListView_InsertColumn(hWndStreamList, 3, &column);

			//////////////////////////////////////////////////////////////////////////

			// Add icon to tray..
			NOTIFYICONDATA IconData;
			IconData.cbSize = sizeof(NOTIFYICONDATA);
			IconData.uID = 1;
			IconData.hWnd = hWnd;
			IconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
			IconData.hIcon = hIcon;
			IconData.uCallbackMessage = WM_TRAYICON;
			lstrcpy(IconData.szTip, TEXT("Manager"));

			::Shell_NotifyIcon(NIM_ADD, &IconData);

			// Set minimum dimensions
			::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOMOVE);
		}
		break;

	case WM_UPDATE_STREAM_LIST:
		{
			Elvees::CCriticalSection::Owner lock(pThis->m_listSection);

			UUID  streamID;
			UuidString pszUuid;
			TCHAR stStreamID[64];

			LVITEM listItem;
			HWND hWndStreamList = ::GetDlgItem(hWnd, IDC_STREAMS_LIST);

			listItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			listItem.iItem = 0;
			listItem.iSubItem = 0;
			listItem.iImage = 0;
			listItem.lParam = (LPARAM)0;

			ListView_DeleteAllItems(hWndStreamList);

			for ( size_t i = 0; i < pThis->m_StreamArr.size(); ++i )
			{
				const StreamInfo& info = pThis->m_StreamArr[i];
				CHCS::IStream* pStream = info.m_pStream;
				// Insert stream ID
				wsprintf(stStreamID, TEXT("%ld"), info.m_StreamID );

				listItem.pszText = stStreamID;
				ListView_InsertItem(hWndStreamList, &listItem);

				// Stream IP

				if(info.m_bIsLocal )
				{
					if(Elvees::IsFileExists(info.m_stMoniker.c_str()))
						lstrcpy(stStreamID, TEXT("Local / File"));
					else
						lstrcpy(stStreamID, TEXT("Local / Device"));
				}
				else
				{
					lstrcpy(stStreamID, info.m_stMoniker.c_str());
				}

				ListView_SetItemText(hWndStreamList, listItem.iItem, 1, stStreamID);

				// Stream UID
				pszUuid = 0;
				streamID = info.m_StreamUID;

				if(::UuidToString(&streamID, &pszUuid) == RPC_S_OK && pszUuid)
				{
					wsprintf(stStreamID, TEXT("{%s}"), pszUuid);
					RpcStringFree(&pszUuid);
				}
				else // very unlikely
					lstrcpy(stStreamID, TEXT("{UUID conversion failed}"));

				ListView_SetItemText(hWndStreamList, listItem.iItem, 2, stStreamID);

				wsprintf(stStreamID, TEXT("%ld"), info.m_DevicePin );
				ListView_SetItemText(hWndStreamList, listItem.iItem, 3, stStreamID);

			}
		}
		break;

	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		break;

	case WM_SYSCOMMAND:
		{
			if(wParam == SC_MINIMIZE)
				ShowWindow(hWnd, SW_HIDE);
			else
				bRet = FALSE; 
		}
		break;


	case WM_GETMINMAXINFO:
		{
			PMINMAXINFO pInfo = (PMINMAXINFO)lParam;

			pInfo->ptMinTrackSize.x = 400;
			pInfo->ptMinTrackSize.y = 150;
		}
		break;

	case WM_DESTROY:
		{
			NOTIFYICONDATA IconData = { sizeof(NOTIFYICONDATA) };
			IconData.uID = 1;
			IconData.hWnd = hWnd;
			::Shell_NotifyIcon(NIM_DELETE, &IconData);

			::PostQuitMessage(0);
		}
		break;

	case WM_SIZE:
		{
			int cx = LOWORD(lParam);
			int cy = HIWORD(lParam);

			::SetWindowPos(::GetDlgItem(hWnd, IDC_STREAMS_LIST), NULL, 0, 0, cx, cy, SWP_NOZORDER);
		}
		break;

	case WM_COMMAND:
		{
			WORD wID = LOWORD(wParam); 

			if(ID_TRAY_SHOW == wID)
			{
				if(!::IsWindowVisible(hWnd))
				{
					RECT rcWnd, rcArea;

					::GetWindowRect(hWnd, &rcWnd);
					::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);

					::SetWindowPos(hWnd, NULL, 
						rcArea.right  - (rcWnd.right - rcWnd.left),
						rcArea.bottom - (rcWnd.bottom - rcWnd.top),
						0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW);
				}

				::SetForegroundWindow(hWnd);
			}
			else if(ID_TRAY_UPDATE == wID)
			{
				::SendMessage(hWnd, WM_UPDATE_STREAM_LIST, 0, 0);
			}
			else if(ID_TRAY_EXIT == wID)
			{
				::DestroyWindow(hWnd);
			}
		}
		break;

	case WM_NOTIFY:
		{
			int idCtrl = (int)wParam;
			LPNMHDR pnmh = (LPNMHDR)lParam;

			if(idCtrl == IDC_STREAMS_LIST && pnmh->code == NM_DBLCLK)
			{
				HWND hWndStreamsList = ::GetDlgItem(hWnd, IDC_STREAMS_LIST);
				LVHITTESTINFO listInfo;

				::GetCursorPos(&(listInfo.pt));
				::ScreenToClient(hWndStreamsList, &(listInfo.pt));

				ListView_HitTest(hWndStreamsList, &listInfo);

				if(listInfo.iItem != -1)
				{
					TCHAR stStreamID[64];

					long lPin;
					UUID streamUID;

					ListView_GetItemText(hWndStreamsList, listInfo.iItem, 3, 
						stStreamID, countof(stStreamID));

					lPin = _ttol(stStreamID);

					ListView_GetItemText(hWndStreamsList, listInfo.iItem, 2, 
						stStreamID, countof(stStreamID));

					if(StringToUuid(stStreamID, &streamUID))
					{
						CHCS::IStream* pStream = pThis->StreamByUID(streamUID, lPin);
#if 1
						CDlgShowStream::Create(hWnd, pStream);
					//	pStream->Release();
#else
						if(pStream)
						{
							CDlgShowStream dlg(pStream);
							dlg.DoModal(hWnd);

							pStream->Release();
						}
#endif
					}

					//	MessageBox(hWnd, stStreamID, TEXT(""), MB_OK);
				}
			}
		}
		break;

	case WM_TRAYICON:
		{
			DWORD wID    = (DWORD)wParam;
			DWORD lEvent = (DWORD)lParam;

			if(wID != 1 || (lEvent != WM_RBUTTONUP && lEvent != WM_LBUTTONDBLCLK))
				return 0;

			// If there's a resource menu with the same ID as the icon, use it as
			// the right-button popup menu. TrayIcon will interprets the first
			// item in the menu as the default command for WM_LBUTTONDBLCLK

			HMENU hMenu = ::LoadMenu(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_SYSTRAY_MENU));
			if(hMenu == NULL) return 0;

			HMENU hSubMenu = ::GetSubMenu(hMenu, 0);

			if(lEvent == WM_RBUTTONUP) {
				// Make first menu item the default (bold font)
				::SetMenuDefaultItem(hSubMenu, 0, TRUE);
				// Display the menu at the current mouse location. There's a "bug"
				// (Microsoft calls it a feature) in Windows 95 that requires calling
				// SetForegroundWindow. To find out more, search for Q135788 in MSDN.

				POINT mouse;
				::GetCursorPos(&mouse);

				::SetForegroundWindow(hWnd);
				::TrackPopupMenu(hSubMenu, 0, mouse.x, mouse.y, 0, hWnd, NULL);
				::PostMessage(hWnd, WM_NULL, 0, 0);
			} else
				// double click: execute first menu item
				::SendMessage(hWnd, WM_COMMAND, ::GetMenuItemID(hSubMenu, 0), 0);

			::DestroyMenu(hMenu);
		}
		break;

	default:
		bRet = FALSE;
	}

	return bRet;
}