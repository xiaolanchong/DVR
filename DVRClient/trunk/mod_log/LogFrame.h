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
#ifndef _LOG_FRAME_7639541802330485_
#define _LOG_FRAME_7639541802330485_

//======================================================================================//
//                                    class LogFrame                                    //
//======================================================================================//

//! \brief Html log tab frame for DVRClient
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   03.03.2006
//! \version 1.0
//! \bug 
//! \todo 

//class HtmlLog;

class LogFrame : public wxHtmlWindow
{
	ptrdiff_t											m_nCurrentStream;
	std::vector< std::pair< std::string, boost::shared_ptr<Elvees::IMessage> > >	m_MessageStreams;
public:
	LogFrame( wxWindow* pParentWnd );
	virtual ~LogFrame();

	boost::shared_ptr<Elvees::IMessage>	CreateMessageStream( const std::string& sName );

private:
	wxTimer m_timer;
	void OnTimer(wxTimerEvent& event);

	virtual void	OnLinkClicked(const wxHtmlLinkInfo& link);
	void			Refresh();
	std::string		CreateMessageStreamTable() const;

	DECLARE_EVENT_TABLE()
};

#endif // _LOG_FRAME_7639541802330485_