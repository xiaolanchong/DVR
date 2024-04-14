#include "stdafx.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <glh/glh_extensions.h>

#include "ConvertYUVRGB/convert_a.h"
#include "FastTimer.h"

#include "CgTestWindow.h"


const GLenum targetTexture = GL_TEXTURE_RECTANGLE_NV;
const int maxWidth = 352;
const int maxHeight = 288;
const int downScale = 2;

//GL_EXT_framebuffer_object
CgTestWindow::CgTestWindow( CWnd* parent, boost::shared_ptr<Elvees::ICameraGrabber> cameraGrabber,
boost::shared_ptr< GPUTestParameters > parameters ) 
	: OneThreadWindow( parent ), m_fpsLogger( 1 ), m_first( true ), 
	m_pGrabber( cameraGrabber ), m_testParameters( parameters ),
	m_frameExtractor( maxWidth / downScale, maxHeight / downScale, 110, 64, downScale )
{
}


void CgTestWindow::DoStuff() 
{
	m_fpsLogger.IncrementFrame();

	BITMAPINFOHEADER header =	m_pGrabber->GetHeader();
	const ubyte * pBytes = 0;
	ulong time;
	m_pGrabber->Read( pBytes, time );

	//CopyYUY2ToVector( &header, pBytes, m_currentFrame, true );
	mmx_YUY2toRGB24(pBytes, &m_currentFrame[0], pBytes + header.biSizeImage, header.biWidth * header.biBitCount / 8 , header.biWidth * header.biBitCount / 8, 0 );

	if( m_first )
	{
		m_frameBufferA->WriteBytes( &m_currentFrame[0], GLTexture::df_rgb );
		m_frameBufferB->WriteBytes( &m_currentFrame[0], GLTexture::df_rgb );
		m_first = false;
	}

	if ( m_testParameters->runShaders )
	{
		m_currentFrameTexture->WriteBytes( &m_currentFrame[0], GLTexture::df_rgb );

		//	m_programBlur->SetTextureParameter( "videoFrame", *m_currentFrameTexture );
		//	m_programBlur->SetOutputBuffer( *m_blurredFrame );
		//	m_programBlur->RunProgram();	

		if( !m_BufferA )
			m_programBgAccumulate->SetOutputBuffer( *m_frameBufferA );
		else
			m_programBgAccumulate->SetOutputBuffer( *m_frameBufferB );

		m_programBgAccumulate->SetTextureParameter( "videoFrame", *m_currentFrameTexture );

		if( m_BufferA )
			m_programBgAccumulate->SetTextureParameter( "background", *m_frameBufferA );
		else
			m_programBgAccumulate->SetTextureParameter( "background", *m_frameBufferB );
		m_programBgAccumulate->SetTextureParameter( "motionMap", *m_motionMap );

		m_programBgAccumulate->RunProgram();

		m_programMotionDetection->SetTextureParameter( "background", *m_frameBufferA );
		m_programMotionDetection->SetTextureParameter( "videoFrame", *m_currentFrameTexture );
		m_programMotionDetection->SetOutputBuffer( *m_motionMap );
		m_programMotionDetection->RunProgram();

		m_smallMotionMap->CopyFrom( *m_motionMap );

		if( m_testParameters->readBack )
		{
			m_smallMotionMap->ReadBytes( &m_currentResult[0], GLTexture::df_red );
			m_frameExtractor.GetFrames( &m_currentResult[0], m_currentFrames );	
		}

		//OutputDebugStringA( ( boost::format( "%d \n" ) % m_currentFrames.size() ).str().c_str() );

		RenderTexture( m_currentFrameTexture->ID(), m_currentFrameTexture->Width(), m_currentFrameTexture->Height() );
		//RenderTexture( m_motionMap->TextureID(), m_motionMap->Width(), m_motionMap->Height() );
		//RenderTexture( m_blurredFrame->TextureID(), m_blurredFrame->Width(), m_blurredFrame->Height() );
		//RenderTexture( m_smallMotionMap->TextureID(), m_smallMotionMap->Width(), m_smallMotionMap->Height() );
		//RenderBytes(  &m_currentResult[0] );

		m_BufferA = !m_BufferA;
	}
}

void CgTestWindow::ThreadInit()
{
	m_textRender = std::auto_ptr< WindowsFontBitmapTextRender>( new WindowsFontBitmapTextRender( "Arial", 13 ) );	
	//////////////////////////////////////////////////////////////////////////
//	InitCameraGrabber();

	InitPixelShader();
	//////////////////////////////////////////////////////////////////////////
	m_currentFrame.resize( maxWidth * maxHeight * 3, 128 );
	m_currentResult.resize( maxWidth * maxHeight / ( downScale * downScale ), 128 );

	// Frame Buffer A
	m_frameBufferA = std::auto_ptr<GLFrameBuffer>( new GLFrameBuffer( maxWidth, maxHeight, GLTexture::tf_rgba ) );
	// Frame Buffer B
	m_frameBufferB =  std::auto_ptr<GLFrameBuffer>( new GLFrameBuffer( maxWidth, maxHeight, GLTexture::tf_rgba ) );

	m_blurredFrame =  std::auto_ptr<GLFrameBuffer>( new GLFrameBuffer( maxWidth, maxHeight, GLTexture::tf_rgba ) );

	m_currentFrameTexture = std::auto_ptr<GLTexture>( new GLTexture( maxWidth, maxHeight, GLTexture::tf_rgba ) );

	m_motionMap = std::auto_ptr<GLFrameBuffer>( new GLFrameBuffer( maxWidth, maxHeight, GLTexture::tf_rgba ) );
	m_smallMotionMap = std::auto_ptr<GLFrameBuffer>( new GLFrameBuffer( maxWidth / downScale, maxHeight / downScale, GLTexture::tf_rgba ) );
}

void CgTestWindow::ThreadUnInit()
{
	m_frameBufferA.reset();
	m_frameBufferB.reset();
	m_currentFrameTexture.reset();
	m_smallMotionMap.reset();
	m_motionMap.reset();
	
	m_programBgAccumulate.reset();
	m_programMotionDetection.reset();
	m_programBlur.reset();

	m_textRender.reset();
	//m_pGrabber.reset();

}


void CgTestWindow::RenderBytes( const void* bytes )
{
	glPixelZoom( 2.0f, 2.0f );
	glRasterPos2d( -1.0, 0.5 );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glDrawPixels( maxWidth / downScale, maxHeight / downScale, GL_RED, GL_UNSIGNED_BYTE, bytes );		
}

void CgTestWindow::RenderTexture( GLuint textureID, unsigned int width, unsigned int height )
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable( targetTexture );
	glBindTexture( targetTexture, textureID );


	glColor3f(1.0, 1, 1);
	const float fraction = 1.0f;
	glBegin(GL_QUADS);
	{
		glTexCoord2i( 0, 0 ); 
		glVertex2f( -fraction,  -fraction );
		glTexCoord2i( 0, height ); 
		glVertex2f( -fraction, fraction );
		glTexCoord2i( width, height ); 
		glVertex2f( fraction, fraction );
		glTexCoord2i( width, 0 ); 
		glVertex2f( fraction, -fraction );
	}
	glEnd();
	glBindTexture( targetTexture, 0 );


	boost::optional<double> fps = m_fpsLogger.GetFPS();
	
	if( fps.get_ptr() != 0 )
		m_fpstext = (boost::format( "%.1f" ) % *fps.get_ptr() ).str();
		//m_fpstext = (boost::format( "%d" ) % m_currentFrames.size() ).str();


	for( size_t i = 0; i < m_currentFrames.size(); ++i )
	{
		DrawRectangle( m_currentFrames[i] );
	}
	m_currentFrames.clear();


	RGBQUAD color = { 255, 0, 0, 0 };
	m_textRender->DrawText( 0.0f, 0.0f, 0.5f, 0.5f, m_fpstext.c_str(), color );
	glPopMatrix();

	glDisable( targetTexture );
}

struct GLSmoothMode
{
	GLSmoothMode( )
	{
		glEnable( GL_LINE_SMOOTH );
		glEnable( GL_BLEND  );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
	}

	~GLSmoothMode( )
	{
		glDisable( GL_BLEND  );
		glDisable( GL_LINE_SMOOTH );
	}
};

void SetPen( float red, float green, float blue, float width  )
{
	glLineWidth( width );
	glColor3f( red, green, blue );

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE ); 
}

void CgTestWindow::DrawRectangle( const Frame& frame )
{

	float x0 = (float)frame.xLeft * 2.0f / ( (float)maxWidth / downScale ) - 1.0f;
	float y0 = (float)frame.yUp * 2.0f / ( (float)maxHeight / downScale ) - 1.0f;
	float x1 = (float)frame.xRight * 2.0f / ( (float)maxWidth / downScale ) - 1.0f;
	float y1 = (float)frame.yDown * 2.0f / ( (float)maxHeight / downScale ) - 1.0f;

	SetPen( 1.0f, 0.0f, 0.0f, 1.0f );
	GLSmoothMode _smooth;
	//Draw Quads
	glBegin( GL_QUADS );
	{
		glVertex2f( x0, y0 );
		glVertex2f( x0, y1 );
		glVertex2f( x1, y1 );
		glVertex2f( x1, y0 );
	}
	glEnd(); 

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
}
void CgTestWindow::InitPixelShader()
{
	std::vector< std::string > texParams;
	texParams.push_back( "videoFrame" );
	texParams.push_back( "background" );
	texParams.push_back( "motionMap" );

	m_programBgAccumulate = std::auto_ptr<CgProgram>( new CgProgram( "shader.cg",
		"background_accumulation", texParams ) );

	texParams.clear();
	texParams.push_back( "videoFrame" );
	texParams.push_back( "background" );

	m_programMotionDetection = std::auto_ptr<CgProgram>( new CgProgram( "shader.cg",
		"motion_detection", texParams ) );

	texParams.clear();
	texParams.push_back( "videoFrame" );

	m_programBlur = std::auto_ptr<CgProgram>( new CgProgram( "shader.cg",
		"blur_program", texParams ) );

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CgTestWindow::CopyYUY2ToVector( const BITMAPINFOHEADER * pHeader,
											const unsigned char* pBytes,
											std::vector<unsigned char> & v,
											bool bInvertY )
{
#define LIMIT(v,min,max)       ( ((v) < (min)) ? (min) : (((v) > (max)) ? (max) : (v)) )

	size_t W = pHeader->biWidth;
	size_t H = pHeader->biHeight;
	size_t scanW = 2 * W;

	if( v.size() != W * H * 3 )
		v.resize( W * H * 3, 128 );

	for (size_t y = 0; y < H; y++)
	{
		const unsigned char * src = pBytes + (bInvertY ? (H-1-y) : y) * scanW;
		unsigned char * dst = &v[0] + W * y * 3;

		for (size_t x = 0, i = 0; x < W; x+=2, i+=4)
		{
			unsigned char * c = dst + x * 3;
			int      y0 = (int)(src[i+0]);
			int      u  = (int)(src[i+1]);
			int      y1 = (int)(src[i+2]);
			int      v  = (int)(src[i+3]);
			int      R, G, B;

			y0 -= 16;
			y1 -= 16;
			u -= 128;
			v -= 128;

			B = ((1220542*y0             + 2116026*u + (1<<19)) >> 20);
			G = ((1220542*y0 -  852492*v -  409993*u + (1<<19)) >> 20);
			R = ((1220542*y0 + 1673527*v             + (1<<19)) >> 20);

			*(c + 0) = (unsigned char)LIMIT( B, 0, 0x0FF );
			*(c + 1) = (unsigned char)LIMIT( G, 0, 0x0FF );
			*(c + 2) = (unsigned char)LIMIT( R, 0, 0x0FF );

			c += 3;

			B = ((1220542*y1             + 2116026*u + (1<<19)) >> 20);
			G = ((1220542*y1 -  852492*v -  409993*u + (1<<19)) >> 20);
			R = ((1220542*y1 + 1673527*v             + (1<<19)) >> 20);

			*(c + 0) = (unsigned char)LIMIT( B, 0, 0x0FF );
			*(c + 1) = (unsigned char)LIMIT( G, 0, 0x0FF );
			*(c + 2) = (unsigned char)LIMIT( R, 0, 0x0FF );
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void 	CgTestWindow::GPUMemoryTest()
{
	const unsigned int szx = 352 / downScale;
	const unsigned int szy = 288 / downScale;

	GLFrameBuffer buffer( szx, szy, GLTexture::tf_rgb );

	const unsigned int byteSize = szx * szy * 3;
	unsigned char tmp [ byteSize ];
	
	const int iterNum = 100000;
	
	// start time count
	CCycleCount duration;
	
	//timer.Start();
	for( int i = 0; i < iterNum; i ++ )
	{
		CTimeAdder timer(&duration);
		buffer.ReadBytes( tmp, GLTexture::df_rgb );
		timer.End();
	//	Sleep( 10 );
	}
	// stop time count
	//Sleep(1000);
//	timer.End();
	
	double seconds = duration.GetSeconds();
	double fillRate = (double)byteSize / seconds / 1024.0; 
	//std::string s = ( boost::format( "%.5f \n" ) % ( fillRate / byteSize * 1024.0 ) ).str();

	std::string s = ( boost::format( "%.5f \n" ) % seconds ).str();
	
	OutputDebugStringA( s.c_str() );

}