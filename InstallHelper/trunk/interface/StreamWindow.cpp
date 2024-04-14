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

#include "stdafx.h"
#include "StreamWindow.h"

void DeleteHFONT( HFONT * pFont )
{
	if( pFont )
	{
		DeleteFont( *pFont ); 
		boost::checked_delete( pFont );
	}
}
//======================================================================================//
//                                  class StreamWindow                                  //
//======================================================================================//

StreamWindow::StreamWindow()
{
	NONCLIENTMETRICS ncm;
	memset( & ncm, 0, sizeof( NONCLIENTMETRICS ) );
	ncm.cbSize = sizeof( ncm );
	BOOL res = SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0 );
	if( res )
	{
		HFONT hFont = CreateFontIndirect( &ncm.lfMessageFont ); 
		m_TitleFont.reset( new HFONT( hFont ), &::DeleteHFONT );  
	}
}

StreamWindow::~StreamWindow()
{
}

LRESULT StreamWindow::OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(&ps);

	HGDIOBJ OldFont = 0;
	if( m_TitleFont.get() )
	{
		OldFont = SelectObject( hDC, *m_TitleFont.get() );
	}

	RECT rcClient;
	GetClientRect( &rcClient );

	if( m_pStream.get() )
	{

		CHCS::IFrame *pFrame = 0;
		m_pStream->GetNextFrame( &pFrame, 300 );
		if( pFrame )
		{
#if 0
			pFrame->Draw ( hDC, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top );
#else
			m_Render.DrawFrame( hDC, rcClient, pFrame );
#endif
			pFrame->Release();
		}
		else
		{
			FillWithBackgroundColor(hDC, rcClient);
		}
		DrawStreamDescription(hDC, rcClient);
	}
	else
	{
		FillWithBackgroundColor(hDC, rcClient);
	}

	if( m_TitleFont.get() )
	{
		SelectObject( hDC, OldFont );
	}

	EndPaint(&ps);

	return 0;
}