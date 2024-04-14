//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Debug Elvees::IMessage stdout stream impelmentation

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MESSAGE_STD_OUT_8346930708160720_
#define _MESSAGE_STD_OUT_8346930708160720_

#include "../../../Shared/Interfaces/i_message.h"

//======================================================================================//
//                                 class MessageStdOut                                  //
//======================================================================================//

//! \brief Debug Elvees::IMessage stdout stream impelmentation
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   17.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class MessageStdOut : public Elvees::IMessage
{
	boost::mutex	m_Lock;
public:
	MessageStdOut()
	{
	}

	virtual ~MessageStdOut()
	{
	}

	void MessageStdOut::Print( sint messageType, const char * szMessage )
	{
		boost::mutex::scoped_lock lk(m_Lock);
		const char * szType = "U ";
		switch(messageType) 
		{
		case mt_debug_info: szType = "D "; break;
		case mt_info:		szType = "I "; break;
		case mt_warning:	szType = "W "; break;
		case mt_error:		szType = "E "; break;
		}
		std::cout << "<" << szType << "> " << szMessage << std::endl;
	}
};

#endif // _MESSAGE_STD_OUT_8346930708160720_