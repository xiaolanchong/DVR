//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: XML document IVideoSystemConfigEx implementation

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   16.09.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#include "XmlVideoSystemConfig.h"

#include "XmlUtility.h"
#include "../../UuidUtility.h"

CComPtr<IXMLDOMElement>	GetParent( IXMLDOMElement* pChild)
{
	CComPtr<IXMLDOMNode> pParentNode;
	HRChecker hr;
	hr = pChild->get_parentNode(&pParentNode);
	CComQIPtr<IXMLDOMElement> pParentElement(pParentNode);
	return pParentElement;
}
//////////////////////////////////////////////////////////////////////////

std::tstring	GetCameraPath()
{
	return  
		( boost::tformat( _T("%s/%s/%s") ) 
		% c_szRootName 
		% c_szDeviceElementName	
		% c_szCameraElementName 
		).str();			
	;
}

//std::tstring	GetDevicePath

using std::max;
//======================================================================================//
//                              class XmlVideoSystemConfig                              //
//======================================================================================//

XmlVideoSystemConfig::XmlVideoSystemConfig(LPCTSTR szFileName):
	m_sFileName(szFileName),
	m_NewCameraID(1)
{
	HRChecker hr;
//	CComPtr<IXMLDOMDocument>	pDocument;

	hr = m_pDocument.CoCreateInstance( CLSID_DOMDocument );
	VARIANT_BOOL bRes;
	//	HRESULT hrLoad;
	m_pDocument->load( CComVariant( szFileName ), &bRes );

	CComPtr<IXMLDOMElement>	pRootElement;
	HRESULT hrRaw = m_pDocument->get_documentElement ( &pRootElement );
	if( S_OK != hrRaw )
	{
		hr = m_pDocument->createElement( CComBSTR(c_szRootName), &pRootElement );
		hr = m_pDocument->putref_documentElement ( pRootElement );
		SaveXmlFile();
	}
	else
	{
		std::tstring sPath = GetCameraPath();
		std::vector<CComPtr<IXMLDOMElement> > Elements = 
			GetAllElements( m_pDocument, sPath.c_str() );
		//m_NewCameraID = static_cast<int>(Elements.size()) + 1;
		for ( size_t i = 0; i < Elements.size(); ++i )
		{
			LONG CameraID = GetAttribute<LONG>( Elements[i], _T("CameraID") );
			m_NewCameraID = max( m_NewCameraID, CameraID );
		}
		++m_NewCameraID;
	}
}

XmlVideoSystemConfig::~XmlVideoSystemConfig()
{
}

std::tstring XmlVideoSystemConfig::GetArchivePath()
{
	// not implemented
	return std::tstring();
}

void		 XmlVideoSystemConfig::GetCodecSettings(IVideoSystemConfig::CodecSettings& params)
{
	// not implemented
}

void		 XmlVideoSystemConfig::SetArchivePath(const std::tstring& sArchivePath)
{
	// not implemented
}

void		 XmlVideoSystemConfig::SetCodecSettings(const IVideoSystemConfig::CodecSettings& params) 
{
	// not implemented
}

//////////////////////////////////////////////////////////////////////////


void	XmlVideoSystemConfig::DeleteAllDevicesAndCameras()
{
	CComPtr<IXMLDOMElement>	pRootElement;
	HRChecker hr;
	hr = m_pDocument->get_documentElement ( &pRootElement );

	std::tstring sPath = 
		( boost::tformat( _T("%s/%s") ) 
		% _T(".") 
		% c_szDeviceElementName	
		).str();

	std::vector<CComPtr<IXMLDOMElement> > Devices = GetAllElements( pRootElement, sPath.c_str() );
	for ( size_t i = 0; i < Devices.size(); ++i )
	{
		hr = pRootElement->removeChild( Devices[i], 0 );
	}
	SaveXmlFile();
}

void	XmlVideoSystemConfig::CreateDevice( const IVideoSystemConfig::DeviceSettings& ds )
{
	std::tstring sPath = 
		( boost::tformat( _T("%s/%s[@%s=\"%s\"]") ) 
		% c_szRootName 
		% c_szDeviceElementName	
		% _T("Device_DisplayName") 
		% ds.stMoniker
		).str();

	std::vector<CComPtr<IXMLDOMElement> > Elements = 
		GetAllElements( m_pDocument, sPath.c_str() );

	if( Elements.empty() )
	{
		CComPtr<IXMLDOMElement>	pRootElement;
		CComPtr<IXMLDOMElement>	pNewDeviceElement;
		HRChecker hr;
		hr = m_pDocument->get_documentElement ( &pRootElement );
		hr = m_pDocument->createElement( CComBSTR(c_szDeviceElementName), &pNewDeviceElement );
		
		const DeviceSettings& DefaultSettings = ds;

		SetAttribute( pNewDeviceElement, c_szUIDAtrributeName, UuidToString( DefaultSettings.streamID ) );
		SetAttribute( pNewDeviceElement, _T("Device_DisplayName"), DefaultSettings.stMoniker );
		SetAttribute( pNewDeviceElement, _T("Device_Width"), DefaultSettings.lWidth );
		SetAttribute( pNewDeviceElement, _T("Device_Height"), DefaultSettings.lHeight );
		SetAttribute( pNewDeviceElement, _T("Device_FrameRate"), DefaultSettings.lFrmRate );
		SetAttribute( pNewDeviceElement, _T("Device_Standard"), DefaultSettings.lVStandard  );
		SetAttribute( pNewDeviceElement, _T("Device_BWMode"), DefaultSettings.bBWMode );

		hr = pRootElement->appendChild( pNewDeviceElement, 0 );

		SaveXmlFile();
	//	return DefaultSettings.streamID;
	}
	else
	{
	//	std::tstring sUID = GetAttribute<std::tstring>( Elements[0], c_szUIDAtrributeName );
	//	return StringToUuid( sUID );
		throw std::runtime_error( "Device already exists" );
	}
}

LONG		XmlVideoSystemConfig::CreateCamera( const UUID& DeviceUID, unsigned int PinNumber )
{
	const LONG CameraID = GetNewCameraID();

	std::tstring sPath = 
		( boost::tformat( _T("%s/%s[@%s=\"%s\"]") ) 
							% c_szRootName 
							% c_szDeviceElementName	
							% c_szUIDAtrributeName 
							% UuidToString( DeviceUID )
							).str()
							;
	CComPtr<IXMLDOMElement> pDevice = GetSingleElement( m_pDocument, sPath.c_str() );
	HRChecker hr;
	CComPtr<IXMLDOMElement> pNewCamera;
	hr = m_pDocument->createElement( CComBSTR(c_szCameraElementName), &pNewCamera );

	SetAttribute( pNewCamera, _T("CameraID"),  CameraID );
	SetAttribute( pNewCamera, _T("DevicePin"), PinNumber );

	hr = pDevice->appendChild( pNewCamera, 0 );

	SaveXmlFile();
	return CameraID;
}

void		 XmlVideoSystemConfig::GetLocalDeviceSettings(std::vector<IVideoSystemConfig::DeviceSettings>& params )
{
	params.clear();
	std::tstring sPath = std::tstring(c_szRootName) + _T("/") +
						 c_szDeviceElementName;
	std::vector<CComPtr<IXMLDOMElement> > Elements = 
		GetAllElements( m_pDocument, sPath.c_str() );

	for ( size_t i = 0; i < Elements.size(); ++i )
	{
		std::tstring sUid = GetAttribute<std::tstring>( Elements[i], c_szUIDAtrributeName );
		DeviceSettings ds;
		ds.streamID		= StringToUuid( sUid );
		ds.stMoniker	= GetAttribute<std::tstring>( Elements[i], _T("Device_DisplayName")  );
		ds.lWidth		= GetAttribute<unsigned int>( Elements[i], _T("Device_Width")  );
		ds.lHeight		= GetAttribute<unsigned int>( Elements[i], _T("Device_Height")  );
		ds.lFrmRate		= GetAttribute<unsigned int>( Elements[i], _T("Device_FrameRate")  );
		ds.lVStandard	= GetAttribute<unsigned int>( Elements[i], _T("Device_Standard")  );
		ds.bBWMode		= GetAttribute<bool>( Elements[i], _T("Device_BWMode")  );

		params.push_back( ds );
	}
}


void		 XmlVideoSystemConfig::GetCameraSettings(std::vector<IVideoSystemConfig::CameraSettings>& params)
{
	params.clear();
	std::tstring sPath = GetCameraPath();
	std::vector<CComPtr<IXMLDOMElement> > Elements = 
		GetAllElements( m_pDocument, sPath.c_str() );

	for ( size_t i = 0; i < Elements.size(); ++i )
	{
		CComPtr<IXMLDOMElement> pParentElement = GetParent(Elements[i]);

		std::tstring sUid = GetAttribute<std::tstring>( pParentElement, c_szUIDAtrributeName );
		CameraSettings cs;
		cs.m_DeviceUID		= StringToUuid( sUid );
		cs.m_lCameraID		= GetAttribute<LONG>( Elements[i], _T("CameraID") );
		cs.m_stCameraIP		= _T("127.0.0.1");
		cs.m_lDevicePin		= GetAttribute<unsigned int>( Elements[i], _T("DevicePin")  );

		params.push_back( cs );
	}
}

void	XmlVideoSystemConfig::DeleteCamera( LONG CameraID )
{
	std::tstring sPath = 
		( boost::tformat( _T("%s/%s/%s[@%s=%d]") ) 
		% c_szRootName 
		% c_szDeviceElementName	
		% c_szCameraElementName
		% _T("CameraID")
		% CameraID
		).str()
		;

	CComPtr<IXMLDOMElement> pCamera = GetSingleElement( m_pDocument, sPath.c_str() );
	CComPtr<IXMLDOMElement> pDevice = GetParent( pCamera );
	HRChecker hr;
	hr = pDevice->removeChild( pCamera, 0 );
	SaveXmlFile();
}

//////////////////////////////////////////////////////////////////////////

LONG XmlVideoSystemConfig::GetNewCameraID()
{
/*	std::vector<CComPtr<IXMLDOMElement> > Elements = 
		GetAllElements( m_pDocument, GetCameraPath().c_str() );
	const size_t CameraNumber = Elements.size();
	const int	 CameraID = static_cast<int>(CameraNumber);
	return CameraID;
	*/
	return m_NewCameraID++;
}

void	XmlVideoSystemConfig::SaveXmlFile()
{
	HRChecker hr;
	hr = m_pDocument->save( CComVariant(m_sFileName.c_str()) );
}