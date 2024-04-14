//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Handler for html message log - create temp file and dump all messages

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   03.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _HTML_LOG_4065029808043266_
#define _HTML_LOG_4065029808043266_
/*
class ILogWriter
{
public:
	virtual ~ILogWriter(){}

	void	WriteDebug( time_t nTime, const std::string& sText );
	void	WriteInfo( time_t nTime, const std::string& sText );
	void	WriteWarning( time_t nTime, const std::string& sText );
	void	WriteError( time_t nTime, const std::string& sText );
	void	WriteUnknown( time_t nTime, const std::string& sText );
};
*/

//======================================================================================//
//                                    class HtmlLog                                     //
//======================================================================================//

//! \brief Handler for html message log - create temp file and dump all messages
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   03.03.2006
//! \version 1.0
//! \bug 
//! \todo 

class HtmlLog : public Elvees::IMessage
{
public:
	enum MsgType
	{
		MsgDebug,
		MsgInfo,
		MsgWarning,
		MsgError,
		MsgUnknown
	};

	HtmlLog(size_t nMaxMessageNumber = 100);
	virtual ~HtmlLog();

	virtual void Print( sint messageType, const char * szMessage ) ;

	void		SetMaxMessageNumber(size_t nMaxMessageNumber) { m_nMaxMessageNumber = nMaxMessageNumber; }
//	void		AddMessage( MsgType nType, const std::string& sText );
	std::string	Dump( ) const;
private:
	typedef boost::tuple< int , time_t, std::string>		MessageInfo_t;
	std::deque<MessageInfo_t >								m_Messages;
	size_t													m_nMaxMessageNumber;
	mutable boost::mutex									m_SyncMutex;

	void OnTimer(wxTimerEvent& event);
};

#endif // _HTML_LOG_4065029808043266_