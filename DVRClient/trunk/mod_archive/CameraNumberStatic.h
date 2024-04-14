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
#ifndef _CAMERA_NUMBER_STATIC_7054790925435277_
#define _CAMERA_NUMBER_STATIC_7054790925435277_

//======================================================================================//
//                               class CameraNumberStatic                               //
//======================================================================================//

//! \brief Отображение номеров камер для элемента отображения ситуаций
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   08.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class CameraNumberStatic : public wxTextCtrl
{
	int		m_nRenderOffsetInPixels;
public:
	CameraNumberStatic();
	virtual ~CameraNumberStatic();

	void	SetRenderOffset( int nOffset );
private:

	DECLARE_EVENT_TABLE();

	void	OnPaint( wxPaintEvent& ev );
};

#endif // _CAMERA_NUMBER_STATIC_7054790925435277_