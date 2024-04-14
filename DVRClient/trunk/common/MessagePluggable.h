//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация IMessage, декоратор другой реализации, можно менять на лету

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   19.04.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MESSAGE_PLUGGABLE_5445105046902514_
#define _MESSAGE_PLUGGABLE_5445105046902514_

//======================================================================================//
//                                class MessagePluggable                                //
//======================================================================================//

//! \brief Реализация IMessage, декоратор другой реализации, можно менять на лету
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   19.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class MessagePluggable: public Elvees::IMessage
{
	boost::mutex							m_Lock;
	boost::shared_ptr<Elvees::IMessage>		m_pMsg;
	std::vector< std::pair<sint, std::string> > m_Cache;
public:
	MessagePluggable() 
	{
	}

	virtual void Print( sint messageType, const char * szMessage )
	{
		boost::mutex::scoped_lock l( m_Lock );
		if(m_pMsg.get())
		{
			m_pMsg->Print( messageType, szMessage );
		}
		else
		{
			m_Cache.push_back( std::make_pair( messageType, szMessage ) );
		}
	}

	void Set( boost::shared_ptr<Elvees::IMessage> pMsg )
	{
		{
		boost::mutex::scoped_lock l( m_Lock );
		m_pMsg = pMsg;
		}
		//std::for_each( m_Cache.begin(), m_Cache.end(), boost::mem_fn(  ) )
		for ( size_t i = 0; i < m_Cache.size(); ++i )
		{
			Print( m_Cache[i].first, m_Cache[i].second.c_str() );
		}
		m_Cache.clear();
	}
};


#endif // _MESSAGE_PLUGGABLE_5445105046902514_