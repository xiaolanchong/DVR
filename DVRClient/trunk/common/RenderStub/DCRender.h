//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Заглушка для IDVRRenderManager - рисование на контексте окна

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   21.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _D_C_RENDER_1750195992905986_
#define _D_C_RENDER_1750195992905986_

//! заглушка-рендер, не делает ничего, мы не можем рисовать пока не в ф-и OnPaint
//! ф-и аналогичны OGLWrapper2D
class DCWrapperBase2D : public IDVRGraphicsWrapper
{
	virtual void ResetStates() {};
	virtual void SetViewport( int width, int height ) {};
	virtual void GetViewport( int& width, int& height ) {};
public:
/*	virtual void Clear( const OGLWrapper2D::Color& color ) {}
	virtual void DrawText( const std::string& text, float x, float y, const OGLWrapper2D::Color& color,  OGLWrapper2D::Font font ){}
	virtual void DrawText( const std::string& text, float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, OGLWrapper2D::Font font){}
	virtual void DrawRect( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, float lineWidth  ){}
	virtual void DrawLine( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, float lineWidth ){}
	virtual void DrawFilledRect( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& fillColor ){}
	virtual void DrawBitmap( const OGLWrapper2D::Bitmap& bitmap, float x, float y, float width, float height, bool highQuality){}
	virtual void DrawOval( flat x, float y, float width, float height, const OGLWrapper2D::Color& color, int segments , float lineWidth){}
*/
	virtual void SetBrush( IBrush* brush ) = 0;
	virtual IBrush* GetBrush() const = 0; 
	virtual void SetPen( IPen* pen ) = 0;
	virtual IPen* GetPen() const = 0;
	virtual void SetFont( IFont* font ) = 0;
	virtual IFont* GetFont() const = 0;
	virtual IBrush* CreateBrush( wxColor& color ) = 0;
	virtual IPen* CreatePen( wxColor& color, int lineWidth ) = 0;
	virtual IFont* CreateFont( const std::string& filename ) = 0;

public:
	virtual void Clear( const wxColor& color );
	virtual void DrawBitmap( float x, float y, float width, float height, int bitmapWidth, int bitmapHeight, int bpp, const void* data );
	virtual void DrawText( const std::string& text, float x, float y  ) ;
	virtual void DrawText( const std::string& text, float x0, float y0, float x1, float y1, TextAlign align ) ;
	virtual void DrawLine( float x0, float y0, float x1, float y1 )
	virtual void DrawRect( float x0, float y0, float x1, float y1 )

	virtual void DrawOval( float x0, float y0, float x1, float y1 )
	{
	}
	virtual void DrawSpline( size_t pointsNum, Vertex2D* vertices ) 
	{
	}
	virtual void DrawPolygon( size_t pointsNum, Vertex2D* vertices ) 
	{
	}
	virtual void SetViewport( int width, int height )
	{
	}
	virtual void GetViewport( int& width, int& height )
	{
	}
};

//! рисует в контекст и окно вывода с размерами
class DCWrapper2D : public DCWrapperBase2D
{
	wxDC*		m_pDC;
	wxSize		m_sizeContext;

	wxPoint		ToPoint(float x, float y) const
	{
		return wxPoint(m_sizeContext.GetWidth() * x, m_sizeContext.GetHeight() * y );
	}
	wxColour	ToColor( const OGLWrapper2D::Color & cl)
	{
		return wxColour( cl.red * 255, cl.green * 255, cl.blue * 255 );
	}
public:
	DCWrapper2D(wxDC* pDC, wxSize sizeContext);

private:
	virtual void Clear( const OGLWrapper2D::Color& color );
	virtual void DrawText( const std::string& text, float x, float y, const OGLWrapper2D::Color& color,  OGLWrapper2D::Font font );
	virtual void DrawText( const std::string& text, float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, OGLWrapper2D::Font font);
	virtual void DrawRect( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, float lineWidth  );
	virtual void DrawLine( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, float lineWidth );
	virtual void DrawFilledRect( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& fillColor );
	virtual void DrawBitmap( const OGLWrapper2D::Bitmap& bitmap, float x, float y, float width, float height, bool highQuality);
	virtual void DrawOval( float x, float y, float width, float height, const OGLWrapper2D::Color& color, int segments , float lineWidth);
};

class DVRStreamRenderStub : public IDVRStreamRender
{
	std::auto_ptr<DCWrapperBase2D>&	m_pRender;
	virtual Result DrawFrame( int cameraId, float x, float y, float width, float height, bool highQuality );
	virtual void SetReleaseTimeout( int timeout ){}
	virtual int GetReleaseTimeout() { return 0; }
public:
	DVRStreamRenderStub(std::auto_ptr<DCWrapperBase2D>&	pRender): 
	  m_pRender(pRender){}
};

class DVRArchiveRenderStub : public IDVRArchiveRender
{
	virtual Result DrawFrame( int cameraId, float x, float y, float width, float height, bool highQuality );

	virtual void SetCameraState( int cameraId, State state )
	{
	}
	virtual State GetCameraState( int cameraId )
	{
		return CS_Stop;
	}
	virtual void SeekCamera( INT64 value )
	{
	}

	std::auto_ptr<DCWrapperBase2D>&	m_pRender;

	virtual void SetReleaseTimeout( int timeout ){}
	virtual int GetReleaseTimeout() { return 0; }

	/*
	*	Returns current archive position
	*/
	virtual INT64 GetPosition() const 
	{
		return 0;
	}

	/*
	*	Sets/Gets archive interval( start )
	*/
	virtual void SetArchiveStart( INT64 time )
	{

	}
	virtual INT64 GetArchiveStart() const
	{
		return 0;
	}

	/*
	*	Sets/Gets archive interval ( end )
	*/
	virtual void SetArchiveEnd( INT64 time ) 
	{

	}
	virtual INT64 GetArchiveEnd() const
	{
		return 0;
	}
public:
	DVRArchiveRenderStub(std::auto_ptr<DCWrapperBase2D>&	pRender): 
	  m_pRender(pRender){}
};

//======================================================================================//
//                                    class DCRender                                    //
//======================================================================================//

//! \brief Заглушка для IDVRRenderManager - рисование на контексте окна
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   21.03.2006
//! \version 1.0
//! \bug 
//! \todo 

class DCRender : private wxWindow, public IDVRRenderManager
{
	boost::function1<void, IDVRStreamRender*> m_StreamFunc;
	boost::function1<void, IDVRArchiveRender*> m_ArchiveFunc;
	std::auto_ptr<DCWrapperBase2D>		m_pRender;
	mutable DVRStreamRenderStub			m_StreamRenderStub;
	mutable DVRArchiveRenderStub		m_ArchiveRenderStub;
	RenderMode							m_RenderMode;
public:
	DCRender( wxWindow* pParentWnd );
	virtual ~DCRender();

	virtual void SetRenderMode( RenderMode mode )
	{
		m_RenderMode = mode;
	}
	virtual RenderMode GetRenderMode() const
	{
		return m_RenderMode;
	}
	virtual IDVRRender* GetRender( RenderMode mode ) const; 
	virtual OGLWrapper2D* GetWrapper2D() 
	{
		return reinterpret_cast<OGLWrapper2D*>(m_pRender.get());
	}
	virtual wxGLCanvas* GetCanvas()
	{
		return reinterpret_cast<wxGLCanvas*>( static_cast<wxWindow*>(this) );
	}

	virtual void BindStreamCallback( boost::function1<void, IDVRStreamRender*> f )
	{
		m_StreamFunc = f;
	}
	virtual void BindArchiveCallback( boost::function1<void, IDVRArchiveRender*> f )
	{
		m_ArchiveFunc = f;
	}

	virtual void UnbindStreamCallback()
	{
		m_StreamFunc.clear();
	}
	virtual void UnbindArchiveCallback()
	{
		m_ArchiveFunc.clear();
	}

private:
	DECLARE_EVENT_TABLE();

	void OnPaint(wxPaintEvent& ev);
};

#endif // _D_C_RENDER_1750195992905986_