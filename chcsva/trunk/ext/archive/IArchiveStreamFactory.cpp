//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Абстрактная фабрика создания интерфейсного класса потока

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.06.2006
*/                                                                                      //
//======================================================================================//
#include "chcs.h"
#include "IArchiveStreamFactory.h"
#include "LocalArchiveStream.h"

class LocalArchiveAdapter : public IArchiveDataRequest
{
	//	IArchiveDataResponse*			m_pResponse;
	std::auto_ptr< CArchiveReader >	m_pArchiveReader;
public:
	LocalArchiveAdapter( 
		IArchiveDataResponse* pResponse, 
		const std::tstring& stBaseArchivePath,
		INT64 startPos
		):
	  m_pArchiveReader( new CArchiveReader(  pResponse, stBaseArchivePath ) )
	  {
		  m_pArchiveReader->Start();
	  }

	  ~LocalArchiveAdapter()
	  {
		  m_pArchiveReader->Shutdown();
	  }

	  virtual bool	SendCommand( LPCSTR szCommand )
	  {
		  m_pArchiveReader->ProcessCommand( szCommand, strlen( szCommand ) );
		  return true;
	  }
};

class RemoteArchiveAdapter :	public IArchiveDataRequest
{
	ArchiveClientConnection	m_conn;



public:
	RemoteArchiveAdapter( IArchiveDataResponse* pResponse,  long lSourceID,  unsigned long address):
		m_conn( lSourceID, InternetAddress( ntohl(address), c_ArchiveServerPort ), pResponse ) 
	{
		m_conn.Start();
	}
	virtual bool	SendCommand( LPCSTR szCommand )
	{
/*		bool res = SendCommandsInQueue();
		if( res )
		{
			res = m_conn.SendCommand( szCommand );
		}
		if(!res)
		{
			m_CommandQueue.push( szCommand );
		}
		return res;*/
		return m_conn.SendCommand( szCommand );
	}

	~RemoteArchiveAdapter()
	{
		m_conn.Stop();
	}

	virtual void ShutdownRequests()
	{
		static_cast<IArchiveDataRequest&>(m_conn).ShutdownRequests();
	}
};

//======================================================================================//
//                             class IArchiveStreamFactory                              //
//======================================================================================//

IArchiveStreamFactory::IArchiveStreamFactory()
{
}

IArchiveStreamFactory::~IArchiveStreamFactory()
{
}

//////////////////////////////////////////////////////////////////////////


class LocalArchiveStreamFactory : public IArchiveStreamFactory
{
	CHCS::IStream* CreateArchiveStream(
		long lSourceID, 
		INT64 startPos, 
		INT64 endPos,
		unsigned long address,
		const std::tstring& sVideoBasePath
		);

	std::auto_ptr<IArchiveDataRequest> CreateArchiveProvider(
		IArchiveDataResponse* pResponse,
		long lSourceID, 
		INT64 startPos, 
		INT64 endPos,
		unsigned long address,
		const std::tstring& sVideoBasePath
		);
};

CHCS::IStream* LocalArchiveStreamFactory::CreateArchiveStream(
	long lSourceID, 
	INT64 startPos, 
	INT64 endPos,
	unsigned long address,
	const std::tstring& sVideoBasePath
	)
{
	LocalArchiveStream* pLocalStream = new LocalArchiveStream( lSourceID );
	std::auto_ptr<IArchiveDataRequest> prv = 
		CreateArchiveProvider (
			pLocalStream,
			lSourceID, 
			startPos, 
			endPos,
			address,
			sVideoBasePath		
		);
	pLocalStream->SetArchiveReader( prv );
	CHCS::IStream * pStream = pLocalStream;
	pStream->Start();
	pStream->SeekTo( startPos );
	return pStream;
}

std::auto_ptr<IArchiveDataRequest>  LocalArchiveStreamFactory::CreateArchiveProvider(
									IArchiveDataResponse* pResponse,
									long lSourceID, 
									INT64 startPos, 
									INT64 endPos,
									unsigned long address,
									const std::tstring& sVideoBasePath )
{
	return std::auto_ptr<IArchiveDataRequest>( 
#if 0
		new LocalArchiveAdapter ( pResponse, sVideoBasePath, startPos )
#else
		new RemoteArchiveAdapter( pResponse, lSourceID, address  ) 
#endif
		);
}

IArchiveStreamFactory* GetArchiveStreamFactory()
{
	static LocalArchiveStreamFactory f;
	return &f;
}