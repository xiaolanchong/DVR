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

//! \brief класс для отображения текста в OpenGL, данные подготавливаются утилитой glfont.exe
//! \version 1.0
//! \date 05-18-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
/*! \code
	
	Пример использования

	GLFont font(FontData, FontDataSize);
	// ..
	GLFont::GLFontRender  FontRender(font);
	font.TextOut("Sample", 0.0f, 0.0f, GLFontColor(1.0f, 1.0f, 1.0f, 1.0f) );
	\endcode
*/
class GLFont
{
public:
	//! структура цвета текста
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
	//! структура данных одного символа
	struct GLFONTCHAR
	{
		float dx, dy;	//!< размер символов
		float tx1, ty1;	//!< текстурные координаты левого верхнего угла символа
		float tx2, ty2;	//!< текстурные координаты правого нижнего угла символа
	} ;

	//! структура параметров шрифта
	struct GLFONT
	{
		int			_unused0;		//!< для совместимости с данными шрифта из файла
		//! ширина текстуры (степень двойки)
		unsigned int TexWidth;		
		//! высота текстуры(степень двойки)
		unsigned int TexHeight;
		//! номер 1го символа в таблице ASCII, учитывая кодировки
		unsigned int IntStart;
		//! номер последнего символа в таблице ASCII
		unsigned int IntEnd;
		int _unused1;		//!< для совместимости с данными шрифта из файла
	} ;

	//! параметры шрифта
	GLFONT					m_FontHdr;
	//! параметры символов
	std::vector<GLFONTCHAR> m_CharDataArr;
	//! текстура шрифта (bind for OpenGL) 
	GLuint					m_OpenGLTextureName;

	//! отрисовать текст
	//! \param String	ASCIIZ строка для вывода
	//! \param x		X-координата левого верхнего угла надписи
	//! \param y		Y-координата левого верхнего угла надписи
	//! \param TextColor	цвет текста
	void	TextOut( const char *String, float x, float y, const GLFont::Color& TextColor ) const;
	//! настроить для вывода текста
	void	PrepareForTextOut() const;
	//! очистить настройки после вывода
	void	ClearAfterTextOut() const;
public:

	//! Creates a glFont from file in memory
	//! \param pData		данные шрифта
	//! \param nDataSize	размер данных
	//! \throw std::runtime_error ошибка создания шрифта
	GLFont( const void* pData, size_t nDataSize );

	//! Creates a glFont 
	//! \param FileName имя файла с данными шрифта (формируется утилитой)
	//! \throw std::runtime_error ошибка создания шрифта
	GLFont(const char *FileName);

	~GLFont();

	//! получить размер символа при рисовании
	//! \param cSym символ
	//! \return		ширина, высота символа
	std::pair<int, int> GetCharSize( unsigned char cSym ) const;


	//! \brief вспомогательный класс для отображения и инициализации
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
			//! отрисовать текст
			//! \param String	ASCIIZ строка для вывода
			//! \param x		X-координата левого верхнего угла надписи
			//! \param y		Y-координата левого верхнего угла надписи
			//! \param TextColor	цвет текста
		void TextOut (const char *String, float x, float y, const GLFont::Color& TextColor) const
		{
			m_Font.TextOut( String, x, y, TextColor );
		}
	};

	friend class GLFont::GLFontRender;
};


//End of file

#endif //__OGLFONT__