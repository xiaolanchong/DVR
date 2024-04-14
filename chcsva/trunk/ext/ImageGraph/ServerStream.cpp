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
#include "chcs.h"
#include "ServerStream.h"
#include "../../FrmBuffer.h"
#include "FrameWrapper.h"
//======================================================================================//
//                                  class ServerStream                                  //
//======================================================================================//

ServerStream::ServerStream(long StreamID, IInputFramePin* pNextFilter):
	IntermediateFilter( pNextFilter) ,
	m_StreamID(StreamID),
	m_dwLastUpdated (0),
	m_pFrameLast ( NULL ),
	m_FpsCounter( 300 ) 
{
}

ServerStream::~ServerStream()
{
	if(m_pFrameLast)
		m_pFrameLast->Release();
}

long ServerStream::AddRef()
{
	// no external reference control
	return 1;
}

long ServerStream::Release()
{
	// no external reference control
	return 1;
}

bool ServerStream::Start()
{
	return true;
}

void ServerStream::Stop() 
{

}

bool ServerStream::IsStarted()
{
	// FIXME - CDevice query required
	return true;
}

long ServerStream::SeekTo(INT64 timePos)
{
	return 0;
}

// Wait for frame in stream
// uTimeout = INFINITE not recommended
bool ServerStream::GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout)
{
	DWORD dwTime = timeGetTime();

	if(dwTime - m_dwLastUpdated > 5000)
	{
		m_dwLastUpdated = dwTime;
	}
	else
	{
		m_newImage.Wait(min(40, uTimeout));
	}

	return GetLastFrame(ppFrame);
}



bool ServerStream::SetCustomFrameData( const void* pData, size_t DataSize )
{
	Elvees::CCriticalSection::Owner lock(m_lastFrameSect);

	if( !DataSize )
	{
		m_FrameCustomData.clear();
	}
	else
	{
		m_FrameCustomData.assign( static_cast<const BYTE* >(pData), 
			static_cast<const BYTE* >(pData) + DataSize );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Last frame
//////////////////////////////////////////////////////////////////////

void ServerStream::SetLastFrame(CHCS::IFrame* pFrame)
{
	if(!pFrame)
		return;

	Elvees::CCriticalSection::Owner lock(m_lastFrameSect);

	if(m_pFrameLast)
		m_pFrameLast->Release();

#if 0 //test code
	char szBuf[255];
	__time64_t t = pFrame->GetTime();
	tm tt;
	_localtime64_s( &tt, &t );
	asctime_s (  szBuf, 255, &tt );
	m_FrameCustomData.assign( szBuf, szBuf + strlen(szBuf) );
#endif
	// hack - we know here about CFrmBuffer object
	CFrmBuffer* pFrameBuffer = dynamic_cast<CFrmBuffer* >( pFrame );
	_ASSERT(pFrameBuffer);
	if( !m_FrameCustomData.empty() )
	{
		pFrameBuffer->SetCustomData( &m_FrameCustomData[0], m_FrameCustomData.size() );
	}
	m_pFrameLast =  pFrame;
	pFrameBuffer->SetSourceID( m_StreamID ); 

	m_pFrameLast->AddRef() ;
	SendImage( m_pFrameLast, 0, false );
#if 1
	m_FpsCounter.IncrementFrame();
	boost::optional<double> fps = m_FpsCounter.GetFPS();
	if( fps.get_ptr() )
	{
		Elvees::OutputF( Elvees::TInfo, _T("[%ld]fps=%.1lf"), m_StreamID, fps.get() );
	}
#endif
}

bool ServerStream::GetLastFrame(CHCS::IFrame** ppFrame)
{
	if(!ppFrame)
		return false;

	Elvees::CCriticalSection::Owner lock(m_lastFrameSect);

	if(m_pFrameLast)
	{
		m_pFrameLast->AddRef();
		*ppFrame = m_pFrameLast;
		return true;
	}

	return false;
}

void ServerStream::PushImage( CHCS::IFrame* pFrame, bool bKeyFrame, size_t MaxFrameSize)
{
	if(!pFrame) return;

	m_dwLastUpdated = timeGetTime();

	pFrame->AddRef();

	SetLastFrame(pFrame);
	m_newImage.Pulse();

	pFrame->Release();
}