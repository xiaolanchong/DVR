///////////////////////////////////////////////////////////////////////////////////////////////////
// i_console_painter.h
// ---------------------
// begin     : Sep 2005
// modified  : 26 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaahaaah@hotmail.com, aaah@mail.ru
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_CONSOLE_PAINTER_H__
#define __DEFINE_ALIB_CONSOLE_PAINTER_H__

namespace alib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct IConsolePainter.
/// \brief  IConsolePainter.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct IConsolePainter
{
  virtual ~IConsolePainter() {}
  virtual void CopyAndDrawDemoImage( const Arr2ub * pImage, const TCHAR * statusText, bool bColor16 ) = 0;
  virtual void CopyAndDrawDemoImage( const AImage * pImage, const TCHAR * statusText ) = 0;
  //virtual void SwapAndDrawDemoImage( Arr2ub * pImage, const TCHAR * statusText, bool bColor16 ) = 0;
  virtual void SwapAndDrawDemoImage( AImage * pImage, const TCHAR * statusText  ) = 0;
  virtual void PrintConsoleText( const TCHAR * text ) = 0;
  virtual void InitializeMenu( const Elvees::AbstractMenu * pMenu ) = 0;

//>>>>>
virtual HDC & GetDC() = 0;
//>>>>>
};

typedef  std::auto_ptr<alib::IConsolePainter>  IConsolePainterPtr;


///================================================================================================
/// \struct IConsolePainterEx.
/// \brief  IConsolePainterEx.
///================================================================================================
struct IConsolePainterEx
{
  virtual ~IConsolePainterEx() {}
  virtual void CopyAndDrawDemoImage( int imageNo, const Arr2ub * pImage, const TCHAR * statusText, bool bColor16 ) = 0;
  virtual void CopyAndDrawDemoImage( int imageNo, const AImage * pImage, const TCHAR * statusText ) = 0;
  //virtual void SwapAndDrawDemoImage( int imageNo, Arr2ub * pImage, const TCHAR * statusText, bool bColor16 ) = 0;
  virtual void SwapAndDrawDemoImage( int imageNo, AImage * pImage, const TCHAR * statusText  ) = 0;
  virtual void PrintConsoleText( const TCHAR * text ) = 0;
  virtual void InitializeMenu( const Elvees::AbstractMenu * pMenu ) = 0;
};

typedef  std::auto_ptr<alib::IConsolePainterEx>  IConsolePainterExPtr;

#ifdef _WIN32
alib::IConsolePainter * CreateWindowsConsolePainter( const TCHAR * title, bool bResizable, bool bInvertAxisY, int consoleHeight = -1 );

alib::IConsolePainterEx * CreateWindowsConsolePainterEx( const TCHAR * title,
                                                         bool          bResizable,
                                                         bool          bInvertAxisY,
                                                         int           consoleHeight = -1,
                                                         int           paneNum = 1,
                                                         bool          bLineLayout = true );
#endif // _WIN32

} // namespace alib

#endif // __DEFINE_ALIB_CONSOLE_PAINTER_H__

