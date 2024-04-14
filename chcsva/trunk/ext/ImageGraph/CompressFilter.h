//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс сжатия изображений XVID кодеком

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _COMPRESS_FILTER_2778339831433444_
#define _COMPRESS_FILTER_2778339831433444_

#include "FramePin.h"
#include "../common/FpsLogger.h"

struct EncoderParam
{
	DWORD				m_CodecFCC;
	long				m_lVideoFPM;
	long				m_lCodecQuality;
	long				m_lCodecKeyRate;
	long				m_lCodecDataRate;
	std::vector<BYTE>	m_CodecStateData;
};

#undef  USE_THREAD_FOR_COMPRESS
//======================================================================================//
//                                 class CompressFilter                                 //
//======================================================================================//

//! \brief Класс сжатия изображений XVID кодеком
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class CompressFilter :	public IntermediateFilter
{
	long			m_lLastFrame;
	long			m_StreamID;
	unsigned int	m_uTimeStart;
	EncoderParam	m_EncoderParam;
	std::auto_ptr<CVidEncoder>		m_pEncoder;

	long		GetID() const
	{
		return m_StreamID;
	}

//	time_t			m_LastRestartedTime;
#ifdef USE_THREAD_FOR_COMPRESS
	std::auto_ptr<boost::thread>	m_WorkingThread;
	Elvees::CCriticalSection m_LastFrameSync;
	Elvees::CEvent	m_NewFrameArrivedEvent;
	Elvees::CManualResetEvent	m_ShutdownEvent;
#endif
//	FpsLogger		m_FpsCounter;

	CHCS::IFrame*	m_pLastFrame;

	void	CompressFrame(CHCS::IFrame* pFrame);
	void	CompressFrame();
#ifdef USE_THREAD_FOR_COMPRESS
	void	ThreadProc();
#endif
public:
	CompressFilter(
		long				StreamID,
		const EncoderParam& param,
		IInputFramePin*		p = NULL
		);
	virtual ~CompressFilter();

private:
	virtual void	PushImage( CHCS::IFrame* pLastFrame, bool bKeyFrame, size_t MaxImageSize );
};

#endif // _COMPRESS_FILTER_2778339831433444_