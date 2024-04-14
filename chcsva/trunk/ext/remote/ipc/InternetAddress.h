//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: SOCKADDR Ó·ÂÚÍ‡

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   28.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _INTERNET_ADDRESS_8848072291420229_
#define _INTERNET_ADDRESS_8848072291420229_
//======================================================================================//
//                                class InternetAddress                                 //
//======================================================================================//

//! \brief SOCKADDR Ó·ÂÚÍ‡
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   28.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class InternetAddress : public SOCKADDR_IN
{
public:
	InternetAddress(
		unsigned long address,
		unsigned short port);

	InternetAddress(
		LPCTSTR lpAddress,
		unsigned short port);

	InternetAddress( const SOCKADDR_IN& address); 
};

std::string InternetAddressToString (const InternetAddress& Addr);

inline bool operator == ( const InternetAddress& lhs, const InternetAddress& rhs)
{
	return 0 == memcmp( &lhs, &rhs, sizeof(InternetAddress) );
}

#endif // _INTERNET_ADDRESS_8848072291420229_