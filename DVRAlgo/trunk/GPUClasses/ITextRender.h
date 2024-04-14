//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интферфейс для отображения текста через OpenGL

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   03.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_TEXT_RENDER_2769283695588345_
#define _I_TEXT_RENDER_2769283695588345_

//======================================================================================//
//                                  class ITextRender                                   //
//======================================================================================//

//! \brief Интферфейс для отображения текста через OpenGL
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   03.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class ITextRender
{
public:
	//! just virtual dtor for descendants
	virtual ~ITextRender() {};
	
	//! получить размеры текста в пикселях
	//! \param szText текстовая строка, оканчивающаяся 0
	//! \return размер надписи в пикселях
	virtual SIZE	GetTextExtent( const char* szText ) = 0; 

	//! нарисовать текст на контексте OpenGL  в прямоугольной области
	//! \param x левый верхняя точка, координата Х
	//! \param y левая верхняя точка, координата Y
	//! \param width ширина области
	//! \param height высота области
	//! \param szText ACIIZ строка
	//! \param clText цвет текста
	//! \return успех/неудача
	virtual bool	DrawText( float x, float y, float width, float height, const char* szText, const RGBQUAD& clText )		= 0;
};

#endif // _I_TEXT_RENDER_2769283695588345_