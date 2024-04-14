//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���������� ��� ����������� ������ ����� OpenGL

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   03.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_TEXT_RENDER_2769283695588345_
#define _I_TEXT_RENDER_2769283695588345_

//======================================================================================//
//                                  class ITextRender                                   //
//======================================================================================//

//! \brief ���������� ��� ����������� ������ ����� OpenGL
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
	
	//! �������� ������� ������ � ��������
	//! \param szText ��������� ������, �������������� 0
	//! \return ������ ������� � ��������
	virtual SIZE	GetTextExtent( const char* szText ) = 0; 

	//! ���������� ����� �� ��������� OpenGL  � ������������� �������
	//! \param x ����� ������� �����, ���������� �
	//! \param y ����� ������� �����, ���������� Y
	//! \param width ������ �������
	//! \param height ������ �������
	//! \param szText ACIIZ ������
	//! \param clText ���� ������
	//! \return �����/�������
	virtual bool	DrawText( float x, float y, float width, float height, const char* szText, const RGBQUAD& clText )		= 0;
};

#endif // _I_TEXT_RENDER_2769283695588345_