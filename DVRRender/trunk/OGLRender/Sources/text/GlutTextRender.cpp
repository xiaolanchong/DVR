//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Отображение теста через GLUT - glutBitmapCharacter

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   03.05.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "GlutTextRender.h"
#include "gl/glut.h"

//======================================================================================//
//                                class GlutTextRender                                  //
//======================================================================================//

GlutTextRender::GlutTextRender()
{
}

GlutTextRender::~GlutTextRender()
{
}

void* fontId = (void*)GLUT_BITMAP_9_BY_15;

wxSize	GlutTextRender::GetTextExtent( const char* szText )
{
	wxSize sizeText(0, 15);
	for(; *szText ; ++szText )
	{
		sizeText.x += glutBitmapWidth( fontId, *szText );
	}
	return sizeText;
}

bool	GlutTextRender::DrawText(  float x, float y, float width, float height, const char* szText, const wxColour& clText )
{
	if(!szText) return true;
	//Set new color
	glColor3ub( clText.Red(), clText.Green(), clText.Blue() );

	//Set raster position
	// начало в левой нижней точке
	glRasterPos2f( x, y + height);

	//Draw characters
//	std::string::const_iterator it;
//	for( it = text.begin(); it != text.end(); ++it )
	for(; *szText ; ++szText )
	{
		glutBitmapCharacter( fontId, *szText );
	}

	//Set raster position
	glRasterPos2f( 0.0f, 0.0f );
	return true;
}