//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: xml utility functions

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   11.09.2006
*/                                                                                      //
//======================================================================================//
#ifndef _XML_UTILITY_8780452105075198_
#define _XML_UTILITY_8780452105075198_

//#include "../IVideoConfig.h"

LPCTSTR const c_szRootName				= _T("config");
//LPCTSTR const c_szDevicesElementName	= _T("devices");
LPCTSTR const c_szDeviceElementName		= _T("device");
LPCTSTR const c_szUIDAtrributeName		= _T("uid");
LPCTSTR const c_szCameraElementName		= _T("camera");

class HRChecker
{
public:
	HRESULT operator = (HRESULT hr)
	{
		if( hr == S_OK )
		{
			return hr;
		}
		else
		{
			throw std::runtime_error("XML format error");
		}
	}
};
/*
void	SaveXmlFile( IXMLDOMDocument* pDocument, LPCTSTR szFileName)
{
	HRChecker hr;
	hr = pDocument->save( CComVariant(szFileName) );
}*/

template<typename AttrType>  AttrType GetAttribute( IXMLDOMElement* pElement, LPCTSTR szName )
try
{
	HRChecker hr;
	CComVariant val ;
	hr = pElement->getAttribute(CComBSTR(szName), &val);
	USES_CONVERSION;
	std::tstring sz = COLE2CT(val.bstrVal);
	return  boost::lexical_cast<AttrType>( sz );
}
catch ( boost::bad_lexical_cast& ex ) 
{
	throw std::runtime_error(ex.what() );
};

template<typename AttributeType>  void SetAttribute( IXMLDOMElement* pElement, LPCTSTR szName, AttributeType Value )
try
{
	HRChecker hr;
	
	std::tstring s = boost::lexical_cast<std::tstring>( Value );
	CComVariant val(s.c_str()) ;
	hr = pElement->setAttribute(CComBSTR(szName), val);

}
catch ( boost::bad_lexical_cast& ex ) 
{
	throw std::runtime_error(ex.what() );
};

template<typename SelectorType> 
	std::vector<CComPtr<IXMLDOMElement> > GetAllElements( SelectorType pSelector, LPCTSTR szXMLPath )
{
	std::vector<CComPtr<IXMLDOMElement> > ElementArr;
	HRChecker hr;
	CComPtr<IXMLDOMNodeList> pList;
	hr = pSelector->selectNodes( CComBSTR(szXMLPath), &pList );
#ifdef _DEBUG
	long z;
	pList->get_length(&z);
#endif
	CComPtr<IXMLDOMNode> pNode;
	while( S_OK == pList->nextNode( &pNode ) )
	{
		CComQIPtr<IXMLDOMElement> pElement( pNode );
		if( pElement )
		{
			ElementArr.push_back( pElement );
		}
		pNode.Release();
	}	
	return ElementArr;
}

template<typename SelectorType> 
	CComPtr<IXMLDOMElement>	GetSingleElement(SelectorType pDocument, LPCTSTR szPath)
{
	std::vector<CComPtr<IXMLDOMElement> >	ElementArr = GetAllElements( pDocument, szPath );
	if( ElementArr.empty() )
	{
		throw std::runtime_error( "No such element" );
	}
	return ElementArr[0];
}

template<typename AttributeType, typename SelectorType > 
AttributeType	GetAttributeValue( SelectorType pDocument, LPCTSTR szPath, LPCTSTR szValueName )
{
	CComPtr<IXMLDOMElement> pElement = GetSingleElement( pDocument, szPath );
	return GetAttribute<AttributeType>( pElement, szValueName );
}

template<typename AttributeType, typename SelectorType > 
void SetAttributeValue( SelectorType pDocument, LPCTSTR szPath, LPCTSTR szValueName, AttributeType Value )
{
	CComPtr<IXMLDOMElement> pElement = GetSingleElement( pDocument, szPath );

	HRChecker hr;
	CComVariant val( Value ) ;
	hr = pElement->setAttribute(CComBSTR(szValueName), val);
//	return  boost::lexical_cast<AttrType>( val.bstrVal );
}

void			GetBinaryValue( IXMLDOMDocument* pDocument, LPCTSTR szPath, std::vector<BYTE>& Value );

#endif // _XML_UTILITY_8780452105075198_