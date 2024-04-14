//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Render chcsva camera images on the client window

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   21.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _I_D_V_R_RENDER_3144377255136610_
#define _I_D_V_R_RENDER_3144377255136610_
//======================================================================================//
//                                  struct IDVRRender                                   //
//======================================================================================//

//! \brief Render chcsva camera images on the client window
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   21.11.2005
//! \version 1.0
//! \bug 
//! \todo 
/*
struct IDVRRender
{
	//! ������������� ������� ������� ����������� ����������� ��� ����� ������ ������ ����
	//! ��� ����� � �������� [0,1]
	//! 0 <= x+w <= 1, 0 <= y+h <= 1
	struct Rect
	{
		float	x, //!< �-���������� (����� ������� ����)
				y, //!< y-���������� (����� ������� ����)
				w, //!< ������
				h; //!< ������
		
		Rect( float _x, float _y, float _w, float _h ):
			x(_x), y(_y), w(_w), h(_h){}
	};

	//! ���������� ��� ����������
	class RenderException : public std::exception{};

	//! ���������� ������� ���� ��� �����������, � ��������
	//! \param Width ������
	//! \param Height ������
	virtual void SetWindowSize( unsigned short Width, unsigned short Height ) = 0;
	//! ����������/������ ������ ������
	//! \param bEnable ��/���
	virtual void Enable( bool bEnable ) = 0;
	//! ���������� ������������ ���� ����������� 
	//! \param WindowArr ������ ��� ������������� ������-������� ���� ������
	virtual void SetLayout( const std::vector<std::pair< int, Rect > >& WindowArr ) = 0;
	//! ���������� ������� ���� ������
	//! \param Index �������� � �������, ����������� � SetLayer, ���� <0, �� ������� ������ ���
	virtual void SetActiveWindow(int Index) = 0;
	//! ���������� ���� ����, ��� - ���, ��� �� ��������� ������ �����������
	//! \param r r-����������
	//! \param g g-����������
	//! \param b b-����������
	virtual void SetBackgroundColor( unsigned char r, unsigned char g, unsigned char b ) = 0;
	//! ���������� ���� ������
	//! \param r r-����������
	//! \param g g-����������
	//! \param b b-����������
	//! \param a a-�����, ���� �� �������������� - ������������
	virtual void SetTextColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) = 0;

	virtual ~IDVRRender() {};
};

*/

class IOGLRender
{
public:
	struct Color
	{
		Color( float red_, float green_, float blue_ ): red( red_ ), green( green_ ), blue( blue_ ){}

		float red;
		float green;
		float blue;
	};

	/*
	*	Clears frame buffer with the specified color
	*/
	virtual void Clear( const Color& color ) = 0;

	/*
	*	Draws text on the canvas at x,y
	*/
	virtual void DrawText( const std::string& text, float x, float y, const Color& color ) = 0;

	/*
	*	Draws rectangle with specified color
	*/
	virtual void DrawRect( float x0, float y0, float x1, float y1, const Color& color ) = 0;

	/*
	*	Draws rectangle and fill it with the specified color
	*/
	virtual void DrawFilledRect( float x0, float y0, float x1, float y1, const Color& fillColor ) = 0;

	/*
	*	Draws and stretch\shrink ( if needed ) bitmap at the specified position 
	*/
	virtual void DrawBitmap( int nCameraID, float x, float y, float width, float height, bool highQuality = false ) = 0;

	/* 
	*	Draws an oval centered at (x,y)
	*	The oval is bound inside a rectangle with specified width and height
	*	segments represents the number of line segments used to draw the oval.
	*/
	virtual void DrawOval( float x, float y, float width, float height, const Color& color, int segments = 25 ) = 0;	
};

//! ��������� ������
class IOGLArchiveRender : public IOGLRender
{
public:
	//! ����������� �� ����� 
	//! \param time - ��� �������� � �������� �����, ������ ������ ���-�� ������ � 1970 ���� 
	virtual void SeekTo(INT64 time) = 0;
};

class IDVRRenderStub
{
public:
	virtual void Bind( boost::function1<void, IOGLRender*> f ) = 0;

	/*
	*	Unbind callback function
	*/
	virtual void Unbind() = 0;

	//! set camera ids
	//! \param CameraIDArr 
	virtual void SetCameras( const std::vector<int>& CameraIDArr ) = 0;

	virtual wxWindow* GetWindow() = 0;
};



IDVRRenderStub*	CreateRenderStub( wxWindow* pParent  );

#endif // _I_D_V_R_RENDER_3144377255136610_