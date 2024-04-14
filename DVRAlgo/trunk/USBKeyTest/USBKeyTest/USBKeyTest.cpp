// USBKeyTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../../Common/UsbKeyProtocol.h"
#include "delta_time_protection.h"

int _tmain(int argc, _TCHAR* argv[])
{
	USBKey idealKey;
	

	g_USBKey.Initialize( "COM3" );
 

	for( int i = 0; i < 100; i++ )
	{	
		int x = rand();
		unsigned char ans = g_USBKey.GetResponseFromKey( x ); 
		unsigned char idealAns = idealKey.GetResponseFromKey( x );
		std::cout << (int)ans << " " << (int)idealAns << std::endl;
		Sleep( 100 );
	}
	
	return 0;
}

