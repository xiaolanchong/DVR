//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Êëàññ ñæàòèÿ èçîáðàæåíèé XVID êîäåêîì

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "CompressFilter.h"
#include "FrameWrapper.h"

const bool c_bWriteArchive = true;

CVidEncoder* CreateCompressor( BITMAPINFO* lbiInput, const EncoderParam& param)
{
	bool bOK = false;
	CVidEncoder* pEncoder = NULL;

	if(param.m_CodecFCC == 0)
	{
		static bool bShowError = true;

		if(bShowError)
		{
			Elvees::Output(Elvees::TCritical, TEXT("No codec selected!"));
			bShowError = false;
		}

		return NULL;
	}

	do
	{
		pEncoder = new CVidEncoder(param.m_CodecFCC);
		if(!pEncoder)
			break;

		const BYTE* pCodecState		= 0;
		size_t		CodecStateSize	= 0;
		if( param.m_CodecStateData.size() )
		{
			pCodecState		= &param.m_CodecStateData[0];
			CodecStateSize	= param.m_CodecStateData.size();
		}
		else
		{
			Elvees::Output( Elvees::TError, _T("Empty codec settings, CompressFilter will not be created")  );
			return 0;
		}

		pEncoder->InitCompressor(lbiInput,
			param.m_lVideoFPM > 0 ? 60000000/param.m_lVideoFPM : 40000,
			param.m_lCodecQuality, param.m_lCodecKeyRate, param.m_lCodecDataRate, 
			const_cast<BYTE*>(pCodecState), 
			static_cast<long>( CodecStateSize ) );

		if(!pEncoder->IsCompressionInited())
			break;

		pEncoder->StartCompression();

		if(!pEncoder->IsCompressionStarted())
			break;

		bOK = true;

		Elvees::OutputF( Elvees::TInfo, _T("Create compressor: %ld fps"), long(param.m_lVideoFPM/60)  );
	}
	while(false);

	if(!bOK)
	{
		delete pEncoder;
		pEncoder = NULL;
	}

	return pEncoder;
}

//======================================================================================//
//                                 class CompressFilter                                 //
//======================================================================================//

CompressFilter::CompressFilter(
		long		 StreamID,
		const EncoderParam& param,
		IInputFramePin* p
		):
	IntermediateFilter(p),
	m_EncoderParam (param),
	m_lLastFrame(-1),
	m_StreamID(StreamID),
	m_uTimeStart(0),
//	m_LastRestartedTime( time(0) ),
#ifdef USE_THREAD_FOR_COMPRESS
	m_NewFrameArrivedEvent( NULL, FALSE, FALSE ),
#endif
	m_pLastFrame(0)
//	m_FpsCounter( 5 * 60 )
{
#ifdef USE_THREAD_FOR_COMPRESS
	m_WorkingThread.reset( new boost::thread( boost::bind( &CompressFilter::ThreadProc, this ) ) );
#endif
}

CompressFilter::~CompressFilter()
{
#ifdef USE_THREAD_FOR_COMPRESS
	m_ShutdownEvent.Set();
	if( m_WorkingThread.get() )
	{
		m_WorkingThread->join();
	}
#endif
	if( m_pLastFrame )
	{
		m_pLastFrame->Release();
		m_pLastFrame = 0;
	}
}

void	CompressFilter::PushImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxImageSize )
{
	pFrame->AddRef();
#ifdef USE_THREAD_FOR_COMPRESS
	Elvees::CCriticalSection::Owner _lk( m_LastFrameSync );
#endif
	if( m_pLastFrame )
	{
		m_pLastFrame->Release();
		m_pLastFrame = 0;
	}
	m_pLastFrame = pFrame;
#ifdef USE_THREAD_FOR_COMPRESS
	m_NewFrameArrivedEvent.Set();
#else
	CompressFrame();
#endif
}

void	CompressFilter::CompressFrame()
{
	CompressFrame( m_pLastFrame );
#ifdef USE_THREAD_FOR_COMPRESS
	Elvees::CCriticalSection::Owner _lk( m_LastFrameSync );
#endif
	m_pLastFrame->Release();
	m_pLastFrame = 0;
}

#ifdef USE_THREAD_FOR_COMPRESS
void	CompressFilter::ThreadProc()
{
	HANDLE Events [2] =
	{
		m_NewFrameArrivedEvent.GetEvent(),
		m_ShutdownEvent.GetEvent()
	};

	do
	{
		DWORD res = WaitForMultipleObjects( 2, Events, FALSE, INFINITE );
		if( WAIT_OBJECT_0 == res )
		{
			CompressFrame();
		}
		else
		{
			break;
		}
	}
	while (true);
}
#endif

void	CompressFilter::CompressFrame(CHCS::IFrame* pFrame)
{
	long     lFrame, lSize;
	unsigned uTime;

	if( m_uTimeStart == 0 )
	{
		m_uTimeStart = pFrame->GetRelativeTime();
	}

	uTime  = pFrame->GetRelativeTime() - m_uTimeStart;
	lFrame = MulDiv(uTime, m_EncoderParam.m_lVideoFPM, 60000);

	if(lFrame <= m_lLastFrame)
		return;
/*
	if( time(0) - m_LastRestartedTime > 1 * 60 * 60  )
	{
		m_pEncoder.reset();
		m_LastRestartedTime = time(0);
		Elvees::Output( Elvees::TInfo, _T("Restarting encoder") );
	}*/
	//////////////////////////////////////////////////////////////////////////
	// Normalize image

#ifdef ENABLE_RESIZE
	const BYTE* pFrameBytes = reinterpret_cast<const BYTE*>(pFrame->GetBytes());
	BITMAPINFO biFrame;
	pFrame->GetFormat(&biFrame);

	for ( LONG nHeight =0; nHeight < m_bihOut.biHeight; ++nHeight )
	{
		for ( LONG nWidth =0; nWidth < m_bihOut.biWidth; ++nWidth )
		{
			m_pFrameOut[ nHeight * m_bihOut.biWidth + nWidth ] = 
				pFrameBytes[ c_nShrinkFactor * nHeight * biFrame.bmiHeader.biWidth + c_nShrinkFactor * nWidth ];
		}
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Create encoder

	if(m_pEncoder.get() == NULL)
	{
#ifndef ENABLE_RESIZE
		BITMAPINFO biFrame;
		pFrame->GetFormat(&biFrame);
		m_pEncoder.reset( CreateCompressor(&biFrame, m_EncoderParam) );
#else		
		m_pEncoder = CreateCompressor((LPBITMAPINFO)&m_bihOut, m_EncoderParam);
#endif

	//	m_FramesPerMinute = pManager->GetVideoRate();
	//	m_lVMS = pManager->GetVideoMaxSize();

		m_EncoderParam.m_lVideoFPM = m_EncoderParam.m_lVideoFPM > 0 ? m_EncoderParam.m_lVideoFPM : 60;
	}

	if(m_pEncoder.get() == NULL)
		return;

	//////////////////////////////////////////////////////////////////////////

	bool bKey  = true;
	void* pCompressed = NULL;
	unsigned uFrameNum;

	lSize = 0;

	if( c_bWriteArchive )
	{

		pCompressed = m_pEncoder->CompressData(
#ifndef ENABLE_RESIZE
			pFrame->GetBytes(),
#else
			m_pFrameOut,
#endif
			&lSize, &bKey, &uFrameNum);

		if(pCompressed)
		{
			if(m_lLastFrame < 0)
				m_uTimeStart = pFrame->GetRelativeTime();

			m_lLastFrame = lFrame;
		}
		else
		{
			DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Compress frame[%ld] failed"), GetID()));
			return;
		}
	}
/*	m_FpsCounter.IncrementFrame();

	boost::optional<double > fps = m_FpsCounter.GetFPS();
	if( fps.get_ptr() )
	{
		Elvees::OutputF( Elvees::TInfo, _T("[%ld]Compressor fps=%.1lf"), m_StreamID, fps.get() );
	}*/
	//////////////////////////////////////////////////////////////////////////
	BITMAPINFO bi;
	m_pEncoder->GetOutFormat(&bi);

	FrameWrapper fwrp( pFrame, &bi, pCompressed, lSize  );
	SendImage( &fwrp, bKey, m_pEncoder->GetMaxPackedSize()  );
}