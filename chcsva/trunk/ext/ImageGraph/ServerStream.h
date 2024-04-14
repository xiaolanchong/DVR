//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Поток для управления кадрами захвата вне видеоподсистемы, 
//	реализация CHCS::IStream

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   30.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_STREAM_4908927798919533_
#define _SERVER_STREAM_4908927798919533_

#include "FramePin.h"
#include "../common/FpsLogger.h"

//======================================================================================//
//                                  class ServerStream                                  //
//======================================================================================//

//! \brief Поток для управления кадрами захвата вне видеоподсистемы, 
//	реализация CHCS::IStream
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   30.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerStream : public CHCS::IStream, 
					 public IntermediateFilter,
					 boost::noncopyable
{
	Elvees::CCriticalSection m_lastFrameSect;
	long					 m_StreamID;
	FpsLogger				 m_FpsCounter;
public:
	ServerStream(long StreamID, IInputFramePin* pNextFilter);
	virtual ~ServerStream();
private:
	virtual long AddRef();
	virtual long Release();

	virtual bool Start();
	virtual void Stop() ;

	virtual bool IsStarted() ;

	virtual long SeekTo(INT64 timePos);

	// Wait for frame in stream
	// uTimeout = INFINITE not recommended
	virtual bool GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout);

	virtual bool SetCustomFrameData( const void* pData, size_t DataSize ) ;

private:
	virtual void PushImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxFrameSize);

private:

	bool GetLastFrame(CHCS::IFrame** ppFrame);
	void SetLastFrame(CHCS::IFrame* pFrame);

	CHCS::IFrame *m_pFrameLast;
	std::vector<BYTE>			 m_FrameCustomData;

	DWORD m_dwLastUpdated;
	Elvees::CManualResetEvent m_newImage;
};

#endif // _SERVER_STREAM_4908927798919533_