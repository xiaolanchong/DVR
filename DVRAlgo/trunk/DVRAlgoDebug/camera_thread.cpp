///////////////////////////////////////////////////////////////////////////////////////////////////
// camera_thread.cpp
// ---------------------
// begin     : 2006
// modified  : 23 Jan 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "camera_thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace dvralgo
{

bool g_bContinueRunning = true;

UINT CameraThreadFunction( LPVOID pParam );

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
CameraThread::CameraThread()
{
  m_cameraNo = -1;
  m_pOut = 0;
  m_pAppParams = 0;
  m_pPaintLocker.reset();
  m_grabber.reset();
  m_painter.reset();
  m_analyzer.reset();
  m_pThread = 0;
  m_bFinished = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Destructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
CameraThread::~CameraThread()
{
  char text[512];

  // Close window first.
  if (m_pPaintLocker.get() != 0)
  {
    CSingleLock lock( m_pPaintLocker.get(), TRUE );
    m_painter.reset();
  }

  // Close the thread.
  if ((m_pThread != 0) && (m_pThread->m_hThread != 0))
  {
    dvralgo::g_bContinueRunning = false; // break thread's loop

    // Try to close the thread normally.
    for (int attempt = 0; (attempt < (777*dvralgo::MAX_CAMERA_NUMBER)) && !m_bFinished; attempt++)
      Sleep(7);

    // Try to close the thread forcibly.
    if (!m_bFinished)
    {
      int i = sprintf( text, "Camera[%d]: thread is being forcibly terminating ... ", m_cameraNo );
      if (::TerminateThread( m_pThread->m_hThread, 2 ))
        sprintf( text+i, "TerminateThread() failed" );
      else
        sprintf( text+i, "TerminateThread() succeeded" );
    }
    else sprintf( text, "Camera[%d]: thread was normally finalized", m_cameraNo );

    if (m_pOut != 0)
      m_pOut->Print( Elvees::IMessage::mt_debug_info, text );
  }

  // Close grabber, analyzer and locker(s) in the end.
  m_grabber.reset();
  m_analyzer.reset();
  m_pPaintLocker.reset();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function initializes a camera thread.
///
/// \param  cameraNo  the index of a camera.
/// \param  iniFile   the name of parameter file.
/// \param  pOutput   pointer to the message handler.
/// \param  pGrabber  pointer to created camera grabber.
///////////////////////////////////////////////////////////////////////////////////////////////////
void CameraThread::Initialize( sint                             cameraNo,
                               const dvralgo::ApplicationData * pAppParams,
                               Elvees::IMessage               * pOutput,
                               Elvees::ICameraGrabber         * pGrabber )
{
  char text[256]; 
  ASSERT( pAppParams != 0 );
  m_cameraNo = cameraNo;
  m_pOut = pOutput;
  m_pAppParams = pAppParams;
  m_pPaintLocker.reset( new CCriticalSection() );
  ELVEES_ASSERT( m_pPaintLocker.get() != 0 );

  m_grabber.reset( pGrabber );
  sprintf( text, "Camera %d", m_cameraNo );
  m_painter.reset( alib::CreateWindowsConsolePainterEx( text, false, dvralgo::INVERT_AXIS_Y, 16, 4, false ) );
  m_analyzer.reset( funcCreateCameraAnalyzer( cameraNo, &(pAppParams->algoParams), (const Arr2ub*)0,
                                              pOutput/*, m_painter->GetDC()*/ ) );
  ELVEES_ASSERT( m_analyzer.get() != 0 );
  m_pThread = AfxBeginThread(
    dvralgo::CameraThreadFunction, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, 0 );

  ELVEES_ASSERT( m_painter.get() != 0 );
  ELVEES_VERIFY( m_analyzer.get() != 0, "Failed to create a camera analyzer" );
  ELVEES_VERIFY( m_pThread != 0, "Failed to create a camera thread" );

  m_pThread->ResumeThread();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Controlling function for the worker thread.
///////////////////////////////////////////////////////////////////////////////////////////////////
UINT CameraThreadFunction( LPVOID pParam )
{
  if (pParam == 0)
    return 1;

  dvralgo::CameraThread * pCamera = reinterpret_cast<dvralgo::CameraThread*>( pParam );
  BITMAPINFO              biYUV2;
  TQImage                 colorImage;
  TByteImagePtr           grayImage;
  const ubyte           * pBytes = 0;
  ulong                   time = 0;
//  char                    statusText[256];
  dvralgo::TInfoPage      infoPage;
  dvralgo::TMenuPtr       menuPtr;

  memset( &biYUV2, 0, sizeof(biYUV2) );
  biYUV2.bmiHeader = pCamera->m_grabber->GetHeader();
  if (biYUV2.bmiHeader.biCompression != (DWORD)(alib::MakeFourCC('Y','U','Y','2')))
  {
    if (pCamera->m_pOut != 0)
      pCamera->m_pOut->Print( Elvees::IMessage::mt_error, "YUY2 format is expected" );
    return 1;
  }

  try
  {
    // Initialize painter's menu.
    if ((pCamera->m_analyzer.get() != 0) && (pCamera->m_pPaintLocker.get() != 0))
    {
      CSingleLock lock( pCamera->m_pPaintLocker.get(), TRUE );
      pCamera->m_analyzer->GetData( &menuPtr );
      if ((menuPtr.data != 0) && (pCamera->m_painter.get() != 0))
        pCamera->m_painter->InitializeMenu( menuPtr.data );
    }

    // Thread's loop.
    while (dvralgo::g_bContinueRunning)
    {
      // Weak checking for validity.
      if ((pCamera->m_grabber.get() == 0) || (pCamera->m_analyzer.get() == 0))
        break;

      // Read the next frame.
      if (!(pCamera->m_grabber->Read( pBytes, time )))
        break;

      // Do processing.
      switch (pCamera->m_pAppParams->algorithmNo)
      {
        case 1:
        {
          ulong t = g_timer.Time();
          //std::cout << "cam analyzer " << pCamera->m_cameraNo << " time " << t << std::endl;
          //Sleep(500);
					if (!(pCamera->m_analyzer->Process( &(biYUV2.bmiHeader), pBytes, t )))
            break;

          // Redraw the window associated with this thread.
          if (pCamera->m_pPaintLocker.get() != 0)
          {
            CSingleLock lock( pCamera->m_pPaintLocker.get(), TRUE );
            if (pCamera->m_painter.get() != 0)
            {
              for (int pictureNo = 0; pictureNo < IMAGE_NUMBER_TO_SHOW; ++pictureNo)
              {
                grayImage.data.first = pictureNo;
                grayImage.data.second = 0;
                if (pCamera->m_analyzer->GetData( &grayImage ))
                  pCamera->m_painter->CopyAndDrawDemoImage( pictureNo, grayImage.data.second, "", true );
              }
            }
          }
        }
        break;

        //case 2:
        //{
        //  // Convert: YUY2 --> RGB32.
        //  dvralgo::ConvertYUY2ToColorImage<2>( &(biYUV2.bmiHeader), pBytes, &(colorImage.data), true );

        //  BITMAPINFOHEADER biRGB;
        //  memset( &biRGB, 0, sizeof(biRGB) );
        //  biRGB.biSize = sizeof(BITMAPINFOHEADER);
        //  biRGB.biPlanes = 1;
        //  biRGB.biBitCount = 32;
        //  biRGB.biWidth = colorImage.data.width();
        //  biRGB.biHeight = colorImage.data.height();
        //  biRGB.biSizeImage = colorImage.data.size() * sizeof(QImage::value_type);

        //  if (!(pCamera->m_analyzer->Process( &biRGB,
        //          reinterpret_cast<const ubyte*>( colorImage.data.begin() ), time )))
        //    break;
        //  if (!(pCamera->m_analyzer->GetData( &colorImage )))
        //    break;

        //  // Redraw the window associated with this thread.
        //  if (pCamera->m_pPaintLocker.get() != 0)
        //  {
        //    CSingleLock lock( pCamera->m_pPaintLocker.get(), TRUE );

        //    if (pCamera->m_painter.get() != 0)
        //    {
        //      // .
        //      {
        //        pCamera->m_analyzer->GetData( &infoPage );
        //        pCamera->m_painter->PrintConsoleText( infoPage.data.c_str() );
        //      }

        //      sprintf( statusText, "Camera %d", pCamera->m_cameraNo );
        //      //pCamera->m_painter->SwapAndDrawDemoImage( &(colorImage.data), statusText );
        //    }
        //  }
        //}
        //break;

        default: ELVEES_ASSERT(0);
      }
    }
  }
  catch (std::runtime_error & e)
  {
    if (pCamera->m_pOut != 0)
      pCamera->m_pOut->Print( Elvees::IMessage::mt_error, e.what() );

    // Close window first.
    if (pCamera->m_pPaintLocker.get() != 0)
    {
      CSingleLock lock( pCamera->m_pPaintLocker.get(), TRUE );
      pCamera->m_painter.reset();
    }
  }
  pCamera->m_bFinished = true;
  return 0;
}

} // namespace dvralgo

