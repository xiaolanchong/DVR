///////////////////////////////////////////////////////////////////////////////////////////////////
// hall_analyzer.cpp
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

namespace dvralgo2
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class HallAnalyzer.
/// \brief Implementation of hall analyzer.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct HallAnalyzer : public Elvees::IHallAnalyzer
{
  bool               m_bGood;       //!< state flag: non-zero if surveillance is currently successful 
  CCriticalSection   m_dataLocker;  //!< object locks/unlocks the data being transferred between threads
  Elvees::IMessage * m_pOut;        //!< pointer to the message handler
  std::fstream       m_log;         //!< log file stream

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
HallAnalyzer()// : m_parameters( dvralgo2::FIRST_CONTROL_IDENTIFIER )
{
  m_pOut = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Destructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
~HallAnalyzer()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function processes accumulated data obtained from all cameras.
///
/// \param  timeStamp  current time in milliseconds.
/// \return            Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool Process( ulong timeStamp )
{
  CSingleLock lock( &m_dataLocker, TRUE );

  timeStamp;
  try
  {
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
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
      case dvralgo::DATAID_FROM_CAMERA_TO_HALL:
      {
        const dvralgo::TFromCameraToHall * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        p;
      }
      break;

      default: return false;
    }
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
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
  HallAnalyzer * pThis = const_cast<HallAnalyzer*>( this );
  CSingleLock    lock( &(pThis->m_dataLocker), TRUE );

  try
  {
    ELVEES_ASSERT( m_bGood && (pData != 0) );
    switch (pData->type())
    {
      case dvralgo::DATAID_FROM_HALL_TO_CAMERA:
      {
        dvralgo::TFromHallToCamera * p = 0;
        Elvees::DynamicCastPtr( pData, &p );
        ELVEES_ASSERT( p->data.first >= 0 );
        p->data.second.clear();
      }
      break;

      default: return false;
    }
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
    return false;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function prints exception message.
/// 
/// \param  e  pointer to the exception object or 0 (unsupported exception).
///////////////////////////////////////////////////////////////////////////////////////////////////
void PrintException( const std::runtime_error & e ) const
{
  HallAnalyzer * pThis = const_cast<HallAnalyzer*>( this );
  if (m_pOut != 0) m_pOut->Print( Elvees::IMessage::mt_error, e.what() );
  if (m_log.is_open() && m_log.good())
    (pThis->m_log) << std::endl << std::endl << e.what() << std::endl << std::endl;
}

}; // struct HallAnalyzer

} // namespace dvralgo2


///////////////////////////////////////////////////////////////////////////////////////////////////
// Exported function(s).
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace dvralgo
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function creates an instance of hall analyzer.
///
/// \param  pOutput   pointer to the message handler.
/// \return           Ok = pointer to the instance of created hall analyzer, otherwise 0.
///////////////////////////////////////////////////////////////////////////////////////////////////
Elvees::IHallAnalyzer * CreateHallAnalyzer( Elvees::IMessage * pOutput )
{
  dvralgo2::HallAnalyzer * p = new dvralgo2::HallAnalyzer();
  if (p == 0)
    return 0;

  p->m_pOut = pOutput;
  return p;
}

} // namespace dvralgo

