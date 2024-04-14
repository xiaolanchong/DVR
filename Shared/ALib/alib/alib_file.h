///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_file.h
// ---------------------
// begin     : Aug 2004
// modified  : 10 Feb 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_FILE_H__
#define __DEFINE_ALIB_FILE_H__

namespace alib
{

#if (defined(_WIN32) || defined(_WIN64))
#define  _ALIB_USE_WINDOWS_FILE_API_  1
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class BinaryFile.
/// \brief Class encapsulates simple I/O operations.
///
/// Functions throw alib::GeneralError exception on failure.
///////////////////////////////////////////////////////////////////////////////////////////////////
class BinaryFile
{
private: 
#if _ALIB_USE_WINDOWS_FILE_API_
  int    m_file;        //!< file identifier
#else
  FILE * m_file;        //!< pointer to the currently opened file
#endif
  StdStr m_name;        //!< file's name
  bool   m_bRead;       //!< nonzero if a file must be opened in read mode

public:
  BinaryFile();
  BinaryFile( const TCHAR * fname, bool bRead );
  ~BinaryFile();

  void open( const TCHAR * fname, bool bRead );
  void close();

  void read( void * buffer, int size );
  void write( const void * buffer, int size );
  void seek( __int64 position, int origin );

  __int64 size() const;
  bool reading() const { return m_bRead; }

  const TCHAR * name() const { return m_name.c_str(); }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Global functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

// alib_file.cpp:

void RemoveTrailingBackslash( StdStr & name );

void AddTrailingBackslash( StdStr & name );

StdStr GetFileName( const StdStr & name );

StdStr GetFilePath( const StdStr & name );

StdStr GetFileTitle( const StdStr & name );

StdStr GetFileExt( const StdStr & name );

StdStr GetFileLabel( const StdStr & name );

StdStr ReplaceFileExt( const StdStr & name, const TCHAR * newExt );

StdStr EnhanceFileName( const StdStr & name, const TCHAR * extra );

bool IsFileExistAndReadable( const TCHAR * fname );

const TCHAR * GetFileNamePtr( const StdStr & name );

const TCHAR * GetFileExtPtr( const StdStr & name );

bool ConstructNameOfSequenceFile( const TCHAR * first, int index, StdStr & name, bool bFull = true );

int GetSequenceIncrement( const TCHAR * first );

bool FillListOfFiles( const TCHAR * spec, StdStrLst & fileNames );

} // namespace alib

#endif // __DEFINE_ALIB_FILE_H__

