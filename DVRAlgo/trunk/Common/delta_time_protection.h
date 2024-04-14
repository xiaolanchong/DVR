#pragma once
#include "boost/format.hpp"
#include "boost/thread/thread.hpp"
#include "UsbKeyProtocol.h"


class USBKey
{
public:
	USBKey(): oldRes( 0 )
	{
	}
	unsigned char GetResponseFromKey( int argument )
	{
		unsigned char * bytes = (unsigned char * )(&argument);
		int res = 170 ^ bytes[0] ^ bytes[1] ^ bytes[2] ^ bytes[3] ^ oldRes;
		oldRes = res;
		return res;
	}
private:
	unsigned char oldRes;
};

extern USBKey g_idealKey;

class DeltaTimeProtection
{
public:
	DeltaTimeProtection():cnt(0), oldRes(0), dif(0), bUseIdealKey( false )
	{
	}
	virtual ~DeltaTimeProtection()
	{
	}

	void Initialize( const std::string& protectionDevice )
	{
		if( protectionDevice == "U3nb40Ls2CfPO561J350d8cvbcvMFGJdJged8gdfg" )
			bUseIdealKey = true;

		if( !bUseIdealKey )
			g_USBKey.Initialize( protectionDevice );
	}

	//returns delta (difference between current and previous times)
	int operator()()
	{
		boost::mutex::scoped_lock lock( m_mutex );

		const int keyTime = 71;
		const int difTime = 137;

		++cnt;
		OutputDebugString( ( boost::format("cnt = %d\n") % cnt ).str().c_str() );

		if ( cnt == keyTime )
		{
			r = rand();
			
			if( r == 2863311530 ) // 'AAAAAAAA'
				r++;

			if( bUseIdealKey)
				res = g_idealKey.GetResponseFromKey( r );
			else
				res = g_USBKey.GetResponseFromKey( r );
			
			return dif * dif;
		} 
		else if ( cnt >= difTime  )
		{
			dif = res - HashFun( r, oldRes );
			oldRes = res;
			cnt = 0;
			return dif * dif;
		}
	
		return dif * dif;
	}

	unsigned char HashFun( int r, unsigned char oldRes )
	{
		unsigned char * bytes = (unsigned char * )(&r);
		int res = 170 ^ bytes[0] ^ bytes[1] ^ bytes[2] ^ bytes[3] ^ oldRes;		
		return res;
	}
private:
	
	unsigned char oldRes;
	int dif;
	int res;
	int r;
	int cnt;
	boost::mutex m_mutex;
	bool bUseIdealKey;
};

extern DeltaTimeProtection g_protection;