//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Класс-обертка интерфейса над CRegKey для конфигурирования кодека и архива

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   18.09.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "ConfigureRegistryWrapper.h"

#if 0
//======================================================================================//
//                            class ConfigureRegistryWrapper                            //
//======================================================================================//

ConfigureRegistryWrapper::ConfigureRegistryWrapper():
	m_pVideoConfig( CHCS::CreateVideoSystemConfig() ) ,
	m_RefCodecSettings(0)
{
}

ConfigureRegistryWrapper::~ConfigureRegistryWrapper()
{
}

//////////////////////////////////////////////////////////////////////////

std::vector<BYTE>* ConfigureRegistryWrapper::GetBinaryField(const  std::tstring& sValueName)
{
	if( !m_RefCodecSettings)
	{
		return 0;
	}

	std::tstring sName( szValueName ) ;
	if ( sName == _T("Codec_State") )
	{
		pRealData = &m_RefCodecSettings->m_CodecStateData;
	}
}

DWORD*				ConfigureRegistryWrapper::GetDWORDField(const  std::tstring& sValueName)
{
	if( !m_RefCodecSettings)
	{
		return 0;
	}

	if( sValueName == _T("Codec_FCC") ) 
	{
		return &m_RefCodecSettings->m_CodecFCC;
	}
	else if( sValueName == TEXT("Codec_Quality") )
	{
		return &m_RefCodecSettings->m_lCodecQuality;
	}
	else if( TEXT("Codec_KeyRate") )
	{

	}
	else if( TEXT("Codec_DataRate") )
	{

	}
	else if( TEXT("Video_FMP") )
	{

	}
	else if(TEXT("Video_MaxSize") )
	{

	}
}

std::tstring*		ConfigureRegistryWrapper::GetStringField(const  std::tstring& sValueName);
{
	if( !m_RefCodecSettings)
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////


DWORD	ConfigureRegistryWrapper::SetStringValue( LPCTSTR szValueName, LPCTSTR szValue  )
{

}

DWORD	ConfigureRegistryWrapper::SetDWORDValue( LPCTSTR szValueName, DWORD dwValue  )
{

}

DWORD	ConfigureRegistryWrapper::SetBinaryValue( LPCTSTR szValueName, const void* pData, ULONG nSize  )
{
	if(! m_RefCodecSettings )
	{
		return ERROR_INVALID_DATA;
	}

	std::vector< BYTE>	pRealData = 0;


	if( pRealData )
	{
		const BYTE* pByteData = static_cast<const BYTE>( pData ); 
		pRealData->assign( pByteData, pByteData + nSize );
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_INVALID_DATA;
	}
}

DWORD	ConfigureRegistryWrapper::QueryDWORDValue( LPCTSTR szValueName, DWORD& dwValue  )
{
	if(! m_RefCodecSettings )
	{
		return ERROR_INVALID_DATA;
	}
}

DWORD	ConfigureRegistryWrapper::QueryBinaryValue( LPCTSTR szValueName, void* pData, ULONG* pdwSize  )
{
	if(! m_RefCodecSettings )
	{
		return ERROR_INVALID_DATA;
	}
}

#endif