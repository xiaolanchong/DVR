//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Отображение номеров камер для элемента отображения ситуаций

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   08.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "CameraNumberStatic.h"

//======================================================================================//
//                               class CameraNumberStatic                               //
//======================================================================================//

CameraNumberStatic::CameraNumberStatic()
{
}

CameraNumberStatic::~CameraNumberStatic()
{
}

BEGIN_EVENT_TABLE(CameraNumberStatic, wxTextCtrl)
	//EVT_PAINT( CameraNumberStatic::OnPaint )
END_EVENT_TABLE()

void	CameraNumberStatic::OnPaint( wxPaintEvent& ev )
{
	wxSize sz = GetClientSize();
	wxPaintDC dc(this);
	dc.DrawText( "1234567", 0, 0 );
}