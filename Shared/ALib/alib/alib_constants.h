///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_constanta.h
// ---------------------
// begin     : 1998
// modified  : 17 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_CONSTANTS_H__
#define __DEFINE_ALIB_CONSTANTS_H__

const double ALIB_PI = 3.14159265358979323846;
const double ALIB_DBL_MIN = DBL_MIN/DBL_EPSILON;
const float  ALIB_FLT_MIN = FLT_MIN/FLT_EPSILON;

const bool ALIB_INVERT_Y      = true;      // invert Y for bitmaps
const int  ALIB_MAX_IMAGE_DIM = (1<<11);   // max. width or height of an image

///////////////////////////////////////////////////////////////////////////////////////////////////
// Text constants.
///////////////////////////////////////////////////////////////////////////////////////////////////

const TCHAR ALIB_CURRENT_DIR[] = _T(".");
const TCHAR ALIB_PARENT_DIR[] = _T("..");
const TCHAR ALIB_UNSUPPORTED_EXCEPTION[] = _T("Unsupported exception");
const TCHAR ALIB_EXCEPTION[] = _T("EXCEPTION:");
const TCHAR ALIB_ERROR[] = _T("ERROR:");
const TCHAR ALIB_ERROR_IO_FILE[] = _T("I/O operation failed on file:");
const TCHAR ALIB_WRONG_INPUT_PARAMETERS[] = _T("Wrong input parameters");
const TCHAR ALIB_EMPTY[] = _T("");
const TCHAR ALIB_SPACE[] = _T(" ");
const TCHAR ALIB_FAILED_TO[] = _T("Failed to ");

#if (defined(_WIN32) || defined(_WIN64))
  const TCHAR ALIB_BACKSLASH = _T('\\');
#else
  const TCHAR ALIB_BACKSLASH = _T('/');
#endif

const TCHAR ALIB_NEWLINE = _T('\n');
const TCHAR ALIB_WIN_BACKSLASH = _T('\\');
const TCHAR ALIB_UNIX_BACKSLASH = _T('/');

#endif // __DEFINE_ALIB_CONSTANTS_H__

