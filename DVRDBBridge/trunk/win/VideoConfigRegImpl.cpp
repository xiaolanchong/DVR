//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация интерфейса выдачи списка камер на БД

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   02.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "VideoConfigRegImpl.h"
#include "DeviceConfigRegImpl.h"

#include "RegCommon.h"
//#include "../resource.h"


//======================================================================================//
//                                class VideoConfigRegImpl                              //
//======================================================================================//


VideoConfigRegImpl::VideoConfigRegImpl(): 
	m_pkeyMain( new CRegKey )
{
	std::tstring sMain = GetRegistryPath();
	RegAssign<DBBridge::IVideoConfig::NoSuchProperty> res;
	DWORD res1st = m_pkeyMain->Open( HKEY_LOCAL_MACHINE, sMain.c_str(), KEY_READ);
	if( ERROR_SUCCESS != res1st )
	{
		res = m_pkeyMain->Create( HKEY_LOCAL_MACHINE, sMain.c_str());
	}
}

VideoConfigRegImpl::~VideoConfigRegImpl()
{
}

boost::shared_ptr<CRegKey>	VideoConfigRegImpl::OpenKey( LPCTSTR szKeyName )
{
	RegAssign<DBBridge::IVideoConfig::NoSuchProperty> res;
	if( szKeyName && _tcslen( szKeyName ) )
	{
		boost::shared_ptr<CRegKey> pkeyTemp( new CRegKey );
		res = pkeyTemp->Open( *m_pkeyMain, szKeyName, KEY_READ );  
		return pkeyTemp;
	}
	else return m_pkeyMain;
}

boost::shared_ptr<CRegKey>	VideoConfigRegImpl::OpenOrCreateKey( LPCTSTR szKeyName )
{
	if(  !szKeyName || !_tcslen( szKeyName ) ) return m_pkeyMain;
	RegAssign<NoSuchProperty> res;
	boost::shared_ptr<CRegKey> pkeyTemp( new CRegKey );
	DWORD res1st = pkeyTemp->Open( *m_pkeyMain, szKeyName );
	if( ERROR_SUCCESS != res1st )
	{
		res = pkeyTemp->Create( *m_pkeyMain, szKeyName );
	}
	return pkeyTemp;
}

void	VideoConfigRegImpl::GetValue( LPCTSTR szPath, LPCTSTR szName, DWORD& dwValue) 
{
	RegAssign<NoSuchProperty> res;
	boost::shared_ptr<CRegKey> pkeyValue = OpenKey( szPath );

	res = pkeyValue->QueryDWORDValue( szName, dwValue );
}

void	VideoConfigRegImpl::GetValue( LPCTSTR szPath, LPCTSTR szName, std::tstring& sValue )
{
	RegAssign<NoSuchProperty> res;
	boost::shared_ptr<CRegKey> pkeyValue = OpenKey( szPath );

	ULONG dwSize = 0;
	res = pkeyValue->QueryStringValue( szName, NULL, &dwSize );
	std::vector< TCHAR > Buf( dwSize );
	res = pkeyValue->QueryStringValue( szName, &Buf[0], &dwSize );
	sValue.assign( Buf.begin(), Buf.end() );
}

void	VideoConfigRegImpl::GetValue( LPCTSTR szPath, LPCTSTR szName, std::vector<BYTE>& binValue )
{
	RegAssign<NoSuchProperty> res;
	boost::shared_ptr<CRegKey> pkeyValue = OpenKey( szPath );

	_ASSERTE(pkeyValue.get());
	ULONG dwSize = 0;
	res = pkeyValue->QueryBinaryValue( szName, NULL, &dwSize );

	TCHAR stDump[32];
	wsprintf(stDump, TEXT("[size] = %u\n"), dwSize);
	OutputDebugString(stDump);

//	char* z = new char [dwSize];
//	*z = 0;
//	printf(z);
//	try {
	///*OutputDebugStringA( (*/boost::format("[size] = %u\n") % dwSize/*).str().c_str() )*/;
	binValue.resize(dwSize);

	if (dwSize > 0)
		res = pkeyValue->QueryBinaryValue( szName, &binValue[0], &dwSize );
/*	}
	catch(...)
	{
		OutputDebugString("Fuck!\n");
	}*/
}

void	VideoConfigRegImpl::SetValue( LPCTSTR szPath, LPCTSTR szName, DWORD dwValue)
{
	RegAssign<NoSuchProperty> res;
	boost::shared_ptr<CRegKey> pkeyValue = OpenOrCreateKey( szPath );

	res = pkeyValue->SetDWORDValue( szName, dwValue );
}

void	VideoConfigRegImpl::SetValue( LPCTSTR szPath, LPCTSTR szName, LPCTSTR sValue )
{
	RegAssign<NoSuchProperty> res;
	boost::shared_ptr<CRegKey> pkeyValue = OpenOrCreateKey( szPath );

	res = pkeyValue->SetStringValue( szName, sValue );
}

void	VideoConfigRegImpl::SetValue( LPCTSTR szPath, LPCTSTR szName, const std::vector<BYTE>& binValue )
{
	RegAssign<NoSuchProperty> res;
	boost::shared_ptr<CRegKey> pkeyValue = OpenOrCreateKey( szPath );

	res = pkeyValue->SetBinaryValue( szName, &binValue[0], (ULONG) binValue.size() );
}

std::tstring VideoConfigRegImpl::GetRegistryPath() 
{
#if 0
	TCHAR stKey[MAX_PATH];
	::LoadString(_Module.GetResourceInstance(), IDS_CHCS_REGISTRY_KEY, stKey, MAX_PATH);
	return std::tstring(stKey);
#else
	return _T("Software\\ElVEES\\DVR");
#endif
}
/*
std::tstring VideoConfigRegImpl::GetOwnIPAddress() 
try
{
	return ::GetOwnIPAddress();
}
catch ( std::exception& ex) 
{
	throw InitFailed( ex.what() );
};
*/
//////////////////////////////////////////////////////////////////////////

void	 VideoConfigRegImpl::GetDevices( std::vector<std::tstring>& DeviceArr ) 
{
	boost::shared_ptr<CRegKey> pkeyValue;
	try
	{
		pkeyValue = OpenKey( _T("Devices") );
	}
	catch ( std::exception& ) 
	{
		return;
	}
	DeviceArr.clear();
	if(!pkeyValue.get() )
	{
		return;
	}

	const DWORD c_BufferSize	= 4096;
	TCHAR szDeviceKeyBuf[c_BufferSize];
	DWORD dwDeviceKeyBufSize	= c_BufferSize;
	DWORD dwDeviceIndex			= 0;
	while( ERROR_SUCCESS == pkeyValue->EnumKey( dwDeviceIndex, szDeviceKeyBuf, &dwDeviceKeyBufSize ) )
	{
		DeviceArr.push_back( szDeviceKeyBuf );
		dwDeviceKeyBufSize = c_BufferSize;
		++dwDeviceIndex;
	}
}

DBBridge::IDeviceConfig*	VideoConfigRegImpl::GetDevice( LPCTSTR szUID  )
{
	boost::shared_ptr<CRegKey> pkeyValue = OpenKey( _T("Devices") );

	const DWORD c_BufferSize	= 4096;
	TCHAR szDeviceKeyBuf[c_BufferSize];
	DWORD dwDeviceKeyBufSize	= c_BufferSize;
	DWORD dwDeviceIndex			= 0;
	while( ERROR_SUCCESS == pkeyValue->EnumKey( dwDeviceIndex, szDeviceKeyBuf, &dwDeviceKeyBufSize ) )
	{
		if( strcmp( szDeviceKeyBuf, szUID) == 0 )
		{

			return new DeviceConfigRegImpl( GetDevicePath().c_str(), szUID, false );
		}
		dwDeviceKeyBufSize = c_BufferSize;
		++dwDeviceIndex;
	}
	return NULL;
}

DBBridge::IDeviceConfig*	VideoConfigRegImpl::CreateDevice( LPCTSTR szUID )
{
	boost::shared_ptr<CRegKey> pkeyValue = OpenOrCreateKey( _T("Devices") );
	if(!pkeyValue.get() )
	{
		return NULL;
	}

	return new DeviceConfigRegImpl( GetDevicePath().c_str(), szUID, true );
}

void			VideoConfigRegImpl::DeleteDevice( LPCTSTR szUID )
{
	boost::shared_ptr<CRegKey> pkeyValue = OpenKey( _T("Devices") );
	DWORD dwRes = SHDeleteKey( *pkeyValue.get(), szUID );
	if( ERROR_SUCCESS != dwRes )
	{
		throw DBBridge::IVideoConfig::VideoConfigException( "Failed to delete a device" );
	}
}