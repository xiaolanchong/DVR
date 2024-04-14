//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Загрузка/сохранение расположения камер

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   12.07.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "LayoutKeeper.h"

HRSRC FindResource( HINSTANCE, LPCTSTR, LPCTSTR );

#include <atlbase.h>

class LayoutSerializer
{
	CComPtr<IXMLDOMDocument>	m_pDocument;
public:
	void	SaveLayout( int Index, const std::vector<int>&	CameraIDArr );
	void	LoadLayout( int Index, std::vector<int>&		CameraIDArr );

	LayoutSerializer();
	~LayoutSerializer();
};

LayoutSerializer& GetSerializer()
{
	static LayoutSerializer ls;
	return ls;
}

//! class for HRESULT validation
template<class Exc> class HrChecker
{
public :
	HRESULT operator= ( HRESULT hr )
	{
		if( hr != S_OK )
		{
			//			::FormatMessageA(  )
			throw Exc();
		}
		else return hr;
	}
};

LayoutSerializer::LayoutSerializer()
try
{
	HrChecker<std::exception> hr;
	HRESULT rawres;
	hr = m_pDocument.CoCreateInstance( CLSID_DOMDocument );
	VARIANT_BOOL res;
	rawres = m_pDocument->load( CComVariant(L"layout.xml"), &res );
	if( S_OK == rawres && res == VARIANT_TRUE )
	{

	}
	else
	{

	}
}
catch (std::exception) 
{
};

LayoutSerializer::~LayoutSerializer()
{

}

void	LayoutSerializer::SaveLayout( int Index, const std::vector<int>&	CameraIDArr )
{

}

void	LayoutSerializer::LoadLayout( int Index, std::vector<int>&		CameraIDArr )
{

}

//======================================================================================//
//                                  class LayoutKeeper                                  //
//======================================================================================//

LayoutKeeper::LayoutKeeper(const char* szSystemName):
	m_sSystemName( szSystemName ) 
{
}

LayoutKeeper::~LayoutKeeper()
{
}

void	LayoutKeeper::SaveLayout( int Index, const std::vector<int>&	CameraIDArr )
{
	m_LayoutMap[Index] = CameraIDArr;
}

void	LayoutKeeper::LoadLayout( int Index, std::vector<int>&		CameraIDArr )
{
	std::map<int, std::vector<int> >::const_iterator it = m_LayoutMap.find( Index );
	if(  it != m_LayoutMap.end() )
	{
		CameraIDArr = it->second;
	}
	else
	{
		CameraIDArr.clear();
	}
}