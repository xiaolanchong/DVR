//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс отображения кадров в переданное окно

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _STREAM_WINDOW_7566432039795916_
#define _STREAM_WINDOW_7566432039795916_

#include "FrameRender.h"

//======================================================================================//
//                                  class StreamWindow                                  //
//======================================================================================//

namespace CHCS
{
	class	IStreamManager;
	struct	IStream;
}

//! \brief класс для отображения на окне кадров с камер
//! \version 1.0
//! \date 05-29-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class StreamWindow : public CWindowImpl<StreamWindow>
{
	//! текущий поток для отображения в окно
	boost::shared_ptr<CHCS::IStream>	m_pStream;

	CString								m_sStreamDescription;

	FrameRender							m_Render;

	boost::shared_ptr<HFONT>			m_TitleFont;
public:
	// Optionally specify name of the new Windows class
	DECLARE_WND_CLASS(_T("InstallHelperStreamWindow")) 

	BEGIN_MSG_MAP(StreamWindow)
		MESSAGE_HANDLER(WM_PAINT,		OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND,	OnEraseBknd)
		MESSAGE_HANDLER(WM_TIMER,		OnTimer)
	END_MSG_MAP()

	StreamWindow();
	~StreamWindow();
private:

	//! обработчик WM_PAINT, перерисовать кадр с потока, если есть, 
	//!  или просто закрасить цветом заднего фона
	//! \param nMsg 
	//! \param wParam 
	//! \param lParam 
	//! \param bHandled 
	//! \return 
	LRESULT OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//! обработчик WM_TIMER, периодически перерисовывать
	//! \param nMsg 
	//! \param wParam 
	//! \param lParam 
	//! \param bHandled 
	//! \return 
	LRESULT OnTimer(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		InvalidateRect(0);
		return 0;
	}

	//! обработчик WM_ERASEBKGND, переисовывать фон не надо, anti-flicking
	//! \param nMsg 
	//! \param wParam 
	//! \param lParam 
	//! \param bHandled 
	//! \return 
	LRESULT OnEraseBknd(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return 1;
	}

	//! закрасить, если кадров нет
	//! \param hDC 
	//! \param rc 
	static void	FillWithBackgroundColor( HDC hDC, const RECT& rc )
	{
		COLORREF clr = GetSysColor( COLOR_BTNFACE );

		::SetBkColor(hDC, clr);
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}

	void	DrawStreamDescription( HDC hDC, const RECT& rc )
	{
		RECT rcStub(rc);
		::SetBkMode( hDC, TRANSPARENT );
		::SetTextColor( hDC, RGB(255, 255, 255) );
		::DrawText( hDC, m_sStreamDescription, -1, &rcStub, DT_SINGLELINE|DT_TOP|DT_LEFT  )	;
	}
public:

	//! установить источник кадров
	//! \param pStream новый поток для отрисовки, если != 0, иначе освободить поток
	void	SetStream( boost::shared_ptr<CHCS::IStream> pStream, CString sStreamDescription )
	{
		m_pStream				= pStream;
		m_sStreamDescription	= sStreamDescription;
	}

	void	ReleaseStream()
	{
		m_pStream.reset();
		m_sStreamDescription.Empty();
	}

};
#endif // _STREAM_WINDOW_7566432039795916_