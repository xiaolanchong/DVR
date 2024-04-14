#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

typedef  boost::shared_ptr<Elvees::ICameraGrabber>  ICameraGrabberPtr;
typedef  std::vector<ICameraGrabberPtr>             ICameraGrabberPtrArr;

typedef  boost::shared_ptr<alib::IConsolePainter>   IConsolePainterPtr;
typedef  std::vector<IConsolePainterPtr>            IConsolePainterPtrArr;

int main( int argc, char * argv[] )
{
  argc;argv;
  if (!AfxWinInit( ::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0 ))
    return 1;

  try
  {
    IConsolePainterPtrArr painters;
    ICameraGrabberPtrArr  grabbers;
    BITMAPINFOHEADER      bi;
    const ubyte *         pBytes;
    ulong                 time;
    Arr2ub                grayImage;
    QImage                colorImage;
    char                  statusText[256];
    int                   cameraNum = 32;

    grabbers.reserve( cameraNum );
    for (int i = 0; i < cameraNum; i++)
    {
      Elvees::ICameraGrabber * p = Elvees::CreateChcsvaCameraGrabber( i, 0 );
      if (p == 0)
        break;
      cameraNum = Elvees::GetNumberOfAvailablesCameras();
      grabbers.push_back( ICameraGrabberPtr( p ) );
    }
    ELVEES_VERIFY( !(grabbers.empty()), "No one camera-grabber was created" );

    painters.resize( grabbers.size() );
    for (int k = 0; k < (int)(painters.size()); k++)
    {
      painters[k].reset( alib::CreateWindowsConsolePainter( "Camera", false, false ) );
      ELVEES_ASSERT( (painters[k]).get() != 0 );
    }

    while (kbhit()); // clear message queue
    for (int ch = 0; !kbhit() || ((ch = getch()) != 27);)
    {
      while (kbhit()); // clear message queue

      for (int k = 0; k < (int)(grabbers.size()); k++)
      {
        sprintf( statusText, "Camera %d", k );
        bi = (grabbers[k])->GetHeader();
        ELVEES_ASSERT( (grabbers[k])->Read( pBytes, time ) );
#if 0
        Elvees::CopyYUY2ToGrayImage( &bi, pBytes, &grayImage, true );
        (painters[k])->CopyAndDrawDemoImage( &grayImage, statusText );
#else
        Elvees::CopyYUY2ToRGBImage( &bi, pBytes, &colorImage, true );
        (painters[k])->CopyAndDrawDemoImage( &colorImage, statusText );
#endif
      }
    }
  }
  catch (std::runtime_error & e)
  {
    std::cout << e.what() << std::endl << std::endl;
    getch();
  }
  return 0;
}

