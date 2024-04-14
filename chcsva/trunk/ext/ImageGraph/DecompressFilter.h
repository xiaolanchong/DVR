//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Фильтр декомпрессора потока сжатых изображений
//	+клиентский поток

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   03.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _DECOMPRESS_FILTER_9319433708643786_
#define _DECOMPRESS_FILTER_9319433708643786_

#include "FramePin.h"
#include "../../FrmBuffer.h"

//======================================================================================//
//                                class DecompressFilter                                //
//======================================================================================//

//! \brief Фильтр декомпрессора потока сжатых изображений
//	+клиентский поток
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   03.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class DecompressFilter : public CHCS::IStream ,
						 public IInputFramePin,
						 boost::noncopyable,
						 CFrmBuffer::Allocator
{
	long	m_StreamID;
	INT64 m_LastFrameTime;
public:
	DecompressFilter(long StreamID);
	virtual ~DecompressFilter();

private:
	virtual long AddRef();
	virtual long Release();

	virtual bool Start();
	virtual void Stop() ;

	virtual bool IsStarted() ;

	virtual long SeekTo(INT64 timePos);

	// Wait for frame in stream
	// uTimeout = INFINITE not recommended
	virtual bool GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout)
	{
		return GetLastFrame(ppFrame);
	}

	virtual bool SetCustomFrameData( const void* pData, size_t DataSize ) 
	{
		return false;
	}
private:
	virtual long AllocatorSourceID()
	{
		return m_StreamID;
	}
private:
	void	PushImage( CHCS::IFrame* pFrame, bool , size_t);
private:

	//! for SetLastFrame/GetLastFrame
	Elvees::CCriticalSection m_lastFrameSect;

	//! for PushImage single entance
	Elvees::CCriticalSection m_ReentranceSection;

	void SetLastFrame(CHCS::IFrame* pFrame);
	bool GetLastFrame(CHCS::IFrame** ppFrame);

	CVidDecoder	m_Decoder;
	CHCS::IFrame *m_pFrameLast;
};

#endif // _DECOMPRESS_FILTER_9319433708643786_