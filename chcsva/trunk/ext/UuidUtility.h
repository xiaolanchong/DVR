//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: uuid-string conversion

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   18.09.2006
*/                                                                                      //
//======================================================================================//
#ifndef _UUID_UTILITY_5344565638358562_
#define _UUID_UTILITY_5344565638358562_

#ifdef _UNICODE
typedef RPC_WSTR UuidString;
#else
typedef RPC_CSTR UuidString;
#endif

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

inline std::tstring UuidToString( const UUID& uid )
{
	UUID _uid = uid;
	UuidString buf;
	HRESULT hr = ::UuidToString( &_uid, &buf );
	std::tstring s;
	if(  S_OK == hr )
	{
		s = reinterpret_cast<LPCTSTR>(buf);
		RpcStringFree(&buf);
	}
	return s;
}

inline UUID	StringToUuid( const std::tstring& uid)
{
	UUID _uid;
	TCHAR buf[255];
	lstrcpyn( buf, uid.c_str(), 255 );
	RPC_STATUS res = ::UuidFromString( reinterpret_cast<UuidString>(buf), &_uid );
	if( S_OK != res )
	{
		_ASSERT(false);
		ZeroMemory( &_uid, sizeof(UUID) );
	}
	return _uid;
}

#endif // _UUID_UTILITY_5344565638358562_