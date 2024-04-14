//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ��������� ����������� 2D-������� - ��������, �������, ��������������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   23.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_D_V_R_GRAPHICS_WRAPPER_5560703614032809_
#define _I_D_V_R_GRAPHICS_WRAPPER_5560703614032809_

//======================================================================================//
//                              class IDVRGraphicsWrapper                               //
//======================================================================================//

//! \brief ��������� ����������� 2D-������� - ��������, �������, ��������������
//!			�������������� ��������
//!				* ����������� (8, 24, 32 bpp)
//!				* ������� - ���� + ������ �����
//!				* ������������� - ���� + ������ �����
//!				* ����������� ������������� - ���� ����������
//!				* ����� - ������������ + �����
//!			����� ��������� - ��� X �����, ��� Y ����, ������ ��������� � ����� ������� ����
//!			��� ���������� � ��������� [0, 1]
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

	//! �������� ������������ �� 8 ���������� ����������� + �����
	enum TextAlign_t
	{
		//! ����� ������� ����
		TA_NorthWest,
		//! ����� � �� ������
		TA_North,
		//! ������ �������
		TA_NorthEast,
		//! ����� �� ������
		TA_East,
		//! ������ ������ ����
		TA_SouthEast,
		//! ���� �� ������
		TA_South,
		//! ����� ������ ����
		TA_SouthWest,
		//! ����� �� ������
		TA_West,
		//! ���� �� ��������� � �����������
		TA_Center
	};

	//! ��������� ������� �������-��
	struct Rect
	{
		//! X ������ �������� ����
		float x0;
		//! Y ������ �������� ����
		float y0;
		//! X ������ 
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

	//! �������� ������
	class IFont
	{
	public:
		//! ���
		enum Weight
		{
			FW_Normal,
			FW_Light,
			FW_Bold
		};

		//! �����
		enum Style
		{
			FS_Normal,
			FS_Italic
		};
	public:
		virtual ~IFont() = 0 {};
	public:
	};

	//! ����� - ��� ��������
	class IBrush
	{
	public:
		virtual ~IBrush() = 0 {};
	public:
		virtual void SetColor(const wxColor& color ) = 0;
		virtual wxColor GetColor() = 0;
	};

	//! ���� ��� ���������
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
	//--------------- ������� �������� ���������� ���������� --------------//

	//! Makes brush (�������� new)
	//! \param color ���� �����
	//! \return ��������� �� ���������� 
	virtual IBrush* CreateBrush( const wxColor& color ) = 0;

	//! Makes pen (�������� new)
	//! \param color		���� ����
	//! \param lineWidth	������ ���� � ��������
	//! \return				��������� �� ����������
	virtual IPen* CreatePen( const wxColor& color, int lineWidth ) = 0;

	//! Makes font (�������� new)
	//! \param filename ��� ������ (�� ������������)
	//! \return 
	virtual IFont* CreateFont( const std::string& filename ) = 0;

public:	
	//----------------- ������� ��������� --------------------//

	//! �������� ���� 
	//! \param	color	���� �������� ����
	virtual void Clear( const wxColor& color ) = 0;

	//! ���������� ����������� � ������������� ������� ����
	//! \param	pFont	�����, ������� �������� �����
	//! \param	text	����� �������
	//! \param  x		x-���������� ������ �������� ���� ������� �����������
	//! \param  y		y-���������� ������ �������� ���� ������� �����������
	//! \param  width	������ ������� �����������
	//! \param  height	������ ������� �����������
	//! \param  bitmapWidth		������ �����������
	//! \param  bitmapHeight	������ �����������
	//! \param  bpp				bit per pixel
	//! \param  data	������ ����������� - bitmapWidth * bitmapHeight * bpp/8 ���� ��� ������������, ������� � �.�.
	//! \return			true - �����/false - ��������� ������
	virtual bool DrawBitmap( const Rect& DrawArea, int bitmapWidth, int bitmapHeight, int bpp, const void* data ) = 0;

	//! ���������� �������
	//! \param	pFont	�����, ������� �������� �����
	//! \param	text	����� �������
	//! \param  x0		x-���������� ������ �������� ���� ������� �����������
	//! \param  y0		y-���������� ������ �������� ���� ������� �����������
	//! \return			true - �����/false - ��������� ������
	bool DrawText( IFont* pFont, const std::string& text, float x, float y, const wxColour& clText  ) 
	{
		return DrawText( pFont , text, Rect(x, y, x, y), TA_NorthWest, clText );
	}

	//! �������� ������� �������
	//! \param pFont ����� ��� �����������, \see CreateFont
	//! \param text  ����� �������
	//! \return		 ������ ������� � ��������
	virtual wxSize GetTextExtent( IFont* pFont, const std::string& text ) = 0;

	//! ���������� � ������������� ������� ���� �������
	//! \param	pFont	�����, ������� �������� �����
	//! \param	text	����� �������
	//! \param  x0		x-���������� ������ �������� ���� �������
	//! \param  y0		y-���������� ������ �������� ���� �������
	//! \param  x1		x-���������� ������� ������� ���� �������
	//! \param  y1		y-���������� ������� ������� ���� �������
	//! \param  align	�������� ������������ ������ �������
	//! \return			true - �����/false - ��������� ������
	virtual bool DrawText( IFont* pFont, const std::string& text, const Rect& DrawArea, TextAlign_t align, const wxColour& clText ) = 0;

	//! ���������� � ���� �������
	//! \param	pPen	����, ������� �������� ����� (������, ����)
	//! \param  x0		x-���������� ��������� �����
	//! \param  y0		y-���������� ��������� �����
	//! \param  x1		x-���������� �������� �����
	//! \param  y1		y-���������� �������� �����
	//! \return			true - �����/false - ��������� ������
	virtual bool DrawLine( IPen* pPen, float x0, float y0, float x1, float y1 ) = 0;

	//! ���������� � ���� ������������� ������� �����
	//! \param	pPen	����, ������� �������� �������, ������� �� �����������!
	//! \param  x0		x-���������� ������ �������� ���� �������
	//! \param  y0		y-���������� ������ �������� ���� �������
	//! \param  x1		x-���������� ������� ������� ���� �������
	//! \param  y1		y-���������� ������� ������� ���� �������
	//! \return			true - �����/false - ��������� ������
	virtual bool DrawRect( IPen* pPen, const Rect& DrawArea ) = 0;

	//! ��������� � ���� ������������� ������� ������
	//! \param	pBrush	����� ���������� (���� ������ ����)
	//! \param  x0		x-���������� ������ �������� ���� �������
	//! \param  y0		y-���������� ������ �������� ���� �������
	//! \param  x1		x-���������� ������� ������� ���� �������
	//! \param  y1		y-���������� ������� ������� ���� �������
	//! \return			true - �����/false - ��������� ������
	virtual bool FillRect( IBrush* pBrush, const Rect& DrawArea ) = 0;

	//! ���������� ������� OpenGL viewport � ����, � ��������
	//! \param width	������ 
	//! \param height	������
	virtual void SetViewport( int width, int height ) = 0;

	//! �������� ������� OpenGL viewport � ��������
	//! \param width	������
	//! \param height	������
	virtual void GetViewport( int& width, int& height ) = 0;

public:
	/*
	*	Virtual destructor
	*/
	virtual ~IDVRGraphicsWrapper() = 0 {};
};

#endif // _I_D_V_R_GRAPHICS_WRAPPER_5560703614032809_