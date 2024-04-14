#include "OneThreadWindow.h"

#include "fpslogger.h"

#include "../../GPUclasses/GLTexture.h"
#include "../../GPUclasses/GLFrameBuffer.h"
#include "../../GPUclasses/CgProgram.h"
#include "../../Grabber/Grabber.h"
#include "../../GPUclasses/WindowsFontBitmapTextRender.h"

#include "FrameExtractor.h"
#include "GPUTestParameters.h"

class CgTestWindow : public OneThreadWindow
{
public:
	CgTestWindow( CWnd* parent, boost::shared_ptr<Elvees::ICameraGrabber> cameraGrabber,  
		boost::shared_ptr< GPUTestParameters > parameters );
private:
	std::auto_ptr< CgProgram > m_programBgAccumulate;
	std::auto_ptr< CgProgram > m_programMotionDetection;

	std::auto_ptr< CgProgram > m_programBlur;


	std::vector<unsigned char> m_currentFrame;
	std::vector<unsigned char> m_currentResult;

	bool m_first;
	FpsLogger m_fpsLogger;

	std::auto_ptr< GLTexture > m_currentFrameTexture;
	std::auto_ptr< GLFrameBuffer > m_frameBufferA;
	std::auto_ptr< GLFrameBuffer > m_frameBufferB;
	std::auto_ptr< GLFrameBuffer > m_motionMap;

	std::auto_ptr< GLFrameBuffer > m_smallMotionMap;

	std::auto_ptr< GLFrameBuffer > m_blurredFrame;

	bool m_BufferA;

	boost::shared_ptr< Elvees::ICameraGrabber > m_pGrabber;
	
	std::auto_ptr< WindowsFontBitmapTextRender > m_textRender;
	std::string m_fpstext;


	FrameExtractor m_frameExtractor;
	std::vector< Frame > m_currentFrames;

	void CopyYUY2ToVector( const BITMAPINFOHEADER * pHeader,
		const unsigned char* pBytes,
		std::vector<unsigned char> & v,
		bool bInvertY );	

	boost::shared_ptr< GPUTestParameters > m_testParameters;

	virtual void	ThreadInit();
	virtual void	DoStuff();
	virtual void	ThreadUnInit();

	void RenderTexture( GLuint textureID, unsigned int width, unsigned int height );
	void RenderBytes( const void* bytes );
	void DrawRectangle( const Frame& frame );

	void InitPixelShader();
	void GPUMemoryTest();
};