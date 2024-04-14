//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Отображение текста через GLFONT библиотеку (шрифт в текстуре)

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   03.05.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "OGLFontTextRender.h"
#include "OGLFont.hpp"

//======================================================================================//
//                               class OGLFontTextRender                                //
//======================================================================================//

extern unsigned char	bin_data[];
extern size_t			bin_data_size;

OGLFontTextRender::OGLFontTextRender():
	m_Font( new  GLFont( bin_data, bin_data_size ) )
{

}

OGLFontTextRender::~OGLFontTextRender()
{
}

wxSize	OGLFontTextRender::GetTextExtent( const char* szText )
{
	wxSize sizeText(0, 0);
	//! для ширины прибавим ширину каждой буквы, для высоты - максимум высоты среди букв
	for ( ; szText && *szText; ++szText )
	{
		std::pair<int, int> CharSize = m_Font->GetCharSize( *szText );
		sizeText.x += CharSize.first;
		sizeText.y = std::max( sizeText.y, CharSize.second );
	}
	_ASSERTE( sizeText.GetX() >= 0 && sizeText.GetY() >= 0 );
	return sizeText;
}

bool	OGLFontTextRender::DrawText( float x, float y, float width, float height, const char* szText, const wxColour& clText )
{
	_ASSERTE( width  >= 0.0f );
	_ASSERTE( height >= 0.0f );
	
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	glTranslatef( x, y/* - height*/, 0.0f );

	GLFont::GLFontRender Render( * m_Font.get() );
#if 1
	//! установим параметры растяжения, так чтобы соблюсти примерную высоту и ширину текста без искажений
	int values[4];
	glGetIntegerv( GL_VIEWPORT, &values[0] );

	const int ViewportWidth	= values[2];
	const int ViewPortHeight	= values[3];

	const int c_AverageTextHeightInPx = 8; 
	const float fAspectRatioY = float(c_AverageTextHeightInPx)/ViewPortHeight; 
	const float fAspectRatioX = float(ViewPortHeight)/ViewportWidth * fAspectRatioY;
	glScalef( fAspectRatioX, -fAspectRatioY, 1.0f );

	Render.TextOut( szText, 0.0f, 0.0f, GLFont::Color( clText.Red()/255, clText.Green()/255, clText.Blue(), 1.0 ) );
#else
	glBegin(GL_QUADS);

	const float z = 0.0f;
		//Specify vertices and texture coordinates
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(x, y, z);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(x, y + 0.25, z);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(x + 0.25, y + 0.25, z);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f(x + 0.25, y, z);

	//Stop rendering quads
	glEnd();
#endif

	glPopMatrix();

	return true;
}