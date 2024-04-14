///////////////////////////////////////////////////////////////////////////////////////////////////
// exception.cpp
// ---------------------
// begin     : Nov 2006
// modified  : 10 Feb 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace alib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function throws std::runtime_error supplied with file and code line information.
///
/// \param  reason  the text of exception reason or 0.
/// \param  file    the name of a file where exception has occurred or 0.
/// \param  line    the line of code where exception has occurred.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ThrowStdException( const char * reason, const char * file, int line )
{
  char lineText[64];
  sprintf( lineText, "%d\n\n", line );
  std::string text;
  ((text += "EXCEPTION:\nreason:\t") += ((reason != 0) ? reason : "...")) += "\nfile:\t";
  ((text += ((file != 0) ? file : "...")) += "\nline:\t") += lineText;
  throw std::runtime_error( text );
}

} // namespace alib

