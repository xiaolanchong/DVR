///////////////////////////////////////////////////////////////////////////////////////////////////
// camera_analyzer.cpp
// ---------------------
// begin     : 2006
// modified  : 20 Jan 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../lib.h"
#include "../../Common/dvr_common.h"
#include "algo2_objects.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef bool (__stdcall * PF_CreateImageIndirect)( Elvees::Win32::CImage**, LPBITMAPINFO , LPBYTE );

namespace dvralgo2
{

class CImage : public Elvees::Win32::CImage
{
public:
  LPBITMAPINFO m_pBI;
  LPBYTE       m_pBytes;

public:
  CImage( LPBITMAPINFO pBI, LPBYTE pBytes ) : m_pBI(pBI), m_pBytes(pBytes) {}

	virtual bool GetImageInfo( LPBITMAPINFO pBih )
  {
    ASSERT( (m_pBI != 0) && (pBih != 0) );
    (*pBih) = (*m_pBI);
    return true;
  }

	virtual bool GetImageBytes( LPBYTE * ppBytes )
  {
    ASSERT( (m_pBytes != 0) && (ppBytes != 0) );
    (*ppBytes) = m_pBytes;
    return true;
  }

	virtual long AddRef()                            { return 0;     }
 	virtual long Release()                           { return 0;     }
  virtual bool IsManaged()                         { return false; }
  virtual long GetSourceId()                       { return 0;     }
	virtual bool GetImageTime(double*)               { return false; }
	virtual bool Draw(HDC,long,long,long,long)       { return false; }
	virtual bool SaveImage(unsigned short*)          { return false; }
	virtual bool CreateCopy(Elvees::Win32::CImage**) { return false; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct CameraAnalyzer.
/// \brief  Implementation of camera analyzer.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct CameraAnalyzer : public Elvees::ICameraAnalyzer
{
  bool                    m_bGood;               //!< state flag: nonzero if surveillance is currently successful
  bool                    m_bReady;              //!< state flag: nonzero if this object is properly initialized
  CCriticalSection        m_dataLocker;          //!< object locks/unlocks the data being transferred between threads
  Elvees::IMessage      * m_pOut;                //!< pointer to the message handler
  std::fstream            m_log;                 //!< log file stream
  HINSTANCE               m_hMapImgDll;          //!< 
  HINSTANCE               m_hSRSDll;             //!< 
  CIAnalyzer            * m_pAnalyzer;           //!< 
  Elvees::Win32::CImage * m_pImage;              //!< 
  PF_CreateImageIndirect  m_CreateImageIndirect; //!< pointer to CreateImageIndirect() function
  dvralgo2::ObjectArr     m_objects;             //!< 
  int                     m_cameraNo;            //!<

  Elvees::AbstractMenu    m_menu;                //!< feedback menu used for debugging
  sint                    m_selectedInfoPage;    //!< index of selected info page

//>>>>>
  HDC * m_pHdc;
//>>>>>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
CameraAnalyzer( Elvees::IMessage * pOutput )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  m_bGood = false;
  m_bReady = false;
  m_dataLocker;
  m_pOut = pOutput;
  m_log.close();
  m_hMapImgDll = 0;
  m_hSRSDll = 0;
  m_pAnalyzer = 0;
  m_pImage = 0;
  m_CreateImageIndirect = 0;
  m_objects.clear();
  m_objects.reserve(100);
  m_cameraNo = -1;

  m_menu.Clear();
  m_selectedInfoPage = -1;

  //>>>>>
  m_pHdc=0;
  //>>>>>
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Destructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual ~CameraAnalyzer()
{
  CSingleLock lock( &m_dataLocker, TRUE );

  m_bGood = false;

  if ((m_hSRSDll != 0) && !AfxFreeLibrary( m_hSRSDll ))
  {
    if (m_pOut != 0)
      m_pOut->Print( Elvees::IMessage::mt_error, "Failed to free safely the library 'SRSDllBase.dll'" );
  }

  if (m_pImage != 0)
    m_pImage->Release();

  if ((m_hMapImgDll != 0) && !AfxFreeLibrary( m_hMapImgDll ))
  {
    if (m_pOut != 0)
      m_pOut->Print( Elvees::IMessage::mt_error, "Failed to free safely the library 'mapimg.dll'" );
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function processes the current frame.
///
/// \param  pHeader    pointer to bitmap header.
/// \param  pBytes     pointer to bitmap contents.
/// \param  timeStamp  current time in milliseconds.
/// \return            Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool Process( const BITMAPINFOHEADER * pHeader,
                      const ubyte            * pBytes,
                      ulong                    timeStamp )
{
  timeStamp;
  CSingleLock lock( &m_dataLocker, TRUE );
  BITMAPINFO  bi;

  try
  {
    ELVEES_ASSERT( m_bGood );
    ELVEES_ASSERT( m_bReady );
    ELVEES_ASSERT( (pHeader != 0) && (pBytes != 0) && (pHeader->biBitCount == 32) );

/*
    // Obtain and fill up CImage object.
    if (m_pImage == 0)
    {
      bi.bmiHeader = (*pHeader);
      bool bCreate = m_CreateImageIndirect( &m_pImage, &bi, const_cast<LPBYTE>( pBytes ) );
      ELVEES_VERIFY( bCreate, "Failed to create CImage" );
    }
    else
    {
      ASSERT( m_pImage->GetImageInfo( &bi ) && (bi.bmiHeader.biBitCount == 32) );
      ASSERT( bi.bmiHeader.biWidth == pHeader->biWidth );
      ASSERT( bi.bmiHeader.biHeight == pHeader->biHeight );

      LPBYTE pBuffer = 0;
      ELVEES_ASSERT( m_pImage->GetImageBytes( &pBuffer ) && (pBuffer != 0) );
      memcpy( pBuffer, pBytes, abs( pHeader->biWidth*pHeader->biHeight*(pHeader->biBitCount/8) ) );
    }

    // Process the new frame and extract detected moving objects.
    {
      ObjectReg * pObj = 0;
      long        N = 0;

      ELVEES_ASSERT( m_pAnalyzer->HandleBmp( m_pImage ) == 0 );
      m_pAnalyzer->GetObjects( &pObj, &N );
      for (int i = 0; i < N; i++) (m_objects[i]).Copy( pObj[i] );
      m_pAnalyzer->DeleteObjects( pObj );
    }
*/

    // Process the new frame and extract detected moving objects.
    {
      bi.bmiHeader = (*pHeader);

      ObjectReg * pObj = 0;
      long        N = 0;
      CImage      cimage( &bi, const_cast<LPBYTE>( pBytes ) );

      ELVEES_ASSERT( m_pAnalyzer->HandleBmp( &cimage ) == 0 );
      m_pAnalyzer->GetObjects( &pObj, &N );
      m_objects.resize( N );
      for (int i = 0; i < N; i++) (m_objects[i]).Copy( pObj[i] );
      m_pAnalyzer->DeleteObjects( pObj );

      if (m_pOut != 0)
      {
        char text[256];
        sprintf( text, "Camera[%d]: number of objects found = %d", /*m_cameraNo*/-1, N );
        //m_pOut->Print( Elvees::IMessage::mt_debug_info, text );
      }
    }

    // Process menu.
    sint selectedId = (sint)(m_menu.GetSelectedItemId());
    if (selectedId >= 0)
    {
      m_selectedInfoPage = -1;
      Elvees::AbstractMenu::ItemArr::iterator i;
      for (i = m_menu.m_items.begin(); i != m_menu.m_items.end(); ++i)
      {
        i->m_bChecked = (ubyte)(selectedId == i->m_id);
        if (i->m_bChecked) m_selectedInfoPage = i->m_id;
      }
      m_menu.SetSelectedItemId(-1);
    }
  }
  catch (std::runtime_error & e)
  {
    HandleException( e );
    return false;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function uploads specified data to this object.
///
/// \param  pData  pointer to the input data.
/// \return        Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool SetData( const Elvees::IDataType * pData )
{
  CSingleLock lock( &m_dataLocker, TRUE );

  try
  {
    ELVEES_ASSERT( m_bGood && (pData != 0) );
    switch (pData->type())
    {
      case dvralgo::DATAID_FROM_HALL_TO_CAMERA:
      {
        const dvralgo::TFromHallToCamera * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        ELVEES_ASSERT( p->data.first == m_cameraNo );
        p->data.second;
      }
      break;

      default: return false;
    }
  }
  catch (std::runtime_error & e)
  {
    HandleException( e );
    return false;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function unloads specified data from this object.
///
/// \param  pData  pointer to the output (possibly resizable) data storage, say STL container.
/// \return        Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool GetData( Elvees::IDataType * pData ) const
{
  CameraAnalyzer * pThis = const_cast<CameraAnalyzer*>( this );
  CSingleLock      lock( &(pThis->m_dataLocker), TRUE );

  try
  {
    ELVEES_ASSERT( m_bGood && (pData != 0) );
    switch (pData->type())
    {
      case dvralgo::DATAID_QIMAGE:
      {
        dvralgo::TQImage * p = 0;
        Elvees::DynamicCastPtr( pData, &p );

        QImage & image = p->data;
        int      H = image.height();

        for (dvralgo2::ObjectArr::const_iterator it = m_objects.begin(); it != m_objects.end(); ++it)
        {
          const RECT & r = it->rect;
          RGBQUAD      black = {0,0,0,0};

          for (int x = r.left; x < r.right; x++)
          {
            image.set_if_in( x, H-1-r.top, black );
            image.set_if_in( x, H-1-r.bottom, black );
          }

          for (int y = r.top; y < r.bottom; y++)
          {
            image.set_if_in( r.left, H-1-y, black );
            image.set_if_in( r.right, H-1-y, black );
          }
        }
      }
      break;

      case dvralgo::DATAID_MENU_PTR:
      {
        dvralgo::TMenuPtr * p = 0;
        Elvees::DynamicCastPtr( pData, &p );

        // Create and fill menu.
        if ((m_pAnalyzer != 0) && m_menu.m_items.empty())
        {
          long nInfoPage = pThis->m_pAnalyzer->GetNumOfInfoPages();
          pThis->m_menu.Clear();
          pThis->m_menu.m_items.resize( nInfoPage );
          for (long t = 0; t < nInfoPage; t++)
          {
            Elvees::AbstractMenu::Item & i = pThis->m_menu.m_items[t];
            strncat( i.m_title, pThis->m_pAnalyzer->GetPageTitle( t ), Elvees::AbstractMenu::TEXT_SIZE );
            i.m_bChecked = (ubyte)(t == 0);
            i.m_id = (sint)t;
          }
          pThis->m_selectedInfoPage = ((nInfoPage > 0) ? 0 : -1);
          p->data = &(pThis->m_menu);
        }
      }
      break;

      case dvralgo::DATAID_INFO_PAGE:
      {
        dvralgo::TInfoPage * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        p->data.clear();
        if ((m_pAnalyzer != 0) && (m_selectedInfoPage >= 0))
        {
          if (m_selectedInfoPage < pThis->m_pAnalyzer->GetNumOfInfoPages())
            p->data = pThis->m_pAnalyzer->GetInfoPage( m_selectedInfoPage );
        }
      }
      break;

      case dvralgo::DATAID_FROM_CAMERA_TO_HALL:
      {
        dvralgo::TFromCameraToHall * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        p->data.first = m_cameraNo;
        p->data.second.clear();
      }
      break;

      default: return false;
    }
  }
  catch (std::runtime_error & e)
  {
    HandleException( e );
    return false;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function prints exception message.
/// 
/// \param  e  pointer to the exception object or 0 (unsupported exception).
///////////////////////////////////////////////////////////////////////////////////////////////////
void HandleException( const std::runtime_error & e ) const
{
  CameraAnalyzer * pThis = const_cast<CameraAnalyzer*>( this );
  pThis->m_bGood = false;
  if (m_pOut != 0) m_pOut->Print( Elvees::IMessage::mt_error, e.what() );
  if (m_log.is_open() && m_log.good())
    (pThis->m_log) << std::endl << std::endl << e.what() << std::endl << std::endl;
}

}; // struct CameraAnalyzer

} // namespace dvralgo2


///////////////////////////////////////////////////////////////////////////////////////////////////
// Exported function(s).
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace dvralgo
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function creates an instance of camera analyzer.
///
/// \param  cameraNo     the index of a camera.
/// \param  pParameters  pointer to a binary storage of parameter settings.
/// \param  pMask        pointer to a mask of visible (nonzero) and non-visible (zero) points.
/// \param  pOutput      pointer to the message handler.
/// \return              Ok = pointer to the instance of created camera analyzer, otherwise 0.
///////////////////////////////////////////////////////////////////////////////////////////////////
Elvees::ICameraAnalyzer * CreateCameraAnalyzer( sint               cameraNo,
                                                const BinaryData * pParameters,
                                                const Arr2ub     * pMask,
                                                Elvees::IMessage * pOutput, HDC & hdc )
{
  dvralgo2::CameraAnalyzer * p = 0;

  try
  {
    pMask;
    ELVEES_ASSERT( (sizeof(char) == sizeof(BinaryData::value_type)) && (pParameters != 0) );
    p = new dvralgo2::CameraAnalyzer( pOutput );
    ELVEES_ASSERT( p != 0 );

    p->m_hMapImgDll = AfxLoadLibrary( "mapimg.dll" );
    ELVEES_VERIFY( p->m_hMapImgDll != 0, "Failed to load 'mapimg.dll'" );

    p->m_hSRSDll = AfxLoadLibrary( "SRSDllBase.dll" );
    ELVEES_VERIFY( p->m_hSRSDll != 0, "Failed to load 'SRSDllBase.dll'" );

    p->m_CreateImageIndirect =
      (PF_CreateImageIndirect)(GetProcAddress( p->m_hMapImgDll, "CreateImageIndirect" ));
    ELVEES_VERIFY( p->m_CreateImageIndirect != 0, "No access to CreateImageIndirect() function" );

#if 0
    typedef long (__cdecl * PF_GetAnalyzerInterface)( CIAnalyzer** );
    PF_GetAnalyzerInterface pGetAnalyzerInterface =
      (PF_GetAnalyzerInterface)(GetProcAddress( p->m_hSRSDll, "GetAnalyzerInterface" ));
    ELVEES_VERIFY( pGetAnalyzerInterface != 0, "No access to GetAnalyzerInterface() function" );
    pGetAnalyzerInterface( &(p->m_pAnalyzer) );
#else
    typedef long (__cdecl * PF_GetAnalyzerInterfaceExt)( CIAnalyzer**, DWORD );
    PF_GetAnalyzerInterfaceExt pGetAnalyzerInterfaceExt =
      (PF_GetAnalyzerInterfaceExt)(GetProcAddress( p->m_hSRSDll, "GetAnalyzerInterfaceExt" ));
    ELVEES_VERIFY( pGetAnalyzerInterfaceExt != 0, "No access to GetAnalyzerInterfaceExt() function" );
    pGetAnalyzerInterfaceExt( &(p->m_pAnalyzer), GAIE_STATUS0 );
#endif

    ELVEES_VERIFY( p->m_pAnalyzer != 0, "Failed to create CIAnalyzer" );
    const char * iniFile = reinterpret_cast<const char*>( &((*pParameters)[0]) );
    ELVEES_VERIFY( p->m_pAnalyzer->Init( iniFile, 0, -1 ), "Failed to initialize CIAnalyzer" );

    p->m_cameraNo = cameraNo;
    p->m_bGood = true;
    p->m_bReady = true;

    //>>>>>
    p->m_pHdc = &hdc; p->m_pAnalyzer->SetHDC(hdc);
    p->m_pAnalyzer->SwitchPrintBG(-1);
    p->m_pAnalyzer->SwitchPrintBG(-1);
    p->m_pAnalyzer->SwitchPrintBG(-1);
    p->m_pAnalyzer->SwitchPrintBG(-1);
    //>>>>>
  }
  catch (std::runtime_error & e)
  {
    if (pOutput != 0) pOutput->Print( Elvees::IMessage::mt_error, e.what() );
    delete p;
    p = 0;
  }
  return p;
}

} // namespace dvralgo

