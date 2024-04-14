///////////////////////////////////////////////////////////////////////////////////////////////////
// camera_thread.h
// ---------------------
// begin     : 2006
// modified  : 30 Jan 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_CAMERA_THREAD_H__
#define __DEFINE_CAMERA_THREAD_H__

#include "main.h"

namespace dvralgo
{

typedef  boost::shared_ptr<alib::IConsolePainterEx>  IConsolePainterPtr;
typedef  boost::shared_ptr<CCriticalSection>         CCriticalSectionPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class CameraThread.
/// \brief CameraThread.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CameraThread
{
protected:
  sint                           m_cameraNo;
  Elvees::IMessage             * m_pOut;
  const dvralgo::ApplicationData * m_pAppParams;
  CCriticalSectionPtr            m_pPaintLocker; //!< object locks/unlocks the data being transferred between threads
  Elvees::ICameraGrabberPtr      m_grabber;
  dvralgo::IConsolePainterPtr    m_painter;
  Elvees::ICameraAnalyzerPtr     m_analyzer;
  CWinThread                   * m_pThread;
  bool                           m_bFinished;

  friend UINT CameraThreadFunction( LPVOID pParam );

public:
  CameraThread();
  virtual ~CameraThread();
  void Initialize( sint cameraNo, const dvralgo::ApplicationData * pAppParams,
                   Elvees::IMessage * pOutput, Elvees::ICameraGrabber * pGrabber );

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Function gives access to the camera analyzer.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  const Elvees::ICameraAnalyzerPtr & GetAnalyzer() const { return m_analyzer; }
};

typedef  std::map<sint,CameraThread>  CameraThreadMap;

extern bool g_bContinueRunning;

} // namespace dvralgo

#endif // __DEFINE_CAMERA_THREAD_H__

