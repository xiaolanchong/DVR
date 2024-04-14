///////////////////////////////////////////////////////////////////////////////////////////////////
// win_console_painter.cpp -- painter for Windows
// ---------------------
// begin     : Sep 2005
// modified  : 20 Sep 2005
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaahaaah@hotmail.com, aaah@mail.ru
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../utility.h"

namespace alib
{

const int DEFAULT_WIN_POS = 16;
const int DEFAULT_IMAGE_WIDTH = 50;
const int DEFAULT_IMAGE_HEIGHT = 50;
const int DEFAULT_CONSOLE_HEIGHT = 128;
const int CONSOLE_BORDER = 3;
const int INTER_IMAGE_GAP = 3;
const int DEFAULT_CONSOLE_FONT_HEIGHT = 16;
const int EXTRA_TEXT_HEIGHT = 2;
const int FIRST_MENU_ITEM_ID = WM_USER;
const int MENU_ITEM_ID_RANGE = WM_APP - WM_USER;

const DWORD RESIZABLE_WINDOW = (WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
const DWORD NON_RESIZABLE_WINDOW = WS_CAPTION;

const TCHAR szWindowClass[] = _T("DemoWindowClass");     // the main window class name
static BOOL g_bRegistered = FALSE;

DWORD WINAPI WinPainterThreadProcEx( LPVOID lpParameter );
LRESULT CALLBACK WindowProcedureEx( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

static void FunctionFailMessage( const char * text )
{
  if (text != 0) std::cout << "ERROR: Function " << text << "() failed" << std::endl;
}

static void ErrorMessage( const char * text )
{
  if (text != 0) std::cout << "ERROR: " << text << std::endl;
}

//=================================================================================================
//=================================================================================================
struct WindowsConsolePainterEx : public alib::IConsolePainterEx
{
  CRITICAL_SECTION             m_critSect;          //!< data protector
  HWND                         m_hwnd;              //!< handle of demo window
  HWND                         m_hConsole;          //!< handle of console sub-window
  HINSTANCE                    m_hInstance;         //!< handle of application instance
  HANDLE                       m_hThread;           //!< handle of window thread
  AImage                       m_images[4];         //!< temporal images
  bool                         m_bResizable;        //!< nonzero if the window can be resized
  bool                         m_bInvertY;          //!< nonzero means that axis Y must be inverted on drawing
  bool                         m_bReady;            //!< nonzero if the object has been properly initialized
  StdStr                       m_title;             //!< window title
  StdStr                       m_statusText;        //!< status bar text
  int                          m_textHeight;        //!< height of status bar
  int                          m_consoleHeight;     //!< height of console sub-window
  int                          m_consoleFontHeight; //!< height (in points) of console font
  HMENU                        m_hMenu;             //!< float menu
  const Elvees::AbstractMenu * m_pMenu;             //!< pointer to the structure that keeps menu contents
  bool                         m_bWindowMoved;      //!< 
  RECT                         m_imageRect[4];      //!< placement rectangles of all images
  int                          m_imageNum;          //!< the number of pictures that can drawn in a window
  bool                         m_bLineLayout;       //!< uses 1xN picture layout if nonzero, otherwise 2x2 picture layout

///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
void RecalculateLayoutAndResizeWindow()
{
  RECT clientRect;
  int  clientW = 0, clientH = 0, W = 0, H = 0;

  if (m_hwnd == 0)
    return;

  ::GetClientRect( m_hwnd, &clientRect );

  // Calculate image width and height.
  for (int i = 0; i < 4; i++)
  {
    W = std::max<int>( W, (m_images[i]).width()  );
    H = std::max<int>( H, (m_images[i]).height() );
  }

  if ((W == 0) || (H == 0))
  {
    W = DEFAULT_IMAGE_WIDTH;
    H = DEFAULT_IMAGE_HEIGHT;
  }

  // Calculate client sizes of main window.
  clientW = clientH = 0;
  if (m_bLineLayout)
  {
    clientW = m_imageNum*W + (m_imageNum-1)*INTER_IMAGE_GAP;
    clientH = H;
  }
  else
  {
    clientW = ((m_imageNum < 2) ? W : (2*W+INTER_IMAGE_GAP));
    clientH = ((m_imageNum < 3) ? H : (2*H+INTER_IMAGE_GAP));
  }
  clientH += m_textHeight + m_consoleHeight + 2*CONSOLE_BORDER;

  // Recalculate layout.
  (m_imageRect[0]).left = 0;
  (m_imageRect[1]).left = W+INTER_IMAGE_GAP;
  (m_imageRect[2]).left = m_bLineLayout ? 2*(W+INTER_IMAGE_GAP) : 0*(W+INTER_IMAGE_GAP);
  (m_imageRect[3]).left = m_bLineLayout ? 3*(W+INTER_IMAGE_GAP) : 1*(W+INTER_IMAGE_GAP);

  (m_imageRect[0]).top = 0;
  (m_imageRect[1]).top = 0;
  (m_imageRect[2]).top = m_bLineLayout ? 0 : (H+INTER_IMAGE_GAP);
  (m_imageRect[3]).top = m_bLineLayout ? 0 : (H+INTER_IMAGE_GAP);

  for (int k = 0; k < 4; k++)
  {
    (m_imageRect[k]).right  = (m_imageRect[k]).left + W;
    (m_imageRect[k]).bottom = (m_imageRect[k]).top  + H;
  }

  // Resize window if necessary.
  if ((clientW != (clientRect.right-clientRect.left)) ||
      (clientH != (clientRect.bottom-clientRect.top)))
  {
    RECT rect, pos;

    rect.left = 0;
    rect.right = clientW;
    rect.top = 0;
    rect.bottom = clientH;

    if (::AdjustWindowRect( &rect, (m_bResizable ? RESIZABLE_WINDOW : NON_RESIZABLE_WINDOW), FALSE ))
    {
      if (::GetWindowRect( m_hwnd, &pos ))
      {
        ::MoveWindow( m_hwnd, pos.left, pos.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

        if ((m_hConsole != 0) && (m_consoleHeight > 0))
        {
          int cx = std::max<int>( clientW-2*CONSOLE_BORDER, CONSOLE_BORDER );
          ::MoveWindow( m_hConsole, CONSOLE_BORDER, clientH-m_consoleHeight-CONSOLE_BORDER, cx, m_consoleHeight, TRUE );
        }
      }
    }
  }
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
void PrintText( HDC hdc, const RECT & clientRect )
{
  SIZE size;
  RECT rect = clientRect;
  int  y = rect.bottom - m_textHeight + EXTRA_TEXT_HEIGHT/2;
  int  length = (int)(_tcslen( m_statusText.c_str() ));

  COLORREF old = ::SetBkColor( hdc, ::GetSysColor( COLOR_3DFACE ) );
  ::TextOut( hdc, 0, y, m_statusText.c_str(), length );
  ::GetTextExtentPoint32( hdc, m_statusText.c_str(), length, &size );
  rect.left = size.cx;
  rect.top = y;
  rect.bottom = y + m_textHeight;
  if ((rect.left < rect.right) && (rect.top < rect.bottom))
    FillRect( hdc, &rect, (HBRUSH)(COLOR_3DFACE+1) );
  ::SetBkColor( hdc, old );
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
void DrawImage( int imageNo, HDC hdc )
{
  if (!((0 <= imageNo) && (imageNo < 4)) || (m_images[imageNo]).empty())
    return;

  const AImage & image = m_images[imageNo];
  int            W = image.width();
  int            H = image.height();
  BITMAPINFO     bi;

  RecalculateLayoutAndResizeWindow();

  // Fill bitmap header.
  memset( &bi, 0, sizeof(bi) );
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = W;
  bi.bmiHeader.biHeight = (m_bInvertY ? -H : +H);
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;
  bi.bmiHeader.biSizeImage = image.size() * sizeof(AImage::value_type);

#if 0
  ::StretchDIBits( hdc, (m_imageRect[imageNo]).left, (m_imageRect[imageNo]).top, W, H,
                   0, 0, W, H, image.begin(), &bi, DIB_RGB_COLORS, SRCCOPY );
#else
  ::SetDIBitsToDevice( hdc, (m_imageRect[imageNo]).left, (m_imageRect[imageNo]).top, W, H,
                       0, 0, 0, H, image.begin(), &bi, DIB_RGB_COLORS );
#endif
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
WindowsConsolePainterEx( const TCHAR * title, bool bResizable, bool bInvertAxisY, int consoleHeight )
{
  ::InitializeCriticalSection( &m_critSect );
  m_hwnd = 0;
  m_hConsole = 0;
  m_hInstance = (HINSTANCE )(::GetModuleHandle( 0 ));
  m_hThread = 0;
  m_images;
  m_bResizable = bResizable;
  m_bInvertY = bInvertAxisY;
  m_bReady = false;
  m_title = (title != 0) ? title : _T("Demo window");
  m_statusText.clear();
  m_textHeight = 0;
  m_consoleHeight = ((consoleHeight > 0) ? consoleHeight : 0);
  m_consoleFontHeight = DEFAULT_CONSOLE_FONT_HEIGHT;
  m_hMenu = 0;
  m_pMenu = 0;
  m_bWindowMoved = false;
  memset( m_imageRect, 0, sizeof(m_imageRect) );
  m_imageNum = 0;
  m_bLineLayout = false;

  // Registering window class once in the main thread!
  if (m_hInstance != 0)
  {
    WNDCLASSEX wcex;

    wcex.cbSize        = sizeof(WNDCLASSEX); 
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = (WNDPROC)WindowProcedureEx;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = m_hInstance;
    wcex.hIcon         = LoadIcon( 0, IDI_APPLICATION );
    wcex.hCursor       = LoadCursor( 0, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
    wcex.lpszMenuName  = 0;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm       = 0;

    if (!g_bRegistered)
      g_bRegistered = ::RegisterClassEx( &wcex );

    if (g_bRegistered)
      m_hThread = ::CreateThread( 0, 0, WinPainterThreadProcEx, this, 0, 0 );
  }
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
virtual ~WindowsConsolePainterEx()
{
  ::EnterCriticalSection( &m_critSect );
  m_bReady = false;
  if ((m_hThread != 0) && (m_hwnd != 0))
  {
    DWORD wait = WAIT_TIMEOUT;
    for (int t = 10; (t < 3000) && (wait == WAIT_TIMEOUT); t += 10)
    {
      ::PostMessage( m_hwnd, WM_QUIT, 0, 0 );
      wait = ::WaitForSingleObject( m_hThread, t );
    }

    if ((wait == WAIT_TIMEOUT) || (wait == WAIT_FAILED))
    {
      std::cout << "Failed to close demo window normally" << std::endl;
      if (!(::TerminateThread( m_hThread, 1 )))
        std::cout << "Failed to terminate demo window's thread" << std::endl;
    }

    if (!(::CloseHandle( m_hThread )))
      std::cout << "Failed to close handle of demo window's thread" << std::endl;
  }
  ::LeaveCriticalSection( &m_critSect );

  ::DeleteCriticalSection( &m_critSect );
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
virtual void CopyAndDrawDemoImage( int imageNo, const AImage * pImage, const TCHAR * statusText  )
{
  if ((pImage == 0) || !((0 <= imageNo) && (imageNo < 4)))
    return;

  ::EnterCriticalSection( &m_critSect );
  if ((m_hwnd != 0) && m_bReady)
  {
    HDC hdc = ::GetDC( m_hwnd );
    if (hdc != 0)
    {
      RECT clientRect;
      ::GetClientRect( m_hwnd, &clientRect );

      if (pImage != 0) m_images[imageNo] = (*pImage);
      DrawImage( imageNo, hdc );

      m_statusText = (statusText != 0) ? statusText : _T("");
      PrintText( hdc, clientRect );

      ::ReleaseDC( m_hwnd, hdc );
    }
  }
  ::LeaveCriticalSection( &m_critSect );
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
virtual void CopyAndDrawDemoImage( int imageNo, const Arr2ub * pImage, const TCHAR * statusText, bool bColor16 )
{
  if ((pImage == 0) || !((0 <= imageNo) && (imageNo < 4)))
    return;

  ::EnterCriticalSection( &m_critSect );
  if ((m_hwnd != 0) && m_bReady)
  {
    HDC hdc = ::GetDC( m_hwnd );
    if (hdc != 0)
    {
      RECT clientRect;
      ::GetClientRect( m_hwnd, &clientRect );

      // Copy grayscaled image to color one.
      if (pImage != 0)
      {
        m_images[imageNo].resize2( *pImage, false );
        ASSERT( sizeof(AImage::value_type) == 4 );

        ubyte       * dst = reinterpret_cast<ubyte*>( m_images[imageNo].begin() );
        const ubyte * src = pImage->begin();

        for (int i = 0, n = m_images[imageNo].size(); i < n; i++)
        {
          if (bColor16 && (src[i] < 16))
          {
            *(reinterpret_cast<RGBQUAD*>( dst )) = alib::Colors16[ src[i] ];
          }
          else dst[0] = (dst[1] = (dst[2] = src[i]));

          dst += sizeof(AImage::value_type);
        }
        ASSERT( dst == reinterpret_cast<ubyte*>( (m_images[imageNo]).end() ) );
      }

      DrawImage( imageNo, hdc );

      m_statusText = (statusText != 0) ? statusText : _T("");
      PrintText( hdc, clientRect );

      ::ReleaseDC( m_hwnd, hdc );
    }
  }
  ::LeaveCriticalSection( &m_critSect );
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
virtual void SwapAndDrawDemoImage( int imageNo, AImage * pImage, const TCHAR * statusText  )
{
  if ((pImage == 0) || !((0 <= imageNo) && (imageNo < 4)))
    return;

  ::EnterCriticalSection( &m_critSect );
  if ((m_hwnd != 0) && m_bReady)
  {
    HDC hdc = ::GetDC( m_hwnd );
    if (hdc != 0)
    {
      RECT clientRect;
      ::GetClientRect( m_hwnd, &clientRect );

      if (pImage != 0) pImage->swap( m_images[imageNo] );
      DrawImage( imageNo, hdc );

      m_statusText = (statusText != 0) ? statusText : _T("");
      PrintText( hdc, clientRect );

      ::ReleaseDC( m_hwnd, hdc );
    }
  }
  ::LeaveCriticalSection( &m_critSect );
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
virtual void PrintConsoleText( const TCHAR * text )
{
  ::EnterCriticalSection( &m_critSect );
  if ((text != 0) && (m_hConsole != 0))
  {
    // Adjust console size.
    int nRow = 2;
    for (int i = 0; text[i] != _T('\0'); i++) { if (text[i] == _T('\n')) ++nRow; }
    if ((nRow*m_consoleFontHeight) != m_consoleHeight)
    {
      m_consoleHeight = nRow*m_consoleFontHeight;
      RecalculateLayoutAndResizeWindow();
    }

    // Print text into console window.
    ::SetWindowText( m_hConsole, text );
  }
  ::LeaveCriticalSection( &m_critSect );
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
virtual void InitializeMenu( const Elvees::AbstractMenu * pMenu )
{
  ::EnterCriticalSection( &m_critSect );

  // Destroy the previous menu.
  m_pMenu = 0;
  if (m_hMenu != 0)
  {
    if (!(::DestroyMenu( m_hMenu )))
      FunctionFailMessage( "DestroyMenu" );
    m_hMenu = 0;
  }

  // Create and fill float menu.
  if (!(pMenu->m_items.empty()))
  {
    m_hMenu = ::CreatePopupMenu();
    if (m_hMenu != 0)
    {
      Elvees::AbstractMenu::ItemArr::const_iterator i;
      for (i = pMenu->m_items.begin(); i != pMenu->m_items.end(); ++i)
      {
        if (ALIB_IS_RANGE( i->m_id, 0, MENU_ITEM_ID_RANGE ))
        {
          if (!(::AppendMenu( m_hMenu, (i->m_bSeparator ? MF_SEPARATOR : MF_STRING),
                              FIRST_MENU_ITEM_ID + i->m_id, i->m_title )))
            FunctionFailMessage( "AppendMenu" );
        }
        else ErrorMessage( "wrong menu id" );
      }
      m_pMenu = pMenu;  // store menu pointer
    }
    else FunctionFailMessage( "CreatePopupMenu" );
  }

  ::LeaveCriticalSection( &m_critSect );
}

}; // WindowsConsolePainterEx


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
DWORD WINAPI WinPainterThreadProcEx( LPVOID lpParameter )
{
  HFONT consoleFont = 0;

  WindowsConsolePainterEx * painter = reinterpret_cast<WindowsConsolePainterEx*>( lpParameter );
  if ((painter == 0) || (painter->m_hInstance == 0))
    return 1;

  // Create and show window.
  DWORD style = (painter->m_bResizable ? RESIZABLE_WINDOW : NON_RESIZABLE_WINDOW);
  painter->m_hwnd = ::CreateWindow( szWindowClass, painter->m_title.c_str(), (style | WS_OVERLAPPED),
                                    CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT,
                                    0, 0, painter->m_hInstance, 0 );
  if (painter->m_hwnd == 0)
    return 3;
  ShowWindow( painter->m_hwnd, SW_SHOWNORMAL );
  UpdateWindow( painter->m_hwnd );

  // Calculate status bar height.
  HDC hdc = ::GetDC( painter->m_hwnd );
  if (hdc != 0)
  {
    TEXTMETRIC tm;
    ::GetTextMetrics( hdc, &tm );
    ::ReleaseDC( painter->m_hwnd, hdc );
    painter->m_textHeight = tm.tmHeight + EXTRA_TEXT_HEIGHT;
  }

  // Store pointer to the painter as associated window data.
  ::SetWindowLongPtr( painter->m_hwnd, GWLP_USERDATA, (LONG_PTR)painter );
  ::SetWindowPos( painter->m_hwnd, HWND_TOP, DEFAULT_WIN_POS, DEFAULT_WIN_POS,
                  DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT,
                  SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE );

  // .
  painter->m_hConsole = 0;
  if (painter->m_consoleHeight > 0)
  {
    painter->m_hConsole = ::CreateWindow( _T("STATIC"), _T("Console"), SS_LEFT | SS_SUNKEN | WS_CHILD | WS_VISIBLE,
                                          0, 0, 1, 1, painter->m_hwnd, 0, painter->m_hInstance, 0 );
    if (painter->m_hConsole == 0)
      return 4;

    HFONT consoleFont = ::CreateFont(
      painter->m_consoleFontHeight, 0, 0, 0, // nHeight, nWidth, nEscapement, nOrientation
      FW_NORMAL, FALSE, FALSE, 0,            // nWeight, bItalic, bUnderline, cStrikeOut
      ANSI_CHARSET,                          // nCharSet
      OUT_DEFAULT_PRECIS,                    // nOutPrecision
      CLIP_DEFAULT_PRECIS,                   // nClipPrecision
      DEFAULT_QUALITY,                       // nQuality
      DEFAULT_PITCH | FF_MODERN,             // nPitchAndFamily
      _T("Courier") );                       // lpszFacename
    if ((painter->m_hConsole != 0) && (consoleFont != 0))
      ::SendMessage( painter->m_hConsole, WM_SETFONT, (WPARAM)consoleFont, (LPARAM)TRUE );
  }

  // .
  painter->RecalculateLayoutAndResizeWindow();

  // Windows' main message loop.
  MSG msg;
  painter->m_bReady = true;
  while (GetMessage( &msg, 0, 0, 0 )) 
  {
    if (!TranslateAccelerator( msg.hwnd, 0, &msg )) 
    {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
  }

  if (consoleFont != 0)
    ::DeleteObject( consoleFont );
  if (painter->m_hMenu != 0)
    ::DestroyMenu( painter->m_hMenu );

  return 0;
}


///------------------------------------------------------------------------------------------------
//  FUNCTION: WindowProcedureEx(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND - process the application menu
//  WM_PAINT   - Paint the main window
//  WM_DESTROY - post a quit message and return
///------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProcedureEx( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
  WindowsConsolePainterEx * painter = 0;

  painter = reinterpret_cast<WindowsConsolePainterEx*>( ::GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
  switch (message) 
  {
    case WM_PAINT:
    {
      if ((painter != 0) && ::TryEnterCriticalSection( &(painter->m_critSect) ))
      {
        PAINTSTRUCT ps;
        HDC  hdc = BeginPaint( hwnd, &ps );
        RECT clientRect;
        ::GetClientRect( hwnd, &clientRect );
        painter->DrawImage( 0, hdc );
        painter->DrawImage( 1, hdc );
        painter->DrawImage( 2, hdc );
        painter->DrawImage( 3, hdc );
        painter->PrintText( hdc, clientRect );
        EndPaint( hwnd, &ps );
        ::LeaveCriticalSection( &(painter->m_critSect) );
      }
    }
    break;

    case WM_DESTROY:
    {
      PostQuitMessage(0);
    }
    break;

    case WM_SYSCOMMAND:
    {
      if ((wParam & 0xFFF0) == SC_CLOSE)
        return 0;
    }
    break;

    case WM_RBUTTONDOWN:
    {
      if ((painter != 0) && (painter->m_hMenu != 0) && (painter->m_pMenu != 0))
      {
        if (::TryEnterCriticalSection( &(painter->m_critSect) ))
        {
          POINT pt;
          pt.x = LOWORD( lParam );
          pt.y = HIWORD( lParam );
          ::GetCursorPos( &pt );

          // Change menu appearance according to current state.
          int nTop = ::GetMenuItemCount( painter->m_hMenu );
          if (nTop >= 0)
          {
            nTop = ALIB_MIN( nTop, (int)(painter->m_pMenu->m_items.size()) );
            for (int t = 0; t < nTop; t++)
            {
              const Elvees::AbstractMenu::Item & i = painter->m_pMenu->m_items[t];
              sint id = i.m_id + FIRST_MENU_ITEM_ID;
              UINT bCheck = (i.m_bChecked ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND;
              UINT bEnable = (i.m_bDisabled ? MF_DISABLED : MF_ENABLED) | MF_BYCOMMAND;

              if (::CheckMenuItem( painter->m_hMenu, id, bCheck ) == (DWORD)(-1))
                FunctionFailMessage( "CheckMenuItem" );
              if (::EnableMenuItem( painter->m_hMenu, id, bEnable ) == (BOOL)(-1))
                FunctionFailMessage( "EnableMenuItem" );
            }
          }
          else FunctionFailMessage( "GetMenuItemCount" );

          ::TrackPopupMenu( painter->m_hMenu, (TPM_LEFTALIGN | TPM_LEFTBUTTON), pt.x, pt.y, 0, hwnd, 0 );
          ::LeaveCriticalSection( &(painter->m_critSect) );
        }
      }
    }
    break;

    case WM_COMMAND:
    {
      if ((painter != 0) && (painter->m_pMenu != 0) && (HIWORD( wParam ) <= 1))
        painter->m_pMenu->SetSelectedItemId( (LONG)((sint)LOWORD( wParam ) - FIRST_MENU_ITEM_ID) );
    }
    break;
  }
  return DefWindowProc( hwnd, message, wParam, lParam );
}


///------------------------------------------------------------------------------------------------
/// \brief .
///------------------------------------------------------------------------------------------------
alib::IConsolePainterEx * CreateWindowsConsolePainterEx( const TCHAR * title,
                                                         bool          bResizable,
                                                         bool          bInvertAxisY,
                                                         int           consoleHeight,
                                                         int           paneNum,
                                                         bool          bLineLayout )
{
  if (!((1 <= paneNum) && (paneNum <= 4)))
    return false;

  WindowsConsolePainterEx * p = new WindowsConsolePainterEx( title, bResizable, bInvertAxisY, consoleHeight );
  p->m_imageNum = paneNum;
  p->m_bLineLayout = bLineLayout;
  return p;
}

} // namespace alib



///------------------------------------------------------------------------------------------------
/* This code dedicated to two-level abstract menu.
///------------------------------------------------------------------------------------------------

// Code comes from LRESULT CALLBACK WindowProcedureEx( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )

    case WM_RBUTTONDOWN:
    {
      if ((painter != 0) && (painter->m_hMenu != 0) && (painter->m_pMenu != 0))
      {
        if (::TryEnterCriticalSection( &(painter->m_critSect) ))
        {
          POINT pt;
          pt.x = LOWORD( lParam );
          pt.y = HIWORD( lParam );
          ::GetCursorPos( &pt );

          int nTop = ::GetMenuItemCount( painter->m_hMenu );
          if (nTop >= 0)
          {
            nTop = std::min( nTop, (int)(painter->m_pMenu->m_topMenu.size()) );
            for (int t = 0; t < nTop; t++)
            {
              const Elvees::AbstractMenu::TopItem & ti = painter->m_pMenu->m_topMenu[t];
              sint id = ti.m_id + FIRST_MENU_ITEM_ID;
              UINT bCheck = ((ti.m_bChecked && ti.m_subMenu.empty()) ? MF_CHECKED : MF_UNCHECKED);
              UINT bEnable = (ti.m_bDisabled ? MF_DISABLED : MF_ENABLED);

              if (::CheckMenuItem( painter->m_hMenu, id, MF_BYCOMMAND | bCheck ) == (DWORD)(-1))
                FunctionFailMessage( "CheckMenuItem" );
              if (::EnableMenuItem( painter->m_hMenu, id, MF_BYCOMMAND | bEnable ) == (BOOL)(-1))
                FunctionFailMessage( "EnableMenuItem" );

              if (!(ti.m_subMenu.empty()))
              {
                HMENU hSubMenu = ::GetSubMenu( painter->m_hMenu, t );
                if (hSubMenu != 0)
                {
                  int nSub = ::GetMenuItemCount( hSubMenu );
                  if (nSub >= 0)
                  {
                    nSub = std::min( nSub, (int)(ti.m_subMenu.size()) );
                    for (int s = 0; s < nSub; s++)
                    {
                      const Elvees::AbstractMenu::SubItem & si = ti.m_subMenu[s];
                      sint id = si.m_id + FIRST_MENU_ITEM_ID;
                      UINT bCheck = (si.m_bChecked ? MF_CHECKED : MF_UNCHECKED);
                      UINT bEnable = (si.m_bDisabled ? MF_DISABLED : MF_ENABLED);

                      if (::CheckMenuItem( painter->m_hMenu, id, MF_BYCOMMAND | bCheck ) == (DWORD)(-1))
                        FunctionFailMessage( "CheckMenuItem" );
                      if (::EnableMenuItem( painter->m_hMenu, id, MF_BYCOMMAND | bEnable ) == (BOOL)(-1))
                        FunctionFailMessage( "EnableMenuItem" );
                    }
                  }
                  else FunctionFailMessage( "GetMenuItemCount" );
                }
                else FunctionFailMessage( "GetSubMenu" );
              }
            }
          }
          else FunctionFailMessage( "GetMenuItemCount" );

          ::TrackPopupMenu( painter->m_hMenu, (TPM_LEFTALIGN | TPM_LEFTBUTTON), pt.x, pt.y, 0, hwnd, 0 );
          ::LeaveCriticalSection( &(painter->m_critSect) );
        }
      }
    }
    break;


///------------------------------------------------------------------------------------------------
///------------------------------------------------------------------------------------------------
virtual void InitializeMenu( const Elvees::AbstractMenu * pMenu )
{
  ::EnterCriticalSection( &m_critSect );

  // Destroy the previous menu.
  m_pMenu = 0;
  if (m_hMenu != 0)
  {
    if (!(::DestroyMenu( m_hMenu )))
      FunctionFailMessage( "DestroyMenu" );
    m_hMenu = 0;
  }

  // Create a float menu.
  if (!(pMenu->m_topMenu.empty()))
  {
    m_hMenu = ::CreatePopupMenu();
    if (m_hMenu != 0)
    {
      // Fill the float menu.
      Elvees::AbstractMenu::TopItemArr::const_iterator topIt;
      for (topIt = pMenu->m_topMenu.begin(); topIt != pMenu->m_topMenu.end(); ++topIt)
      {
        if (!(ALIB_IS_RANGE( topIt->m_id, 0, MENU_ITEM_ID_RANGE )))
        {
          ErrorMessage( "wrong menu id" );
          continue;
        }

        // Insert an ordinary menu item in the first level menu ...
        if (topIt->m_subMenu.empty())
        {
          if (!(::AppendMenu( m_hMenu, (topIt->m_bSeparator ? MF_SEPARATOR : MF_STRING),
                              FIRST_MENU_ITEM_ID + topIt->m_id, topIt->m_title )))
            FunctionFailMessage( "AppendMenu" );
        }
        else // ... otherwise insert a submenu.
        {
          // Create popup submenu.
          HMENU hSub = ::CreateMenu();
          if (hSub != 0)
          {
            // Append second level submenu to the first level menu.
            if (!(::AppendMenu( m_hMenu, MF_POPUP, (UINT_PTR)hSub, topIt->m_title )))
              FunctionFailMessage( "AppendMenu" );

            // Insert menu items into submenu.
            Elvees::AbstractMenu::SubItemArr::const_iterator subIt;
            for (subIt = topIt->m_subMenu.begin(); subIt != topIt->m_subMenu.end(); ++subIt)
            {
              if (ALIB_IS_RANGE( subIt->m_id, 0, MENU_ITEM_ID_RANGE ))
              {
                if (!(::AppendMenu( m_hMenu, (topIt->m_bSeparator ? MF_SEPARATOR : MF_STRING),
                                    FIRST_MENU_ITEM_ID + subIt->m_id, subIt->m_title )))
                  FunctionFailMessage( "AppendMenu" );
              }
              else ErrorMessage( "wrong menu id" );
            }
          }
          else FunctionFailMessage( "CreateMenu" );
        }
      }

      // Store menu pointer.
      m_pMenu = pMenu;
    }
    else FunctionFailMessage( "CreatePopupMenu" );
  }

  ::LeaveCriticalSection( &m_critSect );
}
*/

