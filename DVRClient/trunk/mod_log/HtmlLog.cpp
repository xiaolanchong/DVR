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

#include "stdafx.h"
#include "HtmlLog.h"

//======================================================================================//
//                                    class HtmlLog                                     //
//======================================================================================//

HtmlLog::HtmlLog(size_t nMaxMessageNumber) :
	m_nMaxMessageNumber( nMaxMessageNumber ) 
{
}

HtmlLog::~HtmlLog()
{
}

void	HtmlLog::Print( sint messageType, const char * szMessage )
{
	boost::mutex::scoped_lock lock(m_SyncMutex);

	if( m_Messages.size() > m_nMaxMessageNumber ) 
	{
		m_Messages.pop_front();
	}
	m_Messages.push_back( boost::make_tuple( messageType, time(0), std::string(szMessage) ) );
}

std::string CanonizeText( const std::string& sText )
{
	std::string str(sText);
	boost::erase_all( str, "\n" );
	boost::erase_all( str, "\r" );
	return str;
}

std::string GetColoredMessage( int nType, const std::string& sArbitratyText )
{
	std::string sMsg;
	std::string sText = CanonizeText(sArbitratyText);
	switch( nType ) 
	{
	case Elvees::IMessage::mt_debug_info :
		sMsg += "<font color=\"gray\">";
		sMsg += sText;
		sMsg += "</font>";
		break;
	case Elvees::IMessage::mt_info:
		sMsg += sText;
		break;
	case Elvees::IMessage::mt_warning:
		sMsg += "<font color=\"blue\">";
		sMsg += sText;
		sMsg += "</font>";
		break;
	case Elvees::IMessage::mt_error:
		sMsg += "<font color=\"red\"><b>";
		sMsg += sText;
		sMsg += "</b></font>";
		break;
	default:
		sMsg += "<font color=\"green\">";
		sMsg += sText;
		sMsg += "</font>";
	}
	return sMsg;
}

std::string	HtmlLog::Dump( ) const
{
	const char* szTableHdr =
		"<table valign=\"top\" >"
		"<tr>"
		"<th>Date</th>"
		"<th>Time</th>"
		"<th>Text</th>"
		"</tr>";

	std::string sTable = szTableHdr;

	boost::mutex::scoped_lock lock(m_SyncMutex);
	for ( std::deque<MessageInfo_t>::const_reverse_iterator it = m_Messages.rbegin(); it != m_Messages.rend(); ++it )
	{
		char szBuf[128];
		
#if _MSC_VER > 1300
		struct tm StubTm;
		struct tm* pTime = & StubTm;
		localtime_s( & StubTm, &boost::get<1>(*it) );
#else
		struct tm* pTime =	localtime( &boost::get<1>(*it) );
#endif
		sTable += "<tr>";

		sTable += "<td>";
		strftime( szBuf, 128, "%d %b %y", pTime  );
		sTable += szBuf;
		sTable += "</td>";

		sTable += "<td>";
		strftime( szBuf, 128, "%H:%M:%S", pTime  );
		sTable += szBuf;
		sTable += "</td>";

		sTable += "<td>";
		sTable += GetColoredMessage( boost::get<0>(*it), boost::get<2>(*it) );
		sTable += "</td>";

		sTable += "</tr>";
	}
	sTable += "</table>";
	return sTable;
}