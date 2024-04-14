//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ����������� ������ ����� GLFONT ���������� (����� � ��������)

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   03.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _O_G_L_FONT_TEXT_RENDER_8123002223496473_
#define _O_G_L_FONT_TEXT_RENDER_8123002223496473_

#include "ITextRender.h"
class GLFont;

//======================================================================================//
//                               class OGLFontTextRender                                //
//======================================================================================//

//! \brief ����������� ������ ����� GLFONT ���������� (����� � ��������)
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   03.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class OGLFontTextRender : public ITextRender
{
	//! ��������� ������
	std::auto_ptr<GLFont>			m_Font;		
public:
	//! \exception std::runtime_error - �� ������� ������� ����� �� ���������
	OGLFontTextRender();
	virtual ~OGLFontTextRender();

	virtual wxSize	GetTextExtent( const char* szText ) ; 

	virtual bool	DrawText( float x, float y, float width, float height, const char* szText, const wxColour& clText );
};

#endif // _O_G_L_FONT_TEXT_RENDER_8123002223496473_