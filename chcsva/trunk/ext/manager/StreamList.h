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
#ifndef _STREAM_LIST_1611226759789238_
#define _STREAM_LIST_1611226759789238_
//======================================================================================//
//                                   class StreamList                                   //
//======================================================================================//

//! \brief Поиск потока по идентификаторам
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   30.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class StreamList
{
public:
	struct StreamInfo
	{
		long			m_StreamID;
		UUID			m_StreamUID;
		std::tstring	m_stMoniker;
		bool			m_bIsLocal;
		long			m_DevicePin;
		boost::shared_ptr<CHCS::IStream> m_pStream;
	};
private:
	Elvees::CCriticalSection m_listSection;
	std::vector< StreamInfo >	m_StreamList;
public:

	void	Push( const StreamInfo& NewStream );
	void	Clear();

	CHCS::IStream* StreamByID(long lStreamID);
	CHCS::IStream* StreamByUID(const UUID& StreamUID, long lPin = 0);

	bool GetStreams( std::vector<long>& StreamIDs );

	const std::vector< StreamInfo >& GetStreamInfoArr() const
	{
		return m_StreamList;
	}
};

#endif // _STREAM_LIST_1611226759789238_