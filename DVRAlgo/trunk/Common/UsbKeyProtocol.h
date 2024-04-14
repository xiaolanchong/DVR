//! \file Protection.h
//! \brief ����� CProtection
//!
//! ������ � �������� 2005 ����, �. �������
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <boost/thread/thread.hpp>

class USBKeyProtocol
{
public:
	USBKeyProtocol();
	virtual ~USBKeyProtocol();
	
	unsigned char GetResponseFromKey( int argument );
	void Initialize( const std::string& deviceName  );
private:
	//! ������� ���� � ������ stDeviceName � ����������� nReadTimeout (�������� ��� ������), nBaudRate (�������� ��������)
	HANDLE Open( int nBaudRate );
	//! ��� ����������
	std::string m_deviceName;
	//! handle ����������
	HANDLE m_hFile;

	//////////////////////////////////////////////////////////////////////////
	boost::mutex m_mutex;
	bool m_isInitialized;
};

//! ���������� ��������� ������ ��� ������ ����������
extern USBKeyProtocol g_USBKey;