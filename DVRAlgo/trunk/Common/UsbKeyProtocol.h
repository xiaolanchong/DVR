//! \file Protection.h
//! \brief класс CProtection
//!
//! создан в сент€бре 2005 года, ј. ’амухин
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
	//! открыть порт с именем stDeviceName и параметрами nReadTimeout (ожидание при чтении), nBaudRate (скорость передачи)
	HANDLE Open( int nBaudRate );
	//! им€ устройства
	std::string m_deviceName;
	//! handle устройства
	HANDLE m_hFile;

	//////////////////////////////////////////////////////////////////////////
	boost::mutex m_mutex;
	bool m_isInitialized;
};

//! глобальный экземпл€р защиты дл€ одного компьютера
extern USBKeyProtocol g_USBKey;