///////////////////////////////////////////////////////////////////////////////////////////////////
// ini_file_parser.h
// ---------------------
// begin     : Dec 2004
// modified  : 25 Dec 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ELVEES_INI_FILE_PARSER_H__
#define __DEFINE_ELVEES_INI_FILE_PARSER_H__

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class IniFileParser.
/// \brief Class assists in parsing of a simple ini-file.
///
/// <pre><tt>
/// -------------------------------------------------------
/// Structure of INI-file.
/// -------------------------------------------------------
/// # comment ...
/// # ...
/// # comment ...
///
/// field1   value1   # comment1 ...
///                   # ...
///
///
/// field2   value2   # comment2 ...
///                   # ...
///
/// ... Other fields, their values and comments.
///
/// Field is a text string without white spaces.
/// Value is a digit or a text string without white spaces.
/// Comment is an arbitrary string that begins with '#'.
/// -------------------------------------------------------
/// </pre></tt>
///
/// <pre><tt>
/// -------------------------------------------------------
/// Working scenario:
/// -------------------------------------------------------
/// Elvees::IniFileParser parser;
/// if (parser.Initialize( fileName ))
/// {
///   while (parser.Proceed())
///   {
///     if (parser.IsText( "field1" ))
///     {
///       parser.File() >> value;
///       ...
///     }
///     else if (parser.IsText( "field2" ))
///     {
///       parser.File() >> value;
///       ...
///     }
///     else ...
///   }
/// }
/// -------------------------------------------------------
/// </pre></tt>
///////////////////////////////////////////////////////////////////////////////////////////////////
class IniFileParser
{
private:
  std::string  m_lexeme;     //!< text buffer that contains a single lemexe
  std::fstream m_file;       //!< the source ini-file

public:
  IniFileParser();

  bool Initialize( const char * fileName );
  bool Proceed();
  bool IsText( const char * text ) const;

  std::fstream & File();
};

} // namespace Elvees

#endif // __DEFINE_ELVEES_INI_FILE_PARSER_H__

