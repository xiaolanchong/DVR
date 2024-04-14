/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: OGLWrapper2D.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-01-16
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Simple OpenGL wrapper
*
*
*/

#ifndef __OGLWRAPPER_HPP__
#define __OGLWRAPPER_HPP__

class IDVRGraphicsWrapper;
class IDVRGraphicsWrapper::IBrush;
class IDVRGraphicsWrapper::IPen;
class IDVRGraphicsWrapper::IFont;

#include "text/ITextRender.h"
#include "bitmap/IBitmapRender.h"

class OGLGraphicsWrapper : public IDVRGraphicsWrapper
{
public:
	enum SysBitmapType
	{
		SBT_CAM_NOT_AVAILABLE = 0,
		SBT_CAM_INVALID_BIT_DEPTH,
		SBT_UNKOWN_ERROR
	};

public:
	class Font  : public IDVRGraphicsWrapper::IFont
	{
		boost::shared_ptr<ITextRender> m_pTextRender;
	public:
		Font( boost::shared_ptr<ITextRender> pTextRender):
		  m_pTextRender(pTextRender)
		  {
		  }

		 wxSize GetTextExtent( const std::string& text )
		 {
			 return m_pTextRender->GetTextExtent( text.c_str() );
		 }

		 bool DrawText( float x, float y, float width, float height, const std::string& text, const wxColour& clText )
		 {
		//	 glLogicOp( GL_AND_REVERSE );
		//	 glEnable( GL_COLOR_LOGIC_OP );
			 bool res = m_pTextRender->DrawText( x, y, width, height, text.c_str(), clText );
		//	 glDisable( GL_COLOR_LOGIC_OP );
			 return res;
		 }
	};
	class Brush : public IDVRGraphicsWrapper::IBrush
	{
	public:
		Brush();
		Brush( wxColor color );
		virtual ~Brush();
	public:
		void SetColor( const wxColor& color );
		wxColor GetColor();
	public:
		wxColor mColor;
	};

	class Pen : public IDVRGraphicsWrapper::IPen
	{
	public:
		Pen();
		Pen( wxColor color, int width );
		virtual ~Pen();
	public:
		void SetColor(const wxColor& color );
		wxColor GetColor() const;
	public:
		void SetWidth( float width );
		float GetWidth() const;
	public:
//		void MakeCurrent();
	private:
		float mWidth;
		wxColor mColor;
	};

public:

	void SetPen( IDVRGraphicsWrapper::IPen* pen );

	/*
	*	Makes brush
	*/
	IDVRGraphicsWrapper::IBrush* CreateBrush(const wxColor& color );

	/*
	*	Makes pen
	*/
	IDVRGraphicsWrapper::IPen* CreatePen(const wxColor& color, int lineWidth );
	
	/*
	*	Makes font
	*/
	IDVRGraphicsWrapper::IFont* CreateFont( const std::string& filename );

public:
	/*
	*	Clears viewport to the specified color.
	*/
	void Clear( const wxColor& color );

	/*
	*	Draws bitmap with the specified position and width
	*/
	bool DrawBitmap( const Rect& DrawArea, int bitmapWidth, int bitmapHeight, int bpp, const void* data );

	wxSize GetTextExtent( IFont* pFont, const std::string& text );

	/*
	*	Draws text inside ( x0,y0 - x1,y1 ) rectangle using currently selected font 
	*/
	bool DrawText( IFont* pFont, const std::string& text, const Rect& DrawArea, TextAlign_t align, const wxColour& clText );

	/*
	*	Draws line using current pen
	*/
	bool DrawLine( IPen* pPen, float x0, float y0, float x1, float y1 );

	/*
	*	Draws rectangle using current pen and brush
	*/
	bool DrawRect( IPen* pPen, const Rect& DrawArea );

	bool FillRect( IBrush* pBrush, const Rect& DrawArea );

public:
//	void ResetStates();
	/*
	*	Sets OpenGL viewport dimensions
	*/
	void SetViewport( int width, int height );

	/*
	*	Gets current OpenGL viewport dimensions
	*/
	void GetViewport( int& width, int& height );
public:
	void TestGraphics();

public:
	OGLGraphicsWrapper();
	virtual ~OGLGraphicsWrapper();
private:
	
	//! отображает текст
	boost::shared_ptr<ITextRender>		m_pTextRender;
	//! отображает картинки
	boost::shared_ptr<IBitmapRender>	m_pBitmapRender;
};

#endif //__OGLWRAPPER_HPP__
