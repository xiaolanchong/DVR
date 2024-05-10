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
#include "CameraConfigRegImpl.h"
#include "RegCommon.h"

//======================================================================================//
//                              class CameraConfigRegImpl                               //
//======================================================================================//

CameraConfigRegImpl::CameraConfigRegImpl(LPCTSTR szPath, int nID, int nPinNumber):
	m_sPath( szPath ),
	m_nID( nID )
{
	std::tstring s( m_sPath );
	s += _T("\\");
	s += boost::lexical_cast<std::tstring>( nID );
	DWORD dwRes = m_keyCamera.Create( HKEY_LOCAL_MACHINE, s.c_str() );
	if( dwRes != ERROR_SUCCESS )
	{
		throw DBBridge::IVideoConfig::InitFailed("Failed to create/open the registry key");
	}
	dwRes = m_keyCamera.SetDWORDValue( "Device_Input", static_cast<DWORD>( nPinNumber ) );

	if( dwRes != ERROR_SUCCESS )
	{
		throw DBBridge::IVideoConfig::InitFailed("Failed to create/open the registry key");
	}
}

CameraConfigRegImpl::CameraConfigRegImpl(LPCTSTR szPath, int nID ):
	m_sPath( szPath ),
	m_nID( nID )
{
	std::tstring s( m_sPath );
	s += _T("\\");
	s += boost::lexical_cast<std::tstring>( nID );
	DWORD dwRes = m_keyCamera.Open( HKEY_LOCAL_MACHINE, s.c_str(), KEY_READ );
	if( dwRes != ERROR_SUCCESS )
	{
		throw DBBridge::IVideoConfig::InitFailed("Failed to create/open the registry key");
	}
}

CameraConfigRegImpl::~CameraConfigRegImpl()
{
}

int		CameraConfigRegImpl::GetCameraPin()
{
	DWORD dwValue;
	RegAssign<DBBridge::IVideoConfig::NoSuchProperty> res;
	res = m_keyCamera.QueryDWORDValue( "Device_Input", dwValue );
	return static_cast<int>( dwValue );
}

std::tstring	CameraConfigRegImpl::GetDescription()
{
	RegAssign<DBBridge::IVideoConfig::NoSuchProperty> res;
	TCHAR szDesc[1024];
	DWORD DescSize = 1024;
	res = m_keyCamera.QueryStringValue( _T("Description"), szDesc, &DescSize );
	return std::tstring( szDesc );
}

void			CameraConfigRegImpl::SetDescription(const std::tstring& sDesc)
{
	RegAssign<DBBridge::IVideoConfig::NoSuchProperty> res;
	res = m_keyCamera.SetStringValue( _T("Description"), sDesc.c_str() );
}