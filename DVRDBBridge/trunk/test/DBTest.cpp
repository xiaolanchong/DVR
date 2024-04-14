//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Not available

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   08.12.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "DBTest.h"
#include "../DVRDBBridge.h"

using namespace DBBridge;
//======================================================================================//
//                                     class DBTest                                     //
//======================================================================================//

DBTest::DBTest() :
	m_pConfig( CreateVideoConfig() )
{
}

DBTest::~DBTest()
{
}

void	DBTest::Test()
try
{
	TestVideoCfg();
	TestCamera();
}
catch ( std::exception & ex ) 
{
	std::cerr << "Exception= " << ex.what() << std::endl;
};

void	DBTest::TestVideoCfg()
{
	std::cout << "Testing computer property..." << std::endl;
	try
	{
		DWORD dwId;
		m_pConfig->GetValue( NULL, "__no_such_dw_prop__", dwId );
		std::cout << ">Failed!" << std::endl;
	}
	catch ( IVideoConfig::NoSuchProperty )
	{
	}

	try
	{
		std::string s;
		m_pConfig->GetValue( NULL, "__no_such_string_prop__", s );
		std::cout << ">Failed!" << std::endl;
	}
	catch ( IVideoConfig::NoSuchProperty )
	{
	}

	TestStringParam();
	TestNumericParam();
}

void	DBTest::TestStringParam()
{
	const char* StringParams[] =
	{
		"ArchivePath"
	};
	std::string s;
	std::cout << "[TestStringParam]" << std::endl;
	for ( size_t i =0; i < sizeof(StringParams)/sizeof(StringParams[0]); ++i )
	{
		m_pConfig->GetValue( NULL, StringParams[i], s );
		std::cout << StringParams[i] << " = " << s << std::endl;
	}
}

void	DBTest::TestNumericParam()
{
	const char* StringParams[] =
	{
		"Codec_FCC",
		"Codec_DataRate",
		"Codec_KeyRate",
		"Codec_Quality",
		"Video_FMP",
		"Video_MaxSize"
	};
	DWORD p;
	std::cout << "[TestNumericParam]" << std::endl;
	for ( size_t i =0; i < sizeof(StringParams)/sizeof(StringParams[0]); ++i )
	{
		m_pConfig->GetValue( NULL, StringParams[i], p );
		std::cout << StringParams[i] << " = " << p << std::endl;
	}
}

void	DBTest::TestCamera()
{
	std::cout << "Creating camera..." << std::endl;
	std::auto_ptr<ICameraConfig> pc ( m_pConfig->CreateCamera( "__test__camera__" ) );

	m_pConfig->DeleteCamera( pc->GetCameraID() );
}