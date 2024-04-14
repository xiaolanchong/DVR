//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Поиск потока по идентификаторам

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   30.06.2006
*/                                                                                      //
//======================================================================================//
#include "chcs.h"
#include "StreamList.h"
//======================================================================================//
//                                   class StreamList                                   //
//======================================================================================//

void	StreamList::Push( const StreamInfo& NewStream )
{
	Elvees::CCriticalSection::Owner lock(m_listSection);
	m_StreamList.push_back( NewStream );
}

void	StreamList::Clear()
{
	Elvees::CCriticalSection::Owner lock(m_listSection);
	m_StreamList.clear();
}

CHCS::IStream* StreamList::StreamByID(long lStreamID)
{
	Elvees::CCriticalSection::Owner lock(m_listSection);
	for( size_t i = 0; i < m_StreamList.size(); ++i )
	{
		if(m_StreamList[i].m_StreamID == lStreamID)
		{
			m_StreamList[i].m_pStream->AddRef();
			return m_StreamList[i].m_pStream.get();
		}
	}
	return NULL;
}

CHCS::IStream* StreamList::StreamByUID(const UUID& StreamUID, long lPin)
{
	Elvees::CCriticalSection::Owner lock(m_listSection);
	for( size_t i = 0; i < m_StreamList.size(); ++i )
	{
		if( m_StreamList[i].m_StreamUID		== StreamUID && 
			m_StreamList[i].m_DevicePin		== lPin)
		{
			m_StreamList[i].m_pStream->AddRef();
			return m_StreamList[i].m_pStream.get();
		}
	}
	return NULL;
}

bool StreamList::GetStreams( std::vector<long>& StreamIDs )
{
	Elvees::CCriticalSection::Owner lock(m_listSection);
	StreamIDs.clear();
	for( size_t i = 0; i < m_StreamList.size(); ++i )
	{
		long StreamID = m_StreamList[i].m_StreamID;
		StreamIDs.push_back( StreamID );
	}
	return true;
}