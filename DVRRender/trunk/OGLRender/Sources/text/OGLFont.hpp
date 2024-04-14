/*
*	DVR 
*	Copyright(C) 2006 Elvees
*	All rights reserved.
*
*	$Filename: DVRRender.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-04-25
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: OGL font implementation
*
*
*/

#ifndef __OGLFONT__
#define __OGLFONT__

//*********************************************************
//Structures
//*********************************************************

//! \brief ����� ��� ����������� ������ � OpenGL, ������ ���������������� �������� glfont.exe
//! \version 1.0
//! \date 05-18-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
/*! \code
	
	������ �������������

	GLFont font(FontData, FontDataSize);
	// ..
	GLFont::GLFontRender  FontRender(font);
	font.TextOut("Sample", 0.0f, 0.0f, GLFontColor(1.0f, 1.0f, 1.0f, 1.0f) );
	\endcode
*/
class GLFont
{
public:
	//! ��������� ����� ������
	struct Color
	{
		//! Red, green, blue, alpha value
		float r, g, b, a;
		Color( float _r, float _g, float _b, float _a ):
		r( _r ), g(_g), b(_b), a(_a)
		{
		}
	};
private:	
	//! ��������� ������ ������ �������
	struct GLFONTCHAR
	{
		float dx, dy;	//!< ������ ��������
		float tx1, ty1;	//!< ���������� ���������� ������ �������� ���� �������
		float tx2, ty2;	//!< ���������� ���������� ������� ������� ���� �������
	} ;

	//! ��������� ���������� ������
	struct GLFONT
	{
		int			_unused0;		//!< ��� ������������� � ������� ������ �� �����
		//! ������ �������� (������� ������)
		unsigned int TexWidth;		
		//! ������ ��������(������� ������)
		unsigned int TexHeight;
		//! ����� 1�� ������� � ������� ASCII, �������� ���������
		unsigned int IntStart;
		//! ����� ���������� ������� � ������� ASCII
		unsigned int IntEnd;
		int _unused1;		//!< ��� ������������� � ������� ������ �� �����
	} ;

	//! ��������� ������
	GLFONT					m_FontHdr;
	//! ��������� ��������
	std::vector<GLFONTCHAR> m_CharDataArr;
	//! �������� ������ (bind for OpenGL) 
	GLuint					m_OpenGLTextureName;

	//! ���������� �����
	//! \param String	ASCIIZ ������ ��� ������
	//! \param x		X-���������� ������ �������� ���� �������
	//! \param y		Y-���������� ������ �������� ���� �������
	//! \param TextColor	���� ������
	void	TextOut( const char *String, float x, float y, const GLFont::Color& TextColor ) const;
	//! ��������� ��� ������ ������
	void	PrepareForTextOut() const;
	//! �������� ��������� ����� ������
	void	ClearAfterTextOut() const;
public:

	//! Creates a glFont from file in memory
	//! \param pData		������ ������
	//! \param nDataSize	������ ������
	//! \throw std::runtime_error ������ �������� ������
	GLFont( const void* pData, size_t nDataSize );

	//! Creates a glFont 
	//! \param FileName ��� ����� � ������� ������ (����������� ��������)
	//! \throw std::runtime_error ������ �������� ������
	GLFont(const char *FileName);

	~GLFont();

	//! �������� ������ ������� ��� ���������
	//! \param cSym ������
	//! \return		������, ������ �������
	std::pair<int, int> GetCharSize( unsigned char cSym ) const;


	//! \brief ��������������� ����� ��� ����������� � �������������
	//! \version 1.0
	//! \date 05-18-2006
	//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
	//! \todo 
	//! \bug 
	//!
	class GLFontRender
	{
		//d class GLFont;
		const GLFont&	m_Font;
	public:
		GLFont::GLFontRender( const GLFont& font ):
			 m_Font( font )
		  {
			  m_Font.PrepareForTextOut();
		  }
		~GLFontRender()
		{
			m_Font.ClearAfterTextOut();
		}
			//! ���������� �����
			//! \param String	ASCIIZ ������ ��� ������
			//! \param x		X-���������� ������ �������� ���� �������
			//! \param y		Y-���������� ������ �������� ���� �������
			//! \param TextColor	���� ������
		void TextOut (const char *String, float x, float y, const GLFont::Color& TextColor) const
		{
			m_Font.TextOut( String, x, y, TextColor );
		}
	};

	friend class GLFont::GLFontRender;
};


//End of file

#endif //__OGLFONT__