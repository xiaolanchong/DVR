// DVRDBBridgeTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../interface/IDBServer.h"
#include "../../interface/IDBClient.h"
#include "../../interface/IVideoConfig.h"
#include "../../common/CommonUtility.h"
#include <time.h>

void	GetAllCameras( DBBridge::IVideoConfig* pVideoConfig, std::vector<int>& CamIDArr )
{
	CamIDArr.clear();
	_ASSERTE(pVideoConfig);
	if( !pVideoConfig )
	{
		return;
	}

	std::vector<std::tstring>	AllDevices;
	pVideoConfig->GetDevices( AllDevices );

	std::vector<int> CamIDArrFromOneDevice;
	for ( size_t idxDevice = 0; idxDevice < AllDevices.size(); ++idxDevice )
	{
		CamIDArrFromOneDevice.clear();

		std::auto_ptr<DBBridge::IDeviceConfig> pDeviceConfig ( pVideoConfig->GetDevice( AllDevices[idxDevice].c_str() ) );
		pDeviceConfig->GetCameras( CamIDArrFromOneDevice );
		CamIDArr.insert( CamIDArr.end(), CamIDArrFromOneDevice.begin(), CamIDArrFromOneDevice.end() );
	}
}

using boost::unit_test::test_suite;

int InsertRecords( const char* szConn )
{
	try
	{
		boost::shared_ptr< DBBridge::IDBServer > pServer( DBBridge::CreateDBServer( szConn ) );
		std::vector<DBBridge::Frame> frm;
		time_t nNow = time(0);
		frm.push_back( DBBridge::Frame( 1, nNow - 5, nNow, DBBridge::Frame::Rect(10, 15, 20, 25) ) );
		frm.push_back( DBBridge::Frame( 1, nNow - 4, nNow + 1, DBBridge::Frame::Rect(10, 15, 20, 25) ) );
		frm.push_back( DBBridge::Frame( 1, nNow - 600, nNow + 300, DBBridge::Frame::Rect(1, 2, 3, 4) ) );

		pServer->AddFrames( frm );
	}
	catch( DBBridge::IDBServer::DBServerException & ex)
	{
		std::cout << ex.what() << std::endl;
		return -1;
	}
	return 0;
}

int FetchRecords( const char* szConn )
{
	try
	{
		boost::shared_ptr< DBBridge::IDBClient > pClient( DBBridge::CreateDBClient( szConn ) );	
		time_t nNow = time(0);
		std::vector<DBBridge::Frame> frm;
		pClient->GetFrames( nNow - 24 * 60 * 60, nNow, frm );
	}
	catch (DBBridge::IDBClient::DBClientException & ex) 
	{
		std::cout << ex.what() << std::endl;
		return -1;
	}
	return 0;
}

void	DatabaseTest()
{
	const char* szConn = 
		"DRIVER=Microsoft Access Driver (*.mdb);UID=admin;FIL=MS Access;DBQ="
		"F:\\Project\\VCproject\\_Elvees\\DVR\\DVRDBBridge\\trunk\\test\\DVRDBBridgeTest\\DVR.mdb;";

	int res = 
#if 0
		InsertRecords( szConn );
#else 
		FetchRecords( szConn );
#endif
}

using namespace DBBridge;

void config_create_test()
{

	std::auto_ptr<IVideoConfig> pVideoConfig( CreateVideoConfig() );

	std::auto_ptr<IDeviceConfig> pDeviceConfig0 ( pVideoConfig->CreateDevice( "device0" ) );
	std::auto_ptr<IDeviceConfig> pDeviceConfig1 ( pVideoConfig->CreateDevice( "device1" ) );

	std::auto_ptr<ICameraConfig> pCameraConfig1 ( pDeviceConfig0->CreateCamera( 1, 0 ) );
	std::auto_ptr<ICameraConfig> pCameraConfig2 ( pDeviceConfig0->CreateCamera( 2, 1 ) );
	std::auto_ptr<ICameraConfig> pCameraConfig3 ( pDeviceConfig1->CreateCamera( 3, 0 ) );

	std::vector<int> AllCamera, AllPatternCamera ;
	AllPatternCamera.push_back( 1 );
	AllPatternCamera.push_back( 2 );
	AllPatternCamera.push_back( 3 );

	GetAllCameras( pVideoConfig.get(), AllCamera );

	BOOST_CHECK_EQUAL_COLLECTIONS(	AllCamera.begin(), AllCamera.end() ,
									AllPatternCamera.begin(), AllPatternCamera.end()  );

}

void config_delete_test()
{
	std::auto_ptr<IVideoConfig> pVideoConfig( CreateVideoConfig() );
	std::auto_ptr<IDeviceConfig> pDeviceConfig0 ( pVideoConfig->GetDevice( "device0" ) );

	pDeviceConfig0->DeleteCamera( 1 );

	std::vector<int> AllCamera;

	pDeviceConfig0->GetCameras( AllCamera );
	BOOST_CHECK_EQUAL( AllCamera.size(), 1 );

	pVideoConfig->DeleteDevice( "device0" );

	IDeviceConfig* pDeletedDevice =	pVideoConfig->GetDevice( "device0" );
	BOOST_CHECK_EQUAL( pDeletedDevice , static_cast<IDeviceConfig*>( NULL ) );
}

void config_video_set()
{
	const unsigned char ByteArr[] = { 0x0, 0x2, 0x54, 0x14, 0x96 };

	std::auto_ptr<IVideoConfig> pVideoConfig( CreateVideoConfig() );
	pVideoConfig->SetValue( "", "ArchivePath", "e:\\blah-blah\\ArchiveFolder Here" );

	pVideoConfig->SetValue( "Storage", "Codec_DataRate", 0x123456 );
	pVideoConfig->SetValue( "Storage", "Codec_State", 
		std::vector<unsigned char>( ByteArr, ByteArr + sizeof(ByteArr)/sizeof(ByteArr[0] ) ) );
}

test_suite* init_unit_test_suite( int, char* [] ) 
{
	test_suite* test= BOOST_TEST_SUITE( "VideoConfig unit test" );

	// this example will pass cause we know ahead of time number of expected failures
	test->add( BOOST_TEST_CASE( &config_create_test ), 0 );
	test->add( BOOST_TEST_CASE( &config_delete_test ), 0 );
	test->add( BOOST_TEST_CASE( &config_video_set ), 0 );

	return test;
}
