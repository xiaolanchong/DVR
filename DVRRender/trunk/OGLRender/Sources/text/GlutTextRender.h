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
#ifndef _GLUT_TEXT_RENDER_9735989088288240_
#define _GLUT_TEXT_RENDER_9735989088288240_

#include "ITextRender.h"

//======================================================================================//
//                                class GlutTextRender                                  //
//======================================================================================//

//! \brief Отображение теста через GLUT - glutBitmapCharacter
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   03.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class GlutTextRender : public ITextRender
{
public:
	GlutTextRender();
	virtual ~GlutTextRender();

	virtual wxSize	GetTextExtent( const char* szText ) ; 
	virtual bool	DrawText(  float x, float y, float width, float height, const char* szText, const wxColour& clText )		;
};

#endif // _GLUT_TEXT_RENDER_9735989088288240_