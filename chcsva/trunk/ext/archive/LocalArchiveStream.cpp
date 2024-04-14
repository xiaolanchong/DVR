//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Связывает классы ArchiveStream и ArchiveReader в интерфейс IStream
//	для взаимодействия в одном процессе

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.06.2006
*/                                                                                      //
//======================================================================================//
#include "chcs.h"
#include "LocalArchiveStream.h"

#include "../../StreamArchive.h"
#include "../../ArchiveReader.h"

//======================================================================================//
//                               class LocalArchiveStream                               //
//======================================================================================//

LocalArchiveStream::LocalArchiveStream(long lSourceID )
{
	m_pArchiveStream = CStreamArchive::CreatStream(  lSourceID, 0, this );
}

LocalArchiveStream::~LocalArchiveStream()
{
	
}

long LocalArchiveStream::AddRef()
{
	return m_pArchiveStream->AddRef();
}

long	LocalArchiveStream::ReleaseStream()
{
	Elvees::CCriticalSection::Owner _lock( m_SyncStream );
	long cRef = m_pArchiveStream->Release();
	if(!cRef)
	{
		m_pArchiveStream = 0;
	}
	return cRef ;
}

long LocalArchiveStream::Release()
{
	long cRef = ReleaseStream();
	if( cRef == 0 )
	{
		m_pArchiveReader->ShutdownRequests();	
		m_pArchiveReader.reset();
		OutputDebugString(_T("LocalArchiveStream::Release\n"));
//		m_pArchiveReader->Shutdown();
		delete this;
	}
	return cRef;
}

bool LocalArchiveStream::Start()
{
	return m_pArchiveStream->Start();
}

void LocalArchiveStream::Stop()
{
	return m_pArchiveStream->Stop();
}

bool LocalArchiveStream::IsStarted()
{
	return m_pArchiveStream->IsStarted();
}

long LocalArchiveStream::SeekTo(INT64 timePos)
{
	return m_pArchiveStream->SeekTo(timePos);
}

bool LocalArchiveStream::GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout)
{
	return m_pArchiveStream->GetNextFrame( ppFrame, uTimeout );
}

//////////////////////////////////////////////////////////////////////////

bool	LocalArchiveStream::SendCommand( LPCSTR szCommand ) 
{
	return m_pArchiveReader->SendCommand( szCommand );
}

bool	LocalArchiveStream::SendStreamFormat( DWORD dwFCC, BITMAPINFO *pbiInput )
{
	BITMAPINFOHEADER bihOutput = { 0 };

	bihOutput.biSize = sizeof(BITMAPINFOHEADER);
	bihOutput.biBitCount = 24;
	bihOutput.biPlanes = 1;
	bihOutput.biCompression = BI_RGB;
	bihOutput.biWidth  = pbiInput->bmiHeader.biWidth;
	bihOutput.biHeight = pbiInput->bmiHeader.biHeight;

	m_pArchiveStream->InitDecompressor( dwFCC, reinterpret_cast<LPBITMAPINFO>(&bihOutput), pbiInput );
	return true;
}

bool	LocalArchiveStream::SendFrameData(
				const void* pImageData, size_t ImageDataSize, 
				const void* pUserData,  size_t UserDataSize, 
				unsigned uFrameSeq, unsigned uTime 
				)	
{
	Elvees::CCriticalSection::Owner _lock( m_SyncStream );
	if( m_pArchiveStream )
	{
		m_pArchiveStream->DecompressData( 
							pImageData, static_cast<long>(ImageDataSize), 
							uFrameSeq, uTime 
							);
		return m_pArchiveStream->SetCustomData( 
							pUserData, static_cast<long>(UserDataSize), 
							uFrameSeq, uTime 
							);
	}
	else
	{
		return false;
	}
}