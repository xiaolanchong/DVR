///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_file.cpp
// ---------------------
// begin     : Aug 2004
// modified  : 10 Feb 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alib.h"

#ifdef ALIB_CHECK_MEMORY_LEAKAGE
#define new DEBUG_NEW
#endif

namespace alib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of BinaryFile class.
///////////////////////////////////////////////////////////////////////////////////////////////////

#if _ALIB_USE_WINDOWS_FILE_API_
  #define  _ALIB_NULL_FILE_  -1
#else
  #define  _ALIB_NULL_FILE_   0
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
BinaryFile::BinaryFile() : m_file(_ALIB_NULL_FILE_), m_name(), m_bRead( true )
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor opens file in binary mode.
///////////////////////////////////////////////////////////////////////////////////////////////////
BinaryFile::BinaryFile( const TCHAR * fname, bool bRead )
: m_file(_ALIB_NULL_FILE_), m_name(), m_bRead( bRead )
{
  open( fname, bRead );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Destructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
BinaryFile::~BinaryFile()
{ 
  close();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function opens specified file in binary mode.
///
/// \param  fname  the name of the file to be opened.
/// \param  mode   open mode.
/// \param  bRead  nonzero if a file must be opened in read mode.
///////////////////////////////////////////////////////////////////////////////////////////////////
void BinaryFile::open( const TCHAR * fname, bool bRead )
{
  close();
  if ((fname == 0) || (_tcslen( fname ) == 0))
    ALIB_THROW( _T("Wrong file name") );

#if _ALIB_USE_WINDOWS_FILE_API_
  int oflag = (_O_BINARY | (bRead ? (_O_RDONLY) : (_O_WRONLY | _O_TRUNC | _O_CREAT)));
  int pmode = (bRead ? (_S_IREAD) : (_S_IWRITE));
  m_file = _topen( fname, oflag, pmode );
#else
  m_file = _tfopen( fname, (bRead ? _T("rb") : _T("wb")) );
#endif

  if (m_file == _ALIB_NULL_FILE_)
  {
    StdStr txt( _T("Failed to open file: ") );
    ALIB_THROW( ((txt += ALIB_NEWLINE) += m_name).c_str() );
  }

  m_name = fname;
  m_bRead = bRead;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function closes the currently opened file.
///////////////////////////////////////////////////////////////////////////////////////////////////
void BinaryFile::close()
{
  if (m_file != _ALIB_NULL_FILE_)
  {
#if _ALIB_USE_WINDOWS_FILE_API_
    _close( m_file );
#else
    fclose( m_file );
#endif
  }
  m_file = _ALIB_NULL_FILE_;
  m_name.erase();
  m_bRead = true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function reads a block of bytes from the file.
///
/// \param  buffer  the output storage.
/// \param  size    the number of bytes to be read.
///////////////////////////////////////////////////////////////////////////////////////////////////
void BinaryFile::read( void * buffer, int size )
{
  ALIB_ASSERT( (m_file != 0) && m_bRead );
  if ((buffer != 0) && (size > 0))
  {
#if _ALIB_USE_WINDOWS_FILE_API_
    if (_read( m_file, buffer, size ) != size)
#else
    if (fread( buffer, size, 1, m_file ) != 1)
#endif
    {
      StdStr txt( _T("Failed to read file: ") );
      ALIB_THROW( ((txt += ALIB_NEWLINE) += m_name).c_str() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function writes a block of bytes to the file.
///
/// \param  buffer  the data source.
/// \param  size    the number of bytes to be written.
///////////////////////////////////////////////////////////////////////////////////////////////////
void BinaryFile::write( const void * buffer, int size )
{
  ALIB_ASSERT( (m_file != 0) && !m_bRead );
  if ((buffer != 0) && (size > 0))
  {
#if _ALIB_USE_WINDOWS_FILE_API_
    if (_write( m_file, buffer, size ) != size)
#else
    if (fwrite( buffer, size, 1, m_file ) != 1)
#endif
    {
      StdStr txt( _T("Failed to write file: ") );
      ALIB_THROW( ((txt += ALIB_NEWLINE) += m_name).c_str() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns size of binary file. !!!Microsoft specific!!!
///////////////////////////////////////////////////////////////////////////////////////////////////
__int64 BinaryFile::size() const
{
  __int64 leng = -1L;
#if _ALIB_USE_WINDOWS_FILE_API_
  if ((m_file == _ALIB_NULL_FILE_) || ((leng = _filelengthi64( m_file )) < 0L))
#else
  if ((m_file == _ALIB_NULL_FILE_) || ((leng = _filelengthi64( _fileno( m_file ) )) < 0L))
#endif
  {
    StdStr txt( _T("Failed to get file size: ") );
    ALIB_THROW( ((txt += ALIB_NEWLINE) += m_name).c_str() );
  }
  return leng;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function sets current pointer position in the file.
///
/// \param  position   number of bytes from <i>origin</i>.
/// \param  origin     initial position: SEEK_CUR, SEEK_END or SEEK_SET.
///////////////////////////////////////////////////////////////////////////////////////////////////
void BinaryFile::seek( __int64 position, int origin )
{
#if _ALIB_USE_WINDOWS_FILE_API_
  if ((m_file == _ALIB_NULL_FILE_) || (_lseeki64( m_file, position, origin ) < 0L))
#else
  ALIB_ASSERT( (long)position <= std::numeric_limits<long>::max() );
  if ((m_file == _ALIB_NULL_FILE_) || (fseek( m_file, (long)position, origin ) != 0))
#endif
  {
    StdStr txt( _T("Failed to set new file position: ") );
    ALIB_THROW( ((txt += ALIB_NEWLINE) += m_name).c_str() );
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Global functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

inline bool IsAlphabetOrUnderscore( TCHAR ch )
{
  return ((_istalpha( ch ) != 0) || (ch == _T('_')));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function removes trailing backslash from the name of an input directory.
///////////////////////////////////////////////////////////////////////////////////////////////////
void RemoveTrailingBackslash( StdStr & name )
{
  int n = (int)(name.size());
  if ((n > 0) && ((name[n-1] == ALIB_WIN_BACKSLASH) || (name[n-1] == ALIB_UNIX_BACKSLASH)))
    name.resize( n-1 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function adds the trailing backslash to the path name if necessary.
///////////////////////////////////////////////////////////////////////////////////////////////////
void AddTrailingBackslash( StdStr & name )
{
  int n = (int)(name.size());
  if ((n > 0) && (name[n-1] != ALIB_WIN_BACKSLASH) && (name[n-1] != ALIB_UNIX_BACKSLASH))
    name += ALIB_BACKSLASH;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function extracts a short file name from a full-path file name: <br>
///        "C:\FFF\KKK\LLL\abcd.txt" --> "abcd.txt".
///
/// \param  name  source full-path file name.
/// \return       extracted short file name.
///////////////////////////////////////////////////////////////////////////////////////////////////
StdStr GetFileName( const StdStr & name )
{
  if (name.empty())
    return name;

  unsigned int pos = (unsigned int)(name.find_last_of( _T("/\\") ));
  return ((pos == name.npos) ? name : (name.substr( pos+1 )));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function extracts a path from a file name without trailing backslash: <br>
///        "C:\FFF\KKK\LLL\abcd.txt" --> "C:\FFF\KKK\LLL", <br>  "abcd.txt" --> "".
///
/// \param  name  source full-path file name.
/// \return       extracted path.
///////////////////////////////////////////////////////////////////////////////////////////////////
StdStr GetFilePath( const StdStr & name )
{
  if (name.empty())
    return name;

  unsigned int pos = (unsigned int)(name.find_last_of( _T("/\\") ));
  return ((pos == name.npos) ? (StdStr()) : (name.substr( 0, pos )));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function extracts base name of a file: <br> "C:\FFF\KKK\LLL\abcd.tar.zip" --> "abcd".
///
/// \param  name  source file name.
/// \return       extracted base name.
///////////////////////////////////////////////////////////////////////////////////////////////////
StdStr GetFileTitle( const StdStr & name )
{
  if (name.empty())
    return name;

  unsigned int pos1 = (unsigned int)(name.find_last_of( _T("/\\") ));
  pos1 = (pos1 == name.npos) ? 0 : (pos1+1);
  unsigned int pos2 = (unsigned int)(name.find( _T('.'), pos1 ));
  unsigned int n = (pos2 == name.npos) ? (unsigned int)(name.npos) : (pos2-pos1);
  return (name.substr( pos1, n ));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function extracts extension from a file name: <br> "C:\FFF\LLL\abcd.tar.zip" --> "zip".
///
/// \param  name  source file name.
/// \return       extracted extension.
///////////////////////////////////////////////////////////////////////////////////////////////////
StdStr GetFileExt( const StdStr & name )
{
  if (name.empty())
    return name;

  unsigned int pos = (unsigned int)(name.find_last_of( _T("./\\") ));
  if ((pos == name.npos) || (name[pos] != _T('.')))
    return StdStr();

  return (name.substr( pos+1 ));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function extracts file label from the file name.
///
/// Label is the group of letters or underscores at the beggining of file's name.
/// For example, for '_aaa__8756_anything.bmp' the label is '_aaa__'.
///
/// \param  name  source file name.
/// \return       extracted label.
///////////////////////////////////////////////////////////////////////////////////////////////////
StdStr GetFileLabel( const StdStr & name )
{
  if (name.empty())
    return name;

  unsigned int first = (unsigned int)(name.find_last_of( _T("/\\") ));
  unsigned int last = (first = ((first == name.npos) ? 0 : (first+1)));

  while( (last < name.size()) && IsAlphabetOrUnderscore( name[last] ) ) last++;
  return (name.substr( first, (last-first) ));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function changes extension of the specified file name.
///
/// If file name has no extension, then new extension will be added.
/// Situations "." or ".." or "C:\AAA\BBB\" are not supported (does nothing).
///
/// \param  name    source file name.
/// \param  newExt  new extension.
/// \return         file name with replaced extension.
///////////////////////////////////////////////////////////////////////////////////////////////////
StdStr ReplaceFileExt( const StdStr & name, const TCHAR * newExt )
{
  if ((newExt == 0) || name.empty() || (name == ALIB_CURRENT_DIR) || (name == ALIB_PARENT_DIR))
    return name;

  unsigned int pos = (unsigned int)(name.find_last_of( _T("./\\") ));
  StdStr       res( name );

  if (pos != name.npos)
  {
    if (name[pos] == _T('.'))
      res.erase( pos );
    else if ((pos+1) >= name.size())
      return name;
  }

  (res += _T('.')) += newExt;
  return res;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function inserts additional string before the last dot of a file name: <br>
///        "C:\FFF\LLL\abcd.tar.zip" --> "C:\FFF\LLL\abcd.tarextra.zip".
///
/// \param  name   source file name.
/// \param  extra  string to be added.
/// \return        enhanced file name.
///////////////////////////////////////////////////////////////////////////////////////////////////
StdStr EnhanceFileName( const StdStr & name, const TCHAR * extra )
{
  StdStr res = name;

  if (extra != 0)
  {
    unsigned int p = (unsigned int)(name.rfind( _T('.') ));
    res.insert( ((p == name.npos) ? (unsigned int)(name.size()) : p), extra );
  }
  return res;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function checks a file for existence and readability.
///
/// \param  fname  the name of a file that should be checked for existance.
/// \return        nonzero if a readable file exists.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool IsFileExistAndReadable( const TCHAR * fname )
{
  struct _stat s;
  if (fname == 0)
    return false;

#ifdef _UNICODE
  int result = _wstat( fname, &s );
#else
  int result = _stat( fname, &s );
#endif

  return ((result == 0) && (s.st_mode & _S_IFREG) && (s.st_mode & _S_IREAD));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns pointer to the file name.
///
/// \param  name  source file name.
/// \return       Ok = pointer that points just beyond file path, if any, otherwise 0.
///////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR * GetFileNamePtr( const StdStr & name )
{
  if (name.empty())
    return 0;

  unsigned int pos = (unsigned int)(name.find_last_of( _T("/\\") ));
  return ((pos == name.npos) ? (name.c_str()) : (name.c_str()+pos+1));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns pointer to the file extension.
///
/// \param  name  source file name.
/// \return       Ok = pointer that points to the file extension, otherwise 0.
///////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR * GetFileExtPtr( const StdStr & name )
{
  if (name.empty())
    return 0;

  StdStr::size_type pos = name.find_last_of( _T("/\\.") );
  return (((pos == name.npos) || (name[pos] != _T('.'))) ? 0 : (name.c_str()+pos+1));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function constructs a name of sequence file
///        given its index and the name of the first file.
///
/// Suppose that the name of the first file looks as follows: "c:\aaa\bbb\ccc\d0001.bmp".
/// For example, we want to construct a file name with the index 7. The output
/// result be as follows: "c:\aaa\bbb\ccc\d0008.bmp" (1+7).
///
/// \param  first  name of the starting file of the sequence.
/// \param  index  zero-based index of desired sequence file, counting from the first one.
/// \param  fname  out: constructed name.
/// \param  bFull  if nonzero, then full-path name will be constructed.
/// \return        Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ConstructNameOfSequenceFile( const TCHAR * first,
                                           int           index,
                                           StdStr      & name,
                                           bool          bFull )
{
  const int MAX_INDEX_SIZE = 32;

  if ((first == 0) || (first == name.c_str()) || !(alib::IsRange( index, 0, INT_MAX/10 )))
    return false;
  name = first;

  // Skip file path and place position at the beginning of the file name.
  uint pos = (uint)(name.find_last_of( _T("/\\") ));
  pos = (pos == name.npos) ? 0 : (pos+1);
  if ((pos > 0) && !bFull)
  {
    name.erase( 0, pos );
    pos = 0;
  }

  // Skip non-digits.
  while ((pos < name.size()) && ((int)(name[pos]) != int('.')) && !isdigit( (int)(name[pos]) ))
    ++pos;
  if ((pos == name.size()) || ((int)(name[pos]) == int('.')))
    return false;                 // file title does not contain index

  uint p = pos;
  int  i = 0, nDigit = 0;
  char text[2*MAX_INDEX_SIZE], format[2*MAX_INDEX_SIZE];

  // Extract index text.
  memset( text, 0, sizeof(text) );
  while ((p < name.size()) && (nDigit < MAX_INDEX_SIZE) && isdigit( (int)(name[p]) ))
    text[nDigit++] = (char)(name[p++]);

  // Obtain index of the first frame.
  if (!(alib::IsRange( nDigit, 1, MAX_INDEX_SIZE )) || (sscanf( text, "%d", &i ) != 1))
    return false;                 // wrong index text

  // Construct name of desired frame.
  index += i;
  sprintf( format, "%%0%dd", nDigit );
  sprintf( text, format, index );
  for (i = 0; i < nDigit; i++)
    name[pos+i] = (TCHAR)(text[i]);

  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function analyse index difference between the first and the second existing files
///        of sequence and deduces index increment.
///
/// \param  first  name of the first file of videosequence.
/// \return        Ok = index increment between the first and the second files, otherwise 0.
///////////////////////////////////////////////////////////////////////////////////////////////////
int GetSequenceIncrement( const TCHAR * first )
{
  const int MAX_INC = 32;
  StdStr    name;
  int       inc = 0;

  // Determine index increment.
  for (; inc < MAX_INC; inc++)
  {
    if (!ConstructNameOfSequenceFile( first, inc, name, true ))
      return 0;
    if (alib::IsFileExistAndReadable( name.c_str() ))
      break;
  }

  // Too large increment is erroneous.
  return ((inc >= MAX_INC) ? 0 : inc);
}

} // namespace alib

