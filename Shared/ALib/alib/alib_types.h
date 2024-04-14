///////////////////////////////////////////////////////////////////////////////////////////////////
// alib_types_const.h
// ---------------------
// begin     : 1998
// modified  : 17 Jan 2006
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_ALIB_TYPES_H__
#define __DEFINE_ALIB_TYPES_H__

#ifndef TCHAR
  #ifdef _UNICODE
    #define TCHAR wchar_t
  #else
    #define TCHAR char
  #endif
#endif // TCHAR

typedef  std::vector<sbyte>             SByteArr;
typedef  std::vector<ubyte>             UByteArr;
typedef  std::vector<sword>             SWordArr;
typedef  std::vector<uword>             UWordArr;
typedef  std::vector<sint>              SIntArr;
typedef  std::vector<uint>              UIntArr;
typedef  std::vector<float>             FloatArr;
typedef  std::vector<double>            DoubleArr;
typedef  std::vector<long double>       LongDoubleArr;

typedef  std::list<sbyte>               SByteLst;
typedef  std::list<ubyte>               UByteLst;
typedef  std::list<sword>               SWordLst;
typedef  std::list<uword>               UWordLst;
typedef  std::list<sint>                SIntLst;
typedef  std::list<uint>                UIntLst;
typedef  std::list<float>               FloatLst;
typedef  std::list<double>              DoubleLst;

typedef  std::string                    AStr;
typedef  std::stringstream              AStrStream;
typedef  std::wstring                   WStr;
typedef  std::wstringstream             WStrStream;

typedef  std::basic_string<TCHAR>       StdStr;
typedef  std::vector<StdStr>            StdStrArr;
typedef  std::list<StdStr>              StdStrLst;
typedef  std::basic_stringstream<TCHAR> StdStrStream;
typedef  std::basic_ostream<TCHAR>      StdOStream;
typedef  std::basic_iostream<TCHAR>     StdBaseStream;
typedef  std::auto_ptr<StdStrStream>    StdStrStrmPtr;

#endif // __DEFINE_ALIB_TYPES_H__

