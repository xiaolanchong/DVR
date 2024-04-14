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
#ifndef _CONFIGURE_REGISTRY_WRAPPER_6080981262725626_
#define _CONFIGURE_REGISTRY_WRAPPER_6080981262725626_

//======================================================================================//
//                            class ConfigureRegistryWrapper                            //
//======================================================================================//

//! \brief Класс-обертка интерфейса над CRegKey для конфигурирования кодека и архива
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.09.2006
//! \version 1.0
//! \bug 
//! \todo 

#if 0
class ConfigureRegistryWrapper
{
	boost::shared_ptr<CHCS::IVideoSystemConfigEx> m_pVideoConfig;

	std::tstring								m_sArchivePath;
	std::tstring								m_sDbString;
	CHCS::IVideoSystemConfig::CodecSettings		m_CodecSettings;

	CHCS::IVideoSystemConfig::CodecSettings	*	m_RefCodecSettings;

public:
	ConfigureRegistryWrapper() ;
	~ConfigureRegistryWrapper();

	DWORD Open( HKEY , LPCTSTR )
	{
		m_RefCodecSettings = &m_CodecSettings;
		return ERROR_SUCCESS;
	}

	DWORD Open( ConfigureRegistryWrapper& r, LPCTSTR )
	{
		m_pVideoConfig		= r.m_pVideoConfig;
		m_RefCodecSettings	= r.m_RefCodecSettings;
	}

	DWORD	SetStringValue( LPCTSTR szValueName, LPCTSTR szValue  );
	DWORD	SetDWORDValue( LPCTSTR szValueName, DWORD dwValue  )
	DWORD	SetBinaryValue( LPCTSTR szValueName, const void* pData, ULONG nSize  );

	DWORD	QueryDWORDValue( LPCTSTR szValueName, DWORD& dwValue  );
	DWORD	QueryBinaryValue( LPCTSTR szValueName, void* pData, ULONG* pdwSize  );
};
#endif

#endif // _CONFIGURE_REGISTRY_WRAPPER_6080981262725626_