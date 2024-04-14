//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс отображения 2D-графики - картинки, отрезки, прямоугольники

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_D_V_R_GRAPHICS_WRAPPER_5560703614032809_
#define _I_D_V_R_GRAPHICS_WRAPPER_5560703614032809_

//======================================================================================//
//                              class IDVRGraphicsWrapper                               //
//======================================================================================//

//! \brief Интерфейс отображения 2D-графики - картинки, отрезки, прямоугольники
//!			поддерживаемые примииты
//!				* изображение (8, 24, 32 bpp)
//!				* отрезок - цвет + ширина линии
//!				* прямоугольник - цвет + ширина линии
//!				* закрашенный прямоугольник - цвет заполнения
//!				* текст - выравнивание + шрифт
//!			сетка координат - ось X влево, ось Y вниз, начало координат в левом верхнем углу
//!			все координаты в диапазоне [0, 1]
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.05.2006
//! \version 1.0
//! \bug 
//! \todo 


class IDVRGraphicsWrapper
{
public:
	//Simple vertex structure
	struct Vertex2D
	{
		float x;
		float y;
	};

public:

	//! параметр выравнивания по 8 квадрантам направления + центр
	enum TextAlign_t
	{
		//! левый верхний угол
		TA_NorthWest,
		//! вверх и по центру
		TA_North,
		//! правый верхний
		TA_NorthEast,
		//! право по центру
		TA_East,
		//! нижний правый угол
		TA_SouthEast,
		//! вниз по центру
		TA_South,
		//! левый нижний угол
		TA_SouthWest,
		//! влево по центру
		TA_West,
		//! цент по вертикали и горизонтали
		TA_Center
	};

	//! структура области прямоуг-ка
	struct Rect
	{
		//! X левого верхнего угла
		float x0;
		//! Y левого верхнего угла
		float y0;
		//! X левого 
		float x1;
		float y1;

		Rect( float _x0, float _y0, float _x1, float _y1):
		x0(_x0), y0(_y0), x1(_x1), y1(_y1)
		{
			_ASSERTE( _x1 >= _x0 );
			_ASSERTE( _y1 >= _y0 );
		}
		Rect(){}
	};

	//! атрибуты шрифта
	class IFont
	{
	public:
		//! вес
		enum Weight
		{
			FW_Normal,
			FW_Light,
			FW_Bold
		};

		//! стиль
		enum Style
		{
			FS_Normal,
			FS_Italic
		};
	public:
		virtual ~IFont() = 0 {};
	public:
	};

	//! кисть - для закраски
	class IBrush
	{
	public:
		virtual ~IBrush() = 0 {};
	public:
		virtual void SetColor(const wxColor& color ) = 0;
		virtual wxColor GetColor() = 0;
	};

	//! перо для рисования
	class IPen
	{
	public:
		virtual void SetColor(const wxColor& color ) = 0;
		virtual wxColor GetColor() const = 0;
	public:
		virtual void SetWidth( float width ) = 0;
		virtual float GetWidth() const = 0;
	};

public:
	//--------------- функции создания атрибутных примитивов --------------//

	//! Makes brush (оператор new)
	//! \param color цвет кисти
	//! \return указатель на реализацию 
	virtual IBrush* CreateBrush( const wxColor& color ) = 0;

	//! Makes pen (оператор new)
	//! \param color		цвет пера
	//! \param lineWidth	ширина пера в пикселях
	//! \return				указатель на реализацию
	virtual IPen* CreatePen( const wxColor& color, int lineWidth ) = 0;

	//! Makes font (оператор new)
	//! \param filename имя шрифта (не используется)
	//! \return 
	virtual IFont* CreateFont( const std::string& filename ) = 0;

public:	
	//----------------- функции отрисовки --------------------//

	//! очистить окно 
	//! \param	color	цвет закраски окна
	virtual void Clear( const wxColor& color ) = 0;

	//! нарисовать изображение в прямоугольную область окна
	//! \param	pFont	шрифт, которым рисуется текст
	//! \param	text	текст надписи
	//! \param  x		x-координата левого верхнего угла области отображения
	//! \param  y		y-координата левого верхнего угла области отображения
	//! \param  width	ширина области отображения
	//! \param  height	высота области отображения
	//! \param  bitmapWidth		ширина изображение
	//! \param  bitmapHeight	высота изображение
	//! \param  bpp				bit per pixel
	//! \param  data	данные изображения - bitmapWidth * bitmapHeight * bpp/8 байт без выравнивания, забивки и т.д.
	//! \return			true - успех/false - произошла ошибка
	virtual bool DrawBitmap( const Rect& DrawArea, int bitmapWidth, int bitmapHeight, int bpp, const void* data ) = 0;

	//! нарисовать надпись
	//! \param	pFont	шрифт, которым рисуется текст
	//! \param	text	текст надписи
	//! \param  x0		x-координата левого верхнего угла области отображения
	//! \param  y0		y-координата левого верхнего угла области отображения
	//! \return			true - успех/false - произошла ошибка
	bool DrawText( IFont* pFont, const std::string& text, float x, float y, const wxColour& clText  ) 
	{
		return DrawText( pFont , text, Rect(x, y, x, y), TA_NorthWest, clText );
	}

	//! получить размеры надписи
	//! \param pFont шрифт для отображения, \see CreateFont
	//! \param text  текст надписи
	//! \return		 размер надписи в пикселях
	virtual wxSize GetTextExtent( IFont* pFont, const std::string& text ) = 0;

	//! нарисовать в прямоугольной области окна надпись
	//! \param	pFont	шрифт, которым рисуется текст
	//! \param	text	текст надписи
	//! \param  x0		x-координата левого верхнего угла области
	//! \param  y0		y-координата левого верхнего угла области
	//! \param  x1		x-координата правого нижнего угла области
	//! \param  y1		y-координата правого нижнего угла области
	//! \param  align	параметр выравнивания внутри области
	//! \return			true - успех/false - произошла ошибка
	virtual bool DrawText( IFont* pFont, const std::string& text, const Rect& DrawArea, TextAlign_t align, const wxColour& clText ) = 0;

	//! нарисовать в окне отрезок
	//! \param	pPen	перо, которым рисуется линия (ширина, цвет)
	//! \param  x0		x-координата начальной точки
	//! \param  y0		y-координата начальной точки
	//! \param  x1		x-координата конечной точки
	//! \param  y1		y-координата конечной точки
	//! \return			true - успех/false - произошла ошибка
	virtual bool DrawLine( IPen* pPen, float x0, float y0, float x1, float y1 ) = 0;

	//! нарисовать в окне прямоугольную область пером
	//! \param	pPen	перо, которым рисуются границы, область не заполняется!
	//! \param  x0		x-координата левого верхнего угла области
	//! \param  y0		y-координата левого верхнего угла области
	//! \param  x1		x-координата правого нижнего угла области
	//! \param  y1		y-координата правого нижнего угла области
	//! \return			true - успех/false - произошла ошибка
	virtual bool DrawRect( IPen* pPen, const Rect& DrawArea ) = 0;

	//! заполнить в окне прямоугольную область кистью
	//! \param	pBrush	кисть заполнения (пока только цвет)
	//! \param  x0		x-координата левого верхнего угла области
	//! \param  y0		y-координата левого верхнего угла области
	//! \param  x1		x-координата правого нижнего угла области
	//! \param  y1		y-координата правого нижнего угла области
	//! \return			true - успех/false - произошла ошибка
	virtual bool FillRect( IBrush* pBrush, const Rect& DrawArea ) = 0;

	//! Установить размеры OpenGL viewport в окне, в пикселях
	//! \param width	ширина 
	//! \param height	высота
	virtual void SetViewport( int width, int height ) = 0;

	//! получить размеры OpenGL viewport в пикселях
	//! \param width	ширина
	//! \param height	высота
	virtual void GetViewport( int& width, int& height ) = 0;

public:
	/*
	*	Virtual destructor
	*/
	virtual ~IDVRGraphicsWrapper() = 0 {};
};

#endif // _I_D_V_R_GRAPHICS_WRAPPER_5560703614032809_