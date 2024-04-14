//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Заглушка для рендера, если он не передается, не делает ничего

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   13.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _NULL_RENDER_5813000877593062_
#define _NULL_RENDER_5813000877593062_

#include "../../../../DVRRender/DVRRender.hpp"
#include "../../../../DVRRender/OGLRender/Sources/OGLWrapper.hpp"

class OGLNullWrapper2D /*: public OGLWrapper2D*/
{
public:

	virtual void ResetStates()
	{
	}

	virtual void SetViewport( int width, int height )
	{
	}

	virtual void GetViewport( int& width, int& height )
	{
	}

	virtual void Clear( const OGLWrapper2D::Color& color )
	{
	}

	virtual void DrawText( const std::string& text, float x, float y, const OGLWrapper2D::Color& color, OGLWrapper2D::Font font )
	{
	}

	virtual void DrawText( const std::string& text, float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color )
	{
	}

	virtual void DrawRect( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, float lineWidth  )
	{
	}

	virtual void DrawLine( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& color, float lineWidth )
	{
	}

	virtual void DrawFilledRect( float x0, float y0, float x1, float y1, const OGLWrapper2D::Color& fillColor )
	{
	}

	virtual void DrawBitmap( const OGLWrapper2D::Bitmap& bitmap, float x, float y, float width, float height, bool highQuality  )
	{
	}

	virtual void DrawOval( float x, float y, float width, float height, const OGLWrapper2D::Color& color, int segments , float lineWidth )
	{
	}

	//! very dangerous, vtbl must be the same!
	OGLWrapper2D*	GetWrapper() { return reinterpret_cast<OGLWrapper2D*>(this); }
};

//======================================================================================//
//                                   class NullRender                                   //
//======================================================================================//

//! \brief Заглушка для рендера, если он не передается, не делает ничего
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   13.03.2006
//! \version 1.0
//! \bug 
//! \todo 

class NullRender : public IDVRRender
{
	OGLNullWrapper2D m_Render;
public:
	virtual void Bind( boost::function1<void, IDVRRender*> f )
	{
	}

	/*
	*	Unbind callback function
	*/
	virtual void Unbind()
	{
	}

	/*
	*	Returns OGL2D wrapper 
	*/
	virtual OGLWrapper2D* GetWrapper2D()
	{
		return m_Render.GetWrapper();
	}

	/*
	*	Returns OGL Canvas
	*/
	virtual wxGLCanvas* GetCanvas()
	{
		return NULL;
	}

	/*
	*	Draws and stretch\shrink ( if needed ) bitmap from camera at the specified position
	*/
	virtual void DrawFrame( int cameraId, float x, float y, float width, float height, bool highQuality )
	{
	}
};

class NullArchiveRender : public IDVRArchiveRender
{
	OGLNullWrapper2D m_Render;
public:
	/*
	*	Binds callback function
	*	Callback function will be called every time 
	*	when rendering occurs
	*/
	virtual void Bind( boost::function1<void, IDVRArchiveRender*> f )
	{
	}

	/*
	*	Unbind callback function
	*/
	virtual void Unbind() 
	{
	}

	/*
	*	Draws and stretch\shrink ( if needed ) bitmap from camera at the specified position
	*/
	virtual void DrawFrame( int cameraId, float x, float y, float width, float height, bool highQuality = false )
	{
	}

	/*
	*	Returns OGL2D wrapper 
	*/
	virtual OGLWrapper2D* GetWrapper2D()
	{
		return m_Render.GetWrapper();
	}

	/*
	*	Returns OGL Canvas
	*/
	virtual wxGLCanvas* GetCanvas()
	{
		return NULL;
	}

	/*
	*	Sets camera state
	*	@id: Camera identifier
	*	@state: States: Stop, Start, etc
	*/
	virtual void SetCameraState( int cameraId, State state )
	{
	}

	/*
	*	Gets current camera state
	*/
	virtual IDVRArchiveRender::State GetCameraState( int cameraId )
	{
		return IDVRArchiveRender::CS_Stop;
	}

	/*
	*	Seek specified camera to the position
	*/
	virtual void SeekCamera( int cameraId, INT64 value )
	{
	}
};

#endif // _NULL_RENDER_5813000877593062_