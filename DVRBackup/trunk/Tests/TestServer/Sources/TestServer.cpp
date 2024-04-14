//!
//!	DVR 
//!	Copyright(C) 2006 Elvees
//!	All rights reserved.
//!
//!	\file     TestServer.hpp		
//!	\date     2006-02-16		
//!	\author   Yanakov Leonid		
//!	\version  1.0	
//!	\brief    This is default brief			
#include "StdAfx.hpp"
#include "DVRBackup.hpp"
#include "BackupParam.h"
#include "TestServer.hpp"

#if 0
//////////////////////////////////////////////////////////////////////////
class IDVRBackup;
typedef IDVRBackup* (CREATE_BACKUP_FUNC)( boost::shared_ptr<Elvees::IMessage> message );


//////////////////////////////////////////////////////////////////////////
TestServerApp::TestServerApp():
	mDebugInfo( new DebugInfo() )
{
	//
}

TestServerApp::~TestServerApp()
{
	//
}
//////////////////////////////////////////////////////////////////////////
bool TestServerApp::OnInit()
{
	wxFrame* frame = static_cast<wxFrame*>( new TestServerFrame( "DVRBackup TestServer" ) ); 
	SetTopWindow( frame );

#ifdef _PROFILE
	mDVRBackupDynLib = boost::shared_ptr<wxDynamicLibrary>( new wxDynamicLibrary( "DVRBackupp.dll" ) );
#elif _DEBUG
	mDVRBackupDynLib = boost::shared_ptr<wxDynamicLibrary>( new wxDynamicLibrary( "DVRBackupd.dll" ) );
#else
	mDVRBackupDynLib = boost::shared_ptr<wxDynamicLibrary>( new wxDynamicLibrary( "DVRBackup.dll" ) );
#endif

	CREATE_BACKUP_FUNC* createBackupFunc = static_cast<CREATE_BACKUP_FUNC*>( mDVRBackupDynLib->GetSymbol( "CreateBackup" ) );

	//Create dvrBackup instance
	mDVRBackup = boost::shared_ptr<IDVRBackup>( (createBackupFunc)( mDebugInfo ) );


	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
TestServerFrame::TestServerFrame( const wxString& title )
{

}

TestServerFrame::~TestServerFrame()
{

}
#else

class BackupParamImpl : public IBackupParam
{
	virtual std::string				GetArchivePath() 
	{
		return "f:\\Project\\VCproject\\_Elvees\\DVR\\HEAD\\DVRBackup\\trunk\\Tests\\TestContent";
	}
	virtual std::pair<float, float>	GetDiskSpaceParameters()
	{
		return std::make_pair( 50.0, 60.0);
	}
	virtual unsigned int					GetStorePeriod()
	{
		return 3;
	}

	virtual void		DeleteBeforeTime( time_t nTime )
	{
	}
public:	
};

int main( int argc, char* argv[] )
{
	boost::shared_ptr<DebugInfo>	mDebugInfo(new DebugInfo );
	boost::shared_ptr<IBackupParam> mBackupParam(new BackupParamImpl );
	boost::shared_ptr<IDVRBackup>	mDVRBackup ( CreateBackupInterface( mDebugInfo, mBackupParam ) );

	_getch();

	return 0;
}

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DebugInfo::Print(sint messageType, const char *szMessage)
{
	std::string messageTypeStr;

	switch( messageType )
	{
		case Elvees::IMessage::mt_info:
			messageTypeStr = "Info:";
			break;
		case Elvees::IMessage::mt_debug_info:
			messageTypeStr = "DebugInfo:";
			break;
		case Elvees::IMessage::mt_error:
			messageTypeStr = "Error:";
			break;
		case Elvees::IMessage::mt_warning:
			messageTypeStr = "Warning:";
			break;
		default:
			messageTypeStr = "Unknown:";
	}

	std::string resultStr = boost::str( boost::format( "%s %s\n" ) % messageTypeStr % szMessage );
	std::cout<<resultStr; 
 }