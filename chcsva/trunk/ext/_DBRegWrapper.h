//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ќбертки над IVideoConfig, эмулирующие DB и реестр

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   07.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _D_B_REG_WRAPPER_2260821418937359_
#define _D_B_REG_WRAPPER_2260821418937359_

//////////////////////////////////////////////////////////////////////////
// Adapters
//////////////////////////////////////////////////////////////////////////

class CWrappedRegKey
{
	std::auto_ptr<DBBridge::IVideoConfig>	m_pConfig;
	std::tstring							m_sRelativePath;
public:
	CWrappedRegKey( LPCTSTR szRelPath = _T("") ) : m_sRelativePath(szRelPath)
	{
	}
	

	DWORD Open( HKEY , LPCTSTR  )
	try
	{
		m_pConfig = std::auto_ptr<DBBridge::IVideoConfig>( DBBridge::CreateVideoConfig() );
		return ERROR_SUCCESS;
	}
	catch( std::exception )
	{
		return ERROR_INVALID_FUNCTION; 
	};

	DWORD Open( CWrappedRegKey& r, LPCTSTR szPath )
	try
	{
		m_sRelativePath	= szPath;
		m_pConfig		= r.m_pConfig;
		return ERROR_SUCCESS;
	}
	catch( std::exception )
	{
		return ERROR_INVALID_FUNCTION; 
	};

	DWORD Create( HKEY , LPCTSTR  )
	{
		return Open(HKEY(), NULL);	
	}

	DWORD Create(CWrappedRegKey& , LPCTSTR  )
	{
		return Open(HKEY(), NULL);	
	}

//////////////////////////////////////////////////////////////////////////
	DWORD	SetStringValue( LPCTSTR szValueName, LPCTSTR szValue  )
	try
	{
		m_pConfig->SetValue( m_sRelativePath.c_str(), szValueName, szValue );
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};

	DWORD	SetDWORDValue( LPCTSTR szValueName, DWORD dwValue  )
	try
	{
		m_pConfig->SetValue( m_sRelativePath.c_str(), szValueName, dwValue );
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};

	DWORD	SetBinaryValue( LPCTSTR szValueName, const void* pData, ULONG nSize  )
	try
	{
		std::vector<BYTE> buf( (const BYTE*)pData, (const BYTE*)pData + nSize );
		m_pConfig->SetValue( m_sRelativePath.c_str(), szValueName, buf );
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};
//////////////////////////////////////////////////////////////////////////

	DWORD	QueryStringValue( LPCTSTR szValueName, LPTSTR szValue, ULONG* pdwSize  )
	try
	{
		std::tstring s;
		m_pConfig->GetValue( m_sRelativePath.c_str(), szValueName, s );
		ULONG dwOldSize = *pdwSize;
		*pdwSize = (ULONG)s.length() + 1;
		if( dwOldSize >= *pdwSize )
		{	
			_tcsncpy( szValue, s.c_str(), *pdwSize );
		}
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};

	DWORD	QueryDWORDValue( LPCTSTR szValueName, DWORD& dwValue  )
		try
	{
		m_pConfig->GetValue( m_sRelativePath.c_str(), szValueName, dwValue );
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};

	DWORD	QueryBinaryValue( LPCTSTR szValueName, void* pData, ULONG* pdwSize  )
	try
	{
		std::vector<BYTE> buf;
		m_pConfig->GetValue( m_sRelativePath.c_str(), szValueName, buf );
		ULONG dwOldSize = *pdwSize;
		*pdwSize = (ULONG) buf.size();
		if( dwOldSize >= *pdwSize && !buf.empty() )
		{
			memcpy( pData, &buf[0], *pdwSize );
		}
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};

	DWORD	DeleteSubKey( LPCTSTR szUID  )
	try
	{
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};
};

//////////////////////////////////////////////////////////////////////////

//! access to devices (cameras)

class CWrappedDevicesRegKey 
{
//	friend class CWrappedDeviceRegKey;
//	friend DWORD RegEnumKey( CWrappedDevicesRegKey& rk, DWORD dwIndex, void* szName, DWORD dwSize  );

	std::auto_ptr<DBBridge::IVideoConfig>	m_pConfig;
//	std::auto_ptr<DBBridge::IDeviceConfig>	m_pDeviceConfig;

	std::vector<std::tstring> m_Devices;
	size_t			 m_nCurrentDevice;
public:
	CWrappedDevicesRegKey() : m_nCurrentDevice(0){}

	DWORD Open( HKEY , LPCTSTR  )
	try
	{
		m_pConfig = std::auto_ptr<DBBridge::IVideoConfig>( DBBridge::CreateVideoConfig() );
		m_pConfig->GetDevices(m_Devices);
		m_nCurrentDevice = 0;
		return ERROR_SUCCESS;
	}
	catch( std::exception )
	{
		return ERROR_INVALID_FUNCTION; 
	};

	//! chcsva не создает устройств в конйигурации
	DWORD Create( HKEY , LPCTSTR  )
	try
	{
		return ERROR_SUCCESS;
	}
	catch( std::exception )
	{
		return ERROR_INVALID_FUNCTION; 
	};

	DWORD DeleteSubKey( LPCTSTR szDeviceUID)
	try
	{
	//	m_pConfig->DeleteDevice( szDeviceUID );
		return ERROR_SUCCESS;
	}
	catch( std::exception )
	{
		return ERROR_INVALID_FUNCTION; 
	};

	std::auto_ptr<DBBridge::IDeviceConfig> GetDevice( LPCTSTR szDeviceUID )
	{
		return std::auto_ptr<DBBridge::IDeviceConfig>( m_pConfig->GetDevice(szDeviceUID) );
	}

	std::auto_ptr<DBBridge::IDeviceConfig> CreateDevice( LPCTSTR szDeviceUID )
	{
		return std::auto_ptr<DBBridge::IDeviceConfig>( m_pConfig->CreateDevice(szDeviceUID) );
	}
#if 0
	int	GetCurID() const 
	{ 
		return m_Devices[m_nCurrent]; 
	}
#endif
/*	size_t	GetCurrentPosition() const
	{
		return m_nCurrentDevice;
	}*/

	size_t GetDeviceNumber() const
	{
		return m_Devices.size();
	}

	std::tstring GetDeviceUID(DWORD idxDevice) const
	{
		_ASSERT(idxDevice < GetDeviceNumber());
		return idxDevice < GetDeviceNumber() ? m_Devices[ idxDevice ] : std::tstring();
	}
};

//! access the current camera

class CWrappedDeviceRegKey 
{
	//CWrappedDevicesRegKey*	m_prk;

	std::auto_ptr<DBBridge::IDeviceConfig>	m_pDeviceConfig;
public:
	DWORD Open( CWrappedDevicesRegKey& rk , LPCTSTR szDeviceUID )
	try
	{
	//	m_prk = &rk;
	//	++rk.m_nCurrent;
		m_pDeviceConfig = rk.GetDevice( szDeviceUID );
		return  m_pDeviceConfig.get() ? ERROR_SUCCESS : ERROR_INVALID_FUNCTION;
	}
	catch( std::exception )
	{
		return ERROR_INVALID_FUNCTION; 
	};

	DWORD	QueryStringValue( LPCTSTR szValueName, LPTSTR szValue, ULONG* pdwSize  )
		try
	{
		std::tstring s;
		m_pDeviceConfig->GetValue( szValueName, s );
		ULONG dwOldSize = *pdwSize;
		*pdwSize = (ULONG)s.length() + 1;
		if( dwOldSize >= *pdwSize )
		{	
			_tcsncpy( szValue, s.c_str(), *pdwSize );
		}
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};

	DWORD	QueryDWORDValue( LPCTSTR szValueName, DWORD& dwValue  )
		try
	{
		m_pDeviceConfig->GetValue( szValueName, dwValue );
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};

//////////////////////////////////////////////////////////////////////////

	DWORD Create( CWrappedDevicesRegKey& rk , LPCTSTR szDeviceUID )
		try
	{
		m_pDeviceConfig = std::auto_ptr<DBBridge::IDeviceConfig>( rk.CreateDevice( szDeviceUID ));
		return ERROR_SUCCESS;	
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};
	//////////////////////////////////////////////////////////////////////////
	DWORD	SetStringValue( LPCTSTR szValueName, LPCTSTR szValue  )
		try
	{
		m_pDeviceConfig->SetValue( szValueName, szValue );
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};

	DWORD	SetDWORDValue( LPCTSTR szValueName, DWORD dwValue  )
		try
	{
		m_pDeviceConfig->SetValue(  szValueName, dwValue );
		return ERROR_SUCCESS;
	}
	catch ( std::exception ) 
	{
		return ERROR_INVALID_FUNCTION;
	};
};

//! эмул€ци€ RegEnumKey над CWrappedDevicesRegKey
//! \param rk 
//! \param dwIndex 
//! \param szName 
//! \param dwSize 
//! \return 
inline DWORD RegEnumKey( CWrappedDevicesRegKey& rk, DWORD dwIndex, LPCTSTR szName, DWORD dwSize )
try
{
	if( dwIndex >= rk.GetDeviceNumber() )
	{
		return 1 ;
	}

//	rk.m_pCamConfig = std::auto_ptr<DBBridge::ICameraConfig>( rk.m_pConfig->GetCamera( rk.GetCurID()  ) );
	std::tstring sUID = rk.GetDeviceUID(dwIndex);
	_tcsncpy( (LPTSTR)szName,  sUID.c_str(), dwSize );
	return ERROR_SUCCESS;
}
catch (std::exception) 
{
	return 1;
}



#endif // _D_B_REG_WRAPPER_2260821418937359_