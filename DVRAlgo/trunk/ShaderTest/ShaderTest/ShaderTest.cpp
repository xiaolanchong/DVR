// ShaderTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

//#define GLH_EXT_SINGLE_FILE
const char * RequredExtensions =
"GL_ARB_fragment_program "
"GL_ARB_vertex_program "
"GL_EXT_framebuffer_object ";


//"GL_NV_float_buffer "

#include <glh/glh_extensions.h>


#include "../../GPUclasses/GLTexture.h"
#include "../../GPUclasses/GLFrameBuffer.h"
#include "../../GPUclasses/CgProgram.h"

#include "FpsLogger.h"
#include "..\..\Grabber\Grabber.h"

#include <vector>

std::auto_ptr<CgProgram> g_programBgAccumulate;
std::auto_ptr<CgProgram> g_programMotionDetection;

//const int g_scaleTo = 512;

const int g_windowHeight = maxHeight; 
const int g_windowWidth = maxWidth; 

std::vector<unsigned char> g_currentFrame;

bool g_first = true;

FpsLogger g_fpsLogger( 5 );

std::auto_ptr<GLTexture> g_currentFrameTexture;

std::auto_ptr<GLFrameBuffer> g_frameBufferA;
std::auto_ptr<GLFrameBuffer> g_frameBufferB;
std::auto_ptr<GLFrameBuffer> g_motionMap;

bool g_BufferA;

boost::shared_ptr< Elvees::ICameraGrabber > g_pGrabber;

///------------------------------------------------------------------------------------------------
/// \brief Function copies an image in YUY2 format to RGB one.
///
/// <pre><tt>
/// Convertion used (http://www.fourcc.org/fccyvrgb.php):
/// B = 1.164(Y - 16)                  + 2.018(U - 128)
/// G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
/// R = 1.164(Y - 16) + 1.596(V - 128)
/// </tt></pre>
///
/// \param  pHeader   pointer to the source image header.
/// \param  pBytes    pointer to the source image content.
/// \param  pImage    pointer to destination image.
/// \param  bInvertY  inverts axis Y if nonzero.
///------------------------------------------------------------------------------------------------
void CopyYUY2ToVector( const BITMAPINFOHEADER * pHeader,
												const ubyte * pBytes,
												std::vector<unsigned char> & v,
												bool bInvertY )
{
	#define LIMIT(v,min,max)       ( ((v) < (min)) ? (min) : (((v) > (max)) ? (max) : (v)) )

	int W = pHeader->biWidth;
	int H = pHeader->biHeight;
	int scanW = 2 * W;

	if( v.size() != W * H * 3 )
		v.resize( W * H * 3, 128 );

	for (int y = 0; y < H; y++)
	{
		const unsigned char * src = pBytes + (bInvertY ? (H-1-y) : y) * scanW;
		unsigned char * dst = &v[0] + W * y * 3;

		for (int x = 0, i = 0; x < W; x+=2, i+=4)
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

void LoadTexture();

void InitCameraGrabber()
{
	g_pGrabber = boost::shared_ptr< Elvees::ICameraGrabber >( Elvees::CreateChcsvaCameraGrabber( 0, 0 ) );
	if ( g_pGrabber.get() == 0 )
		exit( 1 );
}

void LoadYUVFrame( const BITMAPINFOHEADER * pHeader, const ubyte * pBytes, GLTexture& texture )
{
	CopyYUY2ToVector( pHeader, pBytes, g_currentFrame, true );
	texture.LoadBytes( &g_currentFrame[0] );
}

void LoadYUVFrame( const BITMAPINFOHEADER * pHeader, const ubyte * pBytes, GLFrameBuffer& buffer )
{
	CopyYUY2ToVector( pHeader, pBytes, g_currentFrame, true );
	buffer.LoadBytes( &g_currentFrame[0] );
}


void RenderTexture( GLuint textureID )
{
	glClearColor(0.2, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable( targetTexture );
	glBindTexture( targetTexture, textureID );
	

	glColor3f(1.0, 1, 1);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0); 
		glVertex2f(-0.9,  -0.9);
		glTexCoord2f(0, maxHeight ); 
		glVertex2f( -0.9, 0.9);
		glTexCoord2f(maxWidth, maxHeight); 
		glVertex2f( 0.9, 0.9);
		glTexCoord2f(maxWidth, 0); 
		glVertex2f( 0.9, -0.9);
	}
	glEnd();
	glPopMatrix();

	glDisable( targetTexture );

	glutSwapBuffers();
}

void RunProgram()
{
	glLoadIdentity();
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0); 
		glVertex2f(-1.0,  -1.0);
		glTexCoord2f(0, maxHeight); 
		glVertex2f( -1.0, 1.0);
		glTexCoord2f(maxWidth, maxHeight); 
		glVertex2f( 1.0, 1.0);
		glTexCoord2f(maxWidth, 0); 
		glVertex2f( 1.0, -1.0);
	}
	glEnd();
}

void display()
{	
	g_fpsLogger.IncrementFrame();
	boost::optional<double> res = g_fpsLogger.GetFPS(); 
	if( res.get_ptr() != 0 )
	{
		std::cout << " fps: " << res.get() << std::endl;
	}

	BITMAPINFOHEADER header =	g_pGrabber->GetHeader();
	const ubyte * pBytes = 0;
	ulong time;
	g_pGrabber->Read( pBytes, time );

	

	if( g_first )
	{
		LoadYUVFrame( &header, pBytes, *g_frameBufferA );
		LoadYUVFrame( &header, pBytes, *g_frameBufferB );

		g_first = false;
	}

	LoadYUVFrame( &header, pBytes, *g_currentFrameTexture );

	glBindTexture( targetTexture, 0 );

	if( !g_BufferA )
		g_programBgAccumulate->SetOutputBuffer( *g_frameBufferA );
	else
		g_programBgAccumulate->SetOutputBuffer( *g_frameBufferB );

	g_programBgAccumulate->SetTextureParameter( "videoFrame", *g_currentFrameTexture );
	
	if( g_BufferA )
		g_programBgAccumulate->SetTextureParameter( "background", *g_frameBufferA );
	else
		g_programBgAccumulate->SetTextureParameter( "background", *g_frameBufferB );

	g_programBgAccumulate->RunProgram();

	g_programMotionDetection->SetTextureParameter( "background", *g_frameBufferA );
	g_programMotionDetection->SetTextureParameter( "videoFrame", *g_currentFrameTexture );
	g_programMotionDetection->SetOutputBuffer( *g_motionMap );
	
	g_programMotionDetection->RunProgram();


	
	RenderTexture( g_motionMap->TextureID() );
	g_BufferA = !g_BufferA;

}

void menu( int v )
{
	switch ((unsigned char)v)
	{
	case '+':
		{
		}
		break;
	case '-':
		{
		}
		break;
	}
}

void key(unsigned char k, int x, int y)
{
	switch (k)
	{
	case 'q':
		exit(0);
		break;
	default:
		menu( k );
	}
	x;y;
}

void idle()
{
	// redisplay 
	glutPostRedisplay();	
}

void createMenu()
{
	////Menu entries//////////////////////////////////////////////////////////
	glutCreateMenu( menu );
	glutAddMenuEntry( "increase [+]", '+' );
	glutAddMenuEntry( "decrease [-]", '-' );
	glutAttachMenu( GLUT_RIGHT_BUTTON );
}

void InitOpenGL()
{
	//////////////////////////////////////////////////////////////////////////
	g_currentFrame.resize( maxWidth * maxHeight * 3, 128 );

	//////////////////////////////////////////////////////////////////////////
	if( !glh_init_extensions( RequredExtensions ) )
	{
		std::cout << "Necessary extensions were not supported:" << std::endl
			<< glh_get_unsupported_extensions() << std::endl;
		exit( -1 );
	}
	
	unsigned char tmpArr[ maxWidth * maxHeight * 3 ] = {0};

	// Frame Buffer A
	g_frameBufferA = std::auto_ptr<GLFrameBuffer>( new GLFrameBuffer( maxWidth, maxHeight, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, tmpArr ) );
	// Frame Buffer B
	g_frameBufferB = std::auto_ptr<GLFrameBuffer>( new GLFrameBuffer( maxWidth, maxHeight, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, tmpArr ) );

	g_currentFrameTexture = std::auto_ptr<GLTexture>( new GLTexture( maxWidth, maxHeight, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, &g_currentFrame[0] ) );
	
	g_motionMap= std::auto_ptr<GLFrameBuffer>( new GLFrameBuffer( maxWidth, maxHeight, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, tmpArr ) );
}


void UnInitOpenGL()
{
	g_frameBufferA.reset();
	g_frameBufferB.reset();
	g_currentFrameTexture.reset();
	g_motionMap.reset();

	g_programBgAccumulate.reset();
	g_programMotionDetection.reset();

}
//void LinkTextureToCgProgram( GLuint textureID, std::string paramName, CGparameter& param )
//{
//	param = cgGetNamedParameter( g_fragmentProgram, paramName.c_str() );
//}

void UnInitCameraGrabber()
{
	g_pGrabber.reset();
	Elvees::DeleteChcsvaCameraGrabber();
}
void InitPixelShader()
{
	std::vector< std::string > texParams;
	texParams.push_back( "videoFrame" );
	texParams.push_back( "background" );
	texParams.push_back( "motionMap" );

	g_programBgAccumulate = std::auto_ptr<CgProgram>( new CgProgram( "D:\\projects\\DVR\\HEAD\\DVRAlgo\\trunk\\ShaderTest\\ShaderTest\\shaders\\shader.cg",
	"background_accumulation", texParams ) );

	texParams.clear();
	texParams.push_back( "videoFrame" );
	texParams.push_back( "background" );
	
	g_programMotionDetection = std::auto_ptr<CgProgram>( new CgProgram("D:\\projects\\DVR\\HEAD\\DVRAlgo\\trunk\\ShaderTest\\ShaderTest\\shaders\\shader.cg",
		"motion_detection", texParams ) );
}


int main(int argc, char* argv[])
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowSize( g_windowWidth, g_windowHeight );
	glutCreateWindow( "Test Shsaders Demo") ;
	
	//////////////////////////////////////////////////////////////////////////
	createMenu();	
	//////////////////////////////////////////////////////////////////////////
		
	glutIdleFunc( idle );
	glutKeyboardFunc( key );
	glutDisplayFunc( display );

	InitOpenGL();
	InitPixelShader();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	InitCameraGrabber();
	glutMainLoop();

	UnInitCameraGrabber();

	glh_shutdown_extensions();
	return 0;
}



/*
glBindTexture( GL_TEXTURE_2D, 0);
glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, PixelBuffers[0] );
{// Drawing the teapot
glClearColor(0.1, 0.8, 0.8, 1.0);
glClear( GL_COLOR_BUFFER_BIT );
glMatrixMode( GL_MODELVIEW );
glPushMatrix();
glLoadIdentity();


glutWireTeapot( 0.5 );
glPopMatrix();

}*/