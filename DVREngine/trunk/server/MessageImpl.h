//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Debug Elvees::IMessage impelmentation

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MESSAGE_IMPL_6162294121357440_
#define _MESSAGE_IMPL_6162294121357440_

#include "..\..\..\Shared\Interfaces\i_message.h"
//======================================================================================//
//                                  class MessageImpl                                   //
//======================================================================================//

//! \brief обертка над Elvees::IMessage для удобства (boost::format)
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.04.2006
//! \version 1.1
//! \bug 
//! \todo 

template<typename T> class MessageImplTmpl
{
	//	boost::shared_ptr<Elvees::IMessage>	m_pMsg;
	typename T						m_pMsg;
public:
	MessageImplTmpl( T pMsg = T()  ) :
	  m_pMsg( pMsg ){} 
	~MessageImplTmpl() {};

	void	Print( sint messageType, const char * szMessage )  
	{
		if( m_pMsg == T() )
		{
#ifndef UNIX_RELEASE
			OutputDebugStringA( szMessage );
			OutputDebugStringA( "\n" );
#endif
		}
		else m_pMsg->Print( messageType, szMessage );
	}
	void	Print( sint messageType, const std::string& sMessage )
	{
		Print( messageType, sMessage.c_str() );
	}

	void	Print( sint messageType, const boost::format& sFmtMessage )
	{
		Print( messageType, sFmtMessage.str() );
	}

	void	Set( T pMsg = T() )
	{
		m_pMsg = pMsg;
	}

	T Get() const { return m_pMsg; }
};

typedef MessageImplTmpl< boost::shared_ptr<Elvees::IMessage> >	MessageImpl;
typedef MessageImplTmpl< Elvees::IMessage* >					MessageImplRaw;

#endif // _MESSAGE_IMPL_6162294121357440_