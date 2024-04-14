///////////////////////////////////////////////////////////////////////////////////////////////////
// ini_file_parser.cpp
// ---------------------
// begin     : Dec 2004
// modified  : 25 Dec 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
IniFileParser::IniFileParser()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function initializes this object.
///
/// \param  fileName  the name of a source ini-file.
/// \return           Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool IniFileParser::Initialize( const char * fileName )
{
  if (fileName == 0)
    return false;
  m_file.open( fileName, std::ios::in );
  m_lexeme.reserve( 1<<8 );
  return (m_file.good());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function determines whether to proceed the parsing loop.
/// 
/// \return  nonzero if the parsing loop must be proceeded.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool IniFileParser::Proceed()
{
  const char COMMENT = '#';
  const char ENDL = '\n';

  while (m_file.good() && !(m_file.eof()))
  {
    m_lexeme.clear();
    m_file >> std::skipws >> m_lexeme;
    if (m_lexeme.empty())
      continue;
 
    if (m_lexeme[0] == COMMENT)
    {
      while (m_file.good() && !(m_file.eof()) && (m_file.get() != ENDL)); // skip comment line
      continue;
    }

    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function determines whether specified text coincides with the last read lexeme.
///
/// \param  text  the text string to be checked for coincidence.
/// \return       nonzero if specified text coincides with the last read lexeme.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool IniFileParser::IsText( const char * text ) const
{
  return ((text != 0) && (_stricmp( text, m_lexeme.c_str() ) == 0));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function gives access to the file stream.
///
/// \return  reference to the file stream.
///////////////////////////////////////////////////////////////////////////////////////////////////
std::fstream & IniFileParser::File()
{
  return m_file;
}

} // namespace Elvees
