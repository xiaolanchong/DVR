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
#ifndef _LOCAL_ARCHIVE_STREAM_2656075425610737_
#define _LOCAL_ARCHIVE_STREAM_2656075425610737_

#include "ArchiveDataExchange.h"

class CStreamArchive;
class CArchiveReader;

//======================================================================================//
//                               class LocalArchiveStream                               //
//======================================================================================//

//! \brief Связывает классы ArchiveStream и ArchiveReader в интерфейс IStream
//	для взаимодействия в одном процессе
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class LocalArchiveStream :  public CHCS::IStream,
							public IArchiveDataRequest,
							public IArchiveDataResponse
{
	CStreamArchive*						m_pArchiveStream;
	std::auto_ptr<IArchiveDataRequest>	m_pArchiveReader;
public:
	LocalArchiveStream( long lSourceID );
	virtual ~LocalArchiveStream();

	void SetArchiveReader( std::auto_ptr<IArchiveDataRequest>	pArchiveReader )
	{
		m_pArchiveReader = pArchiveReader;
	}
private:
	virtual bool	SendCommand( LPCSTR szCommand ) ;
private:

	virtual bool	SendStreamFormat( DWORD dwFCC, BITMAPINFO *pbiInput );
	virtual bool	SendFrameData(
		const void* pImageData, size_t ImageDataSize, 
		const void* pUserData,  size_t UserDataSize, 
		unsigned uFrameSeq, unsigned uTime 
		);

	virtual  void	ShutdownRequests()
	{

	}
	virtual  void	Shutdown()
	{

	}

	virtual  bool IsConnected(int )
	{
		return true;
	}
private:

	// IStream
	virtual long AddRef();
	virtual long Release();
	virtual bool Start();
	virtual void Stop();
	virtual bool IsStarted();
	virtual long SeekTo(INT64 timePos);
	virtual bool GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout);

	virtual bool SetCustomFrameData( const void* pData, size_t DataSize )
	{
		return false;
	}

	long	ReleaseStream();
	//! synchronize access to m_pArchiveStream
	Elvees::CCriticalSection	m_SyncStream;
};

#endif // _LOCAL_ARCHIVE_STREAM_2656075425610737_