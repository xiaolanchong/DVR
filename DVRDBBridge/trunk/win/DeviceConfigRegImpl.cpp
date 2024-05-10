//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация интерфейса на реестре

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.05.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "DeviceConfigRegImpl.h"
#include "CameraConfigRegImpl.h"
#include "../common/OwnIPAddress.h"
#include "RegCommon.h"

//======================================================================================//
//                              class DeviceConfigRegImpl                               //
//======================================================================================//

DeviceConfigRegImpl::DeviceConfigRegImpl( LPCTSTR szPath, LPCTSTR szUID, bool bCreate ):
	m_sUID ( szUID ),
	m_sPath( szPath )
{
	std::tstring s( m_sPath );
	s += _T("\\");
	s += m_sUID;
	DWORD dwRes = bCreate?	m_keyDevice.Create( HKEY_LOCAL_MACHINE, s.c_str() ):
							m_keyDevice.Open( HKEY_LOCAL_MACHINE, s.c_str(), KEY_READ );
	if( dwRes != ERROR_SUCCESS )
	{
		throw DBBridge::IVideoConfig::InitFailed("Failed to create/open the registry key");
	}
}

DeviceConfigRegImpl::~DeviceConfigRegImpl()
{
}

void	DeviceConfigRegImpl::GetValue( LPCTSTR szName, DWORD& dwValue)
{
	RegAssign<DBBridge::IVideoConfig::NoSuchProperty> res;
	res = m_keyDevice.QueryDWORDValue( szName, dwValue );
}

void	DeviceConfigRegImpl::GetValue( LPCTSTR szName, std::tstring& sValue )
{
	RegAssign<DBBridge::IVideoConfig::NoSuchProperty> res;
	ULONG dwSize = 0;
	res = m_keyDevice.QueryStringValue( szName, NULL, &dwSize );
	std::vector< TCHAR > Buf( dwSize );
	res = m_keyDevice.QueryStringValue( szName, &Buf[0], &dwSize );
	sValue.assign( Buf.begin(), Buf.end() );
}

void	DeviceConfigRegImpl::SetValue( LPCTSTR szName, DWORD dwValue)
{
	RegAssign<DBBridge::IVideoConfig::NoSuchProperty> res;
	res = m_keyDevice.SetDWORDValue( szName, dwValue );
}

void	DeviceConfigRegImpl::SetValue( LPCTSTR szName, LPCTSTR sValue )
{
	RegAssign<DBBridge::IVideoConfig::NoSuchProperty> res;
	res = m_keyDevice.SetStringValue( szName, sValue );
}

//////////////////////////////////////////////////////////////////////////

std::set<int>	GetCameraSet( CRegKey& keyDevice )
{
	std::set<int> CameraSet;
	const DWORD c_BufferSize	= 4096;
	TCHAR szCameraKeyBuf[c_BufferSize];
	DWORD dwCameraKeyBufSize	= c_BufferSize;
	DWORD dwCameraIndex			= 0;
	while( ERROR_SUCCESS == keyDevice.EnumKey( dwCameraIndex, szCameraKeyBuf, &dwCameraKeyBufSize ) )
	{
		try
		{
			int nCameraId = boost::lexical_cast<int>( szCameraKeyBuf );
			CameraSet.insert( nCameraId );
		}
		catch( std::exception& )
		{
		}
		dwCameraKeyBufSize = c_BufferSize;
		++dwCameraIndex;
	}
	return CameraSet;
}

void	DeviceConfigRegImpl::GetCameras( std::vector<int>& CamIDArr ) 
{
	CamIDArr.clear();
	
	std::set<int> CameraSet = GetCameraSet( m_keyDevice );
	CamIDArr.assign( CameraSet.begin(), CameraSet.end() );
}

DBBridge::ICameraConfig*	DeviceConfigRegImpl::GetCamera( int nCameraID  )
{
	std::set<int> CameraSet = GetCameraSet( m_keyDevice );
	std::set<int>::const_iterator it = CameraSet.find( nCameraID );
	return  it != CameraSet.end() ? new CameraConfigRegImpl( GetCameraPath().c_str() , nCameraID ) : 
									static_cast<DBBridge::ICameraConfig*> (NULL);
}

DBBridge::ICameraConfig*	DeviceConfigRegImpl::CreateCamera( int nCameraID, int nPinNumber )
{
	return new CameraConfigRegImpl( GetCameraPath().c_str() , nCameraID, nPinNumber );
}

void			DeviceConfigRegImpl::DeleteCamera( int nCameraID )
{
	// no throw
	std::tstring sCamera = boost::lexical_cast<std::tstring>( nCameraID );
	DWORD dwRes = SHDeleteKey( m_keyDevice, sCamera.c_str() );
	if( ERROR_SUCCESS != dwRes )
	{
		throw DBBridge::IVideoConfig::VideoConfigException( "Failed to delete a camera" );
	}
}

std::tstring	DeviceConfigRegImpl::GetIPAddress()
try
{
	std::tstring sIPAddress;
	GetValue( _T("IPAddress"), sIPAddress )  ;
	return sIPAddress;
}
catch( DBBridge::IVideoConfig::NoSuchProperty&   )
{
	// if no such property rely on a local device
	std::vector<std::tstring> IpList;
	try
	{
		IpList =  GetOwnIPAddress();
		return IpList[0];
	}
	catch (std::exception&)
	{
		return std::tstring(_T("127.0.0.1"));	
	}
}
