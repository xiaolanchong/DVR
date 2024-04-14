//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Html log tab frame for DVRClient

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   03.03.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "LogFrame.h"
#include "HtmlLog.h"

const int TIMER_ID = 0xfef1;

//======================================================================================//
//                                    class LogFrame                                    //
//======================================================================================//

LogFrame::LogFrame( wxWindow* pParentWnd ) : 
	wxHtmlWindow(pParentWnd),
	m_timer( this, TIMER_ID ),
	m_nCurrentStream(-1)
{
	//! обновление сообщений 1 раз/сек 
	m_timer.Start( 1000 );
}

LogFrame::~LogFrame()
{
}

void LogFrame::OnTimer(wxTimerEvent& event)
{
	Refresh();
}

std::string		LogFrame::CreateMessageStreamTable() const
{
	std::string sTable =
		"<table valign=\"top\" >"
		"<tr>";
	
	for (size_t i = 0; i < m_MessageStreams.size(); ++i)
	{
		sTable += "<td>";
		if( ptrdiff_t(i) != m_nCurrentStream )
		{
			//! номер потока сохраним в ссылке и достанем в OnLinkClicked, ечли он будет выбран
			boost::format f("%d");
			sTable += "<a href=\"";
			sTable += (f % i).str();
			sTable += "\">";
			sTable += m_MessageStreams[i].first;
			sTable += "</a>";
		}
		else
		{
			sTable += m_MessageStreams[i].first;
		}
		sTable += "</td>";
	}
	sTable += "</tr></table>";
	return sTable;
}

void	LogFrame::Refresh()
{
	if( m_nCurrentStream <0 || m_MessageStreams.empty() ) return;
	_ASSERTE( m_nCurrentStream < static_cast<ptrdiff_t>( m_MessageStreams.size() ));

	std::string sAll = 
		"<html>"
		"<META HTTP-EQUIV=\"Content-Type\"	CONTENT=\"text/html; CHARSET=Windows-1251\">"
		"<head>"
		"<title>DVRClient log</title>"
		"</head>"
		"<body>";
	sAll += CreateMessageStreamTable();
	HtmlLog* pHtmlLog = dynamic_cast<HtmlLog*>(m_MessageStreams[ m_nCurrentStream ].second.get());
	_ASSERTE(pHtmlLog);
	std::string sTable = pHtmlLog->Dump();

	sAll += sTable;
	sAll += 
		"</body>"
		"</html>";
	SetPage( sAll.c_str() );
}

boost::shared_ptr<Elvees::IMessage>	LogFrame::CreateMessageStream( const std::string& sName )
{
	if( m_nCurrentStream < 0  && m_MessageStreams.empty())
	{
		m_nCurrentStream = 0;
	}
	boost::shared_ptr<Elvees::IMessage> p( new HtmlLog );
	m_MessageStreams.push_back( std::make_pair( sName, p  ) );
	return p;
}

void LogFrame::OnLinkClicked(const wxHtmlLinkInfo& link)
{
	//! тут достаем что создали в CreateMessageStreamTable
	wxString s = link.GetHref();
	unsigned long nID;
	bool bConvertionRes = s.ToULong( &nID );
	_ASSERTE(bConvertionRes);
	if( bConvertionRes )
	{
		m_nCurrentStream = nID;
		Refresh();
	}
}

BEGIN_EVENT_TABLE(LogFrame, wxHtmlWindow)
	EVT_TIMER( TIMER_ID, LogFrame::OnTimer )
END_EVENT_TABLE()