//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Стековая обертка над IFrame

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _FRAME_WRAPPER_5338171316499373_
#define _FRAME_WRAPPER_5338171316499373_

#include "../../FrmBuffer.h"
//======================================================================================//
//                                  class FrameWrapper                                  //
//======================================================================================//

//! \brief Стековая обертка над IFrame
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class FrameWrapper : public CHCS::IFrame
{
	CHCS::IFrame*	m_pSourceFrame;
	const void*		m_pData;
	const size_t	m_DataSize;
	const BITMAPINFO* m_pFormat;
public:
	FrameWrapper( CHCS::IFrame* pSourceFrame, const BITMAPINFO* pFormat, const void* pData, size_t DataSize ):
		m_pSourceFrame( pSourceFrame ),
		m_DataSize(DataSize),
		m_pData(pData),
		m_pFormat( pFormat ) 
		{

		}
	virtual ~FrameWrapper()
	{
	}

	virtual long AddRef()
	{
		return 1;
	}
	virtual long Release()
	{
		return 1;
	}

	// StreamID or -1 if undefined
	virtual long GetSourceID()
	{
		return m_pSourceFrame->GetSourceID();
	}

	// Current frame signal detection
	//   0 Horizontal sync is not locked.
	//   1 Horizontal sync is locked.
	//  -1 Can't be determine
	virtual long IsSignalLocked()
	{
		return m_pSourceFrame->IsSignalLocked();
	}

	// Time in ms
	// since stream start (period 2^32 ms ~= 49 days)
	virtual unsigned GetRelativeTime()
	{
		return m_pSourceFrame->GetRelativeTime();
	}

	// Get UNIX-style time in sec
	// See _time64 documentation for details
	virtual INT64 GetTime() 
	{
		return m_pSourceFrame->GetTime();
	}

	// Copy frame format to your buffer
	// If pbi is zero, returns the size of the structure. 
	// If pbi is nonzero, returns -1 if an error occured.
	virtual long GetFormat(BITMAPINFO* pbi)
	{
		size_t cbSize = m_pFormat->bmiHeader.biSize + 
			m_pFormat->bmiHeader.biClrUsed*4;

		if(pbi == NULL)
			return static_cast<long> (cbSize);

		try
		{
			CopyMemory(pbi, m_pFormat, cbSize);
			pbi->bmiHeader.biSizeImage = static_cast<DWORD>(m_DataSize);
		}
		catch(...)
		{
			return -1;
		}

		return 0;
	}

	// Return pointer to frame bytes
	// Valid untill final release
	// NOTE: read only
	virtual const void* GetBytes()
	{
		return m_pData;
	}

	// Draw frame
	virtual bool Draw(HDC hDC, long x, long y, long cx, long cy)
	{
		return false;
	}

	// Copy frame data
	virtual bool CreateCopy(IFrame **ppFrame)
	{
		return false;
	}

	virtual bool GetCustomData( void* pDataBuffer, size_t& nDataSize )
	{
		return m_pSourceFrame->GetCustomData( pDataBuffer, nDataSize );
	}
};
/*
class FrameWrapperFromAnotherSource : public CHCS::IFrame
{
	CHCS::IFrame*	m_pSourceFrame;
//	const void*		m_pData;
//	const size_t	m_DataSize;
	long			m_SourceID;
public:
	FrameWrapperFromAnotherSource( CHCS::IFrame* pSourceFrame, long SourceID):
			m_pSourceFrame( pSourceFrame ),
			m_SourceID(SourceID)
	  {

	  }
	  virtual ~FrameWrapperFromAnotherSource()
	  {
	  }

	  virtual long AddRef()
	  {
		  return m_pSourceFrame->AddRef();
	  }
	  virtual long Release()
	  {
		  long ref = m_pSourceFrame->Release();
		  if(0 == ref)
		  {
			  delete this;
		  }
		  return ref;
	  }

	  // StreamID or -1 if undefined
	  virtual long GetSourceID()
	  {
		  return m_SourceID;
	  }

	  // Current frame signal detection
	  //   0 Horizontal sync is not locked.
	  //   1 Horizontal sync is locked.
	  //  -1 Can't be determine
	  virtual long IsSignalLocked()
	  {
		  return m_pSourceFrame->IsSignalLocked();
	  }

	  // Time in ms
	  // since stream start (period 2^32 ms ~= 49 days)
	  virtual unsigned GetRelativeTime()
	  {
		  return m_pSourceFrame->GetRelativeTime();
	  }

	  // Get UNIX-style time in sec
	  // See _time64 documentation for details
	  virtual INT64 GetTime() 
	  {
		  return m_pSourceFrame->GetTime();
	  }

	  // Copy frame format to your buffer
	  // If pbi is zero, returns the size of the structure. 
	  // If pbi is nonzero, returns -1 if an error occured.
	  virtual long GetFormat(BITMAPINFO* pbi)
	  {
		  return m_pSourceFrame->GetFormat( pbi );
	  }

	  // Return pointer to frame bytes
	  // Valid untill final release
	  // NOTE: read only
	  virtual const void* GetBytes()
	  {
		  return m_pSourceFrame->GetBytes( );
	  }

	  // Draw frame
	  virtual bool Draw(HDC hDC, long x, long y, long cx, long cy)
	  {
		  return m_pSourceFrame->Draw( hDC, x, y, cx, cy );
	  }

	  // Copy frame data
	  virtual bool CreateCopy(IFrame **ppFrame)
	  {
		  return false;
	  }

	  virtual bool GetCustomData( void* pDataBuffer, size_t& nDataSize )
	  {
		  return m_pSourceFrame->GetCustomData( pDataBuffer, nDataSize );
	  }
};*/

class FrameFullStackWrapper : public CHCS::IFrame
{
	const BITMAPINFO*	m_pFormat;
	const void*		m_pImageData;
	const size_t	m_ImageDataSize;
	const void*		m_pUserData;
	const size_t	m_UserDataSize;
	long			m_SourceID;
	INT64			m_Time;
	unsigned		m_RelativeTime;
public:
	FrameFullStackWrapper( 
		long			SourceID,
		const BITMAPINFO*	pFormat,
		const void*		pImageData,
		const size_t	ImageDataSize,
		const void*		pUserData,
		const size_t	UserDataSize,
		INT64			Time,
		unsigned		RelativeTime
		):
			m_pImageData(pImageData),
			m_ImageDataSize(ImageDataSize),
			m_pUserData(pUserData),
			m_UserDataSize(UserDataSize),
			m_SourceID(SourceID),
			m_Time(Time),
			m_RelativeTime(RelativeTime),
			m_pFormat(pFormat)
	  {

	  }
	  virtual ~FrameFullStackWrapper()
	  {
	  }

	  virtual long AddRef()
	  {
		  return 1;
	  }
	  virtual long Release()
	  {
		  return 1;
	  }

	  // StreamID or -1 if undefined
	  virtual long GetSourceID()
	  {
		  return m_SourceID;
	  }

	  // Current frame signal detection
	  //   0 Horizontal sync is not locked.
	  //   1 Horizontal sync is locked.
	  //  -1 Can't be determine
	  virtual long IsSignalLocked()
	  {
		  return -1;
	  }

	  // Time in ms
	  // since stream start (period 2^32 ms ~= 49 days)
	  virtual unsigned GetRelativeTime()
	  {
		  return m_RelativeTime;
	  }

	  // Get UNIX-style time in sec
	  // See _time64 documentation for details
	  virtual INT64 GetTime() 
	  {
		  return m_Time;
	  }

	  // Copy frame format to your buffer
	  // If pbi is zero, returns the size of the structure. 
	  // If pbi is nonzero, returns -1 if an error occured.
	  virtual long GetFormat(BITMAPINFO* pbi)
	  {
		  size_t cbSize = m_pFormat->bmiHeader.biSize + 
			  m_pFormat->bmiHeader.biClrUsed*4;

		  if(pbi == NULL)
			  return static_cast<long> (cbSize);

		  try
		  {
			  CopyMemory(pbi, m_pFormat, cbSize);
			  pbi->bmiHeader.biSizeImage = static_cast<DWORD>(m_ImageDataSize);
		  }
		  catch(...)
		  {
			  return -1;
		  }

		  return 0;
	  }

	  // Return pointer to frame bytes
	  // Valid untill final release
	  // NOTE: read only
	  virtual const void* GetBytes()
	  {
		  return m_pImageData;
	  }

	  // Draw frame
	  virtual bool Draw(HDC hDC, long x, long y, long cx, long cy)
	  {
		  return false;
	  }

	  // Copy frame data
	  virtual bool CreateCopy(IFrame **ppFrame)
	  {
		  return false;
	  }

	  virtual bool GetCustomData( void* pDataBuffer, size_t& nDataSize )
	  {
		  return GetCustomFrameData( m_pUserData, m_UserDataSize, pDataBuffer, nDataSize );
	  }
};

#endif // _FRAME_WRAPPER_5338171316499373_