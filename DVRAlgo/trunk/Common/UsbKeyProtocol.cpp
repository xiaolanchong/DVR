// Protection.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "UsbKeyProtocol.h"
#include <stdexcept>

USBKeyProtocol g_USBKey;


USBKeyProtocol::USBKeyProtocol(): m_isInitialized( false )
{
}

USBKeyProtocol::~USBKeyProtocol()
{
	if ( m_hFile != INVALID_HANDLE_VALUE ) 
	{
		CloseHandle(m_hFile);
	}
}

void USBKeyProtocol::Initialize( const std::string& deviceName  )
{
	boost::mutex::scoped_lock lock( m_mutex );
	if( !m_isInitialized )
	{
		m_deviceName = deviceName;
		m_hFile = Open( 115200 );
		GetResponseFromKey( 2863311530 ); //'AAAAAAAA' for reset
		m_isInitialized = true;
	}
}


HANDLE USBKeyProtocol::Open( int nBaudRate)
{
	boost::mutex::scoped_lock lock( m_mutex );

	HANDLE hPort = CreateFile( m_deviceName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if( ( hPort == INVALID_HANDLE_VALUE ) || ( hPort == 0 ) )
	{
		hPort = INVALID_HANDLE_VALUE;
		return hPort;
	}

	// Setup the default communication mask
	//

	if( !::SetCommMask(hPort, EV_RXCHAR) )
		throw std::runtime_error ( "SetCommMask failed" );

	
	DCB dcb = { 0 };
	dcb.DCBlength = sizeof(DCB);

	if ( !::GetCommState( hPort, &dcb ) )
	{
		throw std::runtime_error ( "GetCommState failed" );
	}
	else
	{
		// Set the new data
		dcb.BaudRate = nBaudRate;
		dcb.ByteSize = 8;
		dcb.Parity   = NOPARITY;
		dcb.StopBits = ONESTOPBIT;

		// Determine if parity is used
		dcb.fParity  = (dcb.Parity != NOPARITY);

		// SetupHandshaking
		//

		dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
		dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)

		// Set the new DCB structure
		if( !::SetCommState( hPort, &dcb ) )
			throw std::runtime_error( "SetCommState failed" );
	}

	// Setting a Time-Out Communication 
	//

	COMMTIMEOUTS cto;
	if ( !::GetCommTimeouts( hPort, &cto ) )
	{
		throw std::runtime_error( "GetCommTimeouts failed" );
	}
	else
	{
		cto.ReadIntervalTimeout = 40;
		cto.ReadTotalTimeoutMultiplier = 1;//30;
		cto.ReadTotalTimeoutConstant   = 1;// 40;

		cto.WriteTotalTimeoutConstant = 1;//1000;
		cto.WriteTotalTimeoutMultiplier = 1;//]100;


		if ( !::SetCommTimeouts( hPort, &cto ) )
			throw std::runtime_error( "SetCommTimeouts failed" ) ;
	}

	if( !::PurgeComm( hPort, PURGE_TXCLEAR | PURGE_RXCLEAR ) )
		throw std::runtime_error( "PurgeComm failed" );

	return hPort;
}


//////////////////////////////////////////////////////////////////////////
unsigned char USBKeyProtocol::GetResponseFromKey( int argument )
{
	boost::mutex::scoped_lock lock( m_mutex );
	DWORD written;
	BOOL b = WriteFile( m_hFile, &argument, 4, &written, 0 );
	DWORD read;
	unsigned char res = 0;
	ReadFile( m_hFile, &res, 1, &read, 0 );
	return res;
}

