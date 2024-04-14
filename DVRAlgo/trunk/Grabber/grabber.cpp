#include "stdafx.h"
#include "grabber.h"
#include "../../../chcsva/trunk/chcsva.h"
#include "../../../Shared/Interfaces/i_camera_grabber.h"
#include "../../../Shared/Interfaces/i_message.h"
#include "../../../Shared/Common/debug_macros.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct CGrabberDll.
/// \brief  CGrabberDll.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct CCrabberDll : public CWinApp
{
	
  std::vector<sint> m_usedCameras;     //!< array of cameras in use (-1 for unused camera)
//  CAUUID            m_streamsUIDs;     //!< storage of stream UIDs
  bool              m_bManagerInited;  //!< nonzero if the stream manager has been initialized
  bool              m_bStreamsInited;  //!< nonzero if all available streams have been intialized
	std::vector<long> m_streamsIDs;
	CHCS::IStreamManager* m_pStreamManager;
///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
CCrabberDll()
{
  OutputDebugStringA( "CCrabberDll()\n" );
  m_usedCameras.clear();
  m_bManagerInited = false;
  m_bStreamsInited = false;
	m_pStreamManager = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function cleans up when your application terminates.
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ExitInstance()
{
  OutputDebugStringA( "ExitInstance()\n" );
//  if (m_bManagerInited)
//	{
   // CHCS::FreeStreamManager();
//		m_pStreamManager->Release();
//		m_pStreamManager = 0;
//	}
  return CWinApp::ExitInstance();
}

  DECLARE_MESSAGE_MAP()
};

CCrabberDll theDll;

BEGIN_MESSAGE_MAP(CCrabberDll, CWinApp)
END_MESSAGE_MAP() 

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct ChcsvaCameraGrabber.
/// \brief  ChcsvaCameraGrabber.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ChcsvaCameraGrabber : public Elvees::ICameraGrabber
{
  BITMAPINFOHEADER   m_header;       //!< common header of all frames of opened stream
  bool               m_bInitilized;  //!< nonzero if the camera grabber has been initialized
  sint               m_cameraNo;     //!< index of associated a camera (0,1,2,...)
  CHCS::IFrame     * m_pFrame;       //!< pointer to the current frame
  CHCS::IStream    * m_pStream;      //!< pointer to the opened video-stream
  Elvees::IMessage * m_pOut;         //!< pointer to the message handler
	

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///
/// \param  pOutput  pointer to the message handler.
///////////////////////////////////////////////////////////////////////////////////////////////////
ChcsvaCameraGrabber( Elvees::IMessage * pOutput )
{
  memset( &m_header, 0, sizeof(m_header) );
  m_bInitilized = false;
  m_cameraNo = -1;
  m_pFrame = 0;
  m_pStream = 0;
  m_pOut = pOutput;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Destructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual ~ChcsvaCameraGrabber()
{
  if (m_pFrame != 0)
    m_pFrame->Release();
  if (m_pStream != 0)
{
    m_pStream->Release();
//>>>>> Hack
m_pStream->Release();
//>>>>>
}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns the general header of images of a sequence.
///
/// \return  reference to the internal header storage.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual const BITMAPINFOHEADER & GetHeader() const
{
  ELVEES_VERIFY( m_bInitilized, "Grabber was not initialized" );
  return m_header;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function reads a new frame from a sequence, and, in the case of video file,
///        moves internal pointer to next frame.
///
/// It is assumed that returned pointer to the image buffer remains valid until the next call of
/// Read(..) function, i.e. the image is locked between successive calls.
///
/// \param  pBytes  reference to a pointer that will receive the address of internal image buffer.
/// \param  time    the time stamp associated with the frame being read.
/// \return         nonzero if a frame has been successfully read, otherwise zero
///                 if the end of a file has been reached or video became unavailable.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool Read( const ubyte *& pBytes, ulong & time )
{

 // std::cout << "frame read by cam " << m_cameraNo << std::endl;
   try
  {
    pBytes = 0;
    time = 0;
    if (m_pFrame != 0)
      m_pFrame->Release();
    m_pFrame = 0;
    ELVEES_ASSERT( m_bInitilized );
    ELVEES_ASSERT( m_pStream->GetNextFrame( &m_pFrame, 100 ) );
    pBytes = (m_pFrame != 0) ? reinterpret_cast<const ubyte*>( m_pFrame->GetBytes() ) : 0;
    ELVEES_ASSERT( pBytes != 0 );
  }
  catch (std::runtime_error & e)
  {
    if (m_pOut != 0) m_pOut->Print( Elvees::IMessage::mt_error, e.what() );
    throw;
  }
//	Sleep(200);
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function initializes a camera given the camera index.
///
/// \param  cameraNo  index of a camera.
/// \return           Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool Initialize( sint cameraNo )
{
  try
  {
    ELVEES_ASSERT( !m_bInitilized );
    if (theDll.m_usedCameras.empty())
    {
      CHCS::INITCHCS initCHCS;
      initCHCS.dwSize = sizeof(initCHCS);
      initCHCS.dwMode = CHCS_MODE_DEVICE|CHCS_MODE_DATABASE
#if 0
      | CHCS_MODE_DEVICENEW;
#else
      ;
#endif
      initCHCS.dwReserved = 0;
      initCHCS.dwUserData = 0;
      initCHCS.procOutput = NULL;

      ELVEES_VERIFY( theDll.m_pStreamManager = CHCS::InitStreamManager( &initCHCS ), "CHCS::InitStreamManager() failed" );
      theDll.m_bManagerInited = true;

			size_t sz;
			theDll.m_streamsIDs.resize( 100 );
      ELVEES_VERIFY( theDll.m_pStreamManager-> GetAvailableStreamIDs( &(theDll.m_streamsIDs[0]), &sz), "CHCS::GetAvailableStreams() failed" );
			theDll.m_streamsIDs.resize( sz );

      theDll.m_bStreamsInited = true;
			ELVEES_VERIFY( sz > 0, "No video-streams were found" );
      theDll.m_usedCameras.resize( sz, (sint)(-1) );
    }
    ELVEES_ASSERT( (0 <= cameraNo) && (cameraNo < (int)theDll.m_streamsIDs.size()) );
    ELVEES_VERIFY( theDll.m_usedCameras[cameraNo] < 0, "The camera is already in use" );
    ELVEES_ASSERT( theDll.m_pStreamManager->GetStreamByID( &m_pStream, theDll.m_streamsIDs[cameraNo]) );
    ELVEES_ASSERT( m_pStream != 0 );

    CHCS::IFrame * pFrm = 0;
    int attempt = 32000;
    while ((attempt-- > 0) && !(m_pStream->GetNextFrame( &pFrm, 1000 )))
    {
//      OutputDebugString( (pFrm == 0) ? "frame pointer == 0\n" : "frame pointer != 0\n" );
    }
    ELVEES_ASSERT( (attempt > 0) && (pFrm != 0) ); 
    ELVEES_ASSERT( pFrm->GetFormat(0) == sizeof(BITMAPINFOHEADER) );
    ELVEES_ASSERT( pFrm->GetFormat( reinterpret_cast<BITMAPINFO*>( &m_header ) ) >= 0 );
    if (pFrm != 0)
      pFrm->Release();

    theDll.m_usedCameras[cameraNo] = cameraNo;
    m_cameraNo = cameraNo;
    m_bInitilized = true;
  }
  catch (std::runtime_error & e)
  {
    if (m_pOut != 0) m_pOut->Print( Elvees::IMessage::mt_error, e.what() );
    return false;
  }
  return true;
}

};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Global Functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function creates an instance of camera grabber.
///
/// \param  cameraNo  index of a camera.
/// \param  pOutput   pointer to the message handler.
/// \return           nonzero pointer if the camera grabber was successfully created.
///////////////////////////////////////////////////////////////////////////////////////////////////
Elvees::ICameraGrabber * CreateChcsvaCameraGrabber( sint cameraNo, Elvees::IMessage * pOutput)
{
  ChcsvaCameraGrabber * p = new ChcsvaCameraGrabber( pOutput );
  if ((p != 0) && p->Initialize( cameraNo ))
    return reinterpret_cast<Elvees::ICameraGrabber*>( p );
  delete p;
  return 0;
}

void DeleteChcsvaCameraGrabber()
{
	if (theDll.m_bManagerInited)
	{
		// CHCS::FreeStreamManager();
		theDll.m_pStreamManager->Release();
		theDll.m_pStreamManager = 0;
	}
	theDll.m_bManagerInited = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function return the number of availables cameras.
///
/// \return  the number of availables cameras.
///////////////////////////////////////////////////////////////////////////////////////////////////
sint GetNumberOfAvailablesCameras()
{
  return (sint)(theDll.m_streamsIDs.size() );
}

} // namespace Elvees

