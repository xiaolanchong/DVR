///////////////////////////////////////////////////////////////////////////////////////////////////
// console_message.cpp
// ---------------------
// begin     : 2006
// modified  : 24 Jan 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../../../Shared/Common/elvees.h"
#include "console_imessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function prints a message.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ConsoleMessage::Print( sint messageType, const char * szMessage )
{
  switch (messageType)
  {
    case Elvees::IMessage::mt_debug_info:
      std::cout << std::endl << "DEBUG_INFO:" << std::endl;
      break;
    case Elvees::IMessage::mt_error:
      std::cout << std::endl << "ERROR:" << std::endl;
      break;
    case Elvees::IMessage::mt_info:
      std::cout << std::endl << "INFO:" << std::endl;
      break;
    case Elvees::IMessage::mt_warning:
      std::cout << std::endl << "WARNING:" << std::endl;
      break;
    default:
      std::cout << std::endl << "MESSAGE OF UNKNOWN TYPE:" << std::endl;
  }
  std::cout << ((szMessage != 0) ? szMessage : "...") << std::endl << std::endl;
}

} // namespace Elvees

