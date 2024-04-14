//!
//!	DVR 
//!	Copyright(C) 2006 Elvees
//!	All rights reserved.
//!
//!	\file     DVRBackupImp.cpp		
//!	\date     2006-02-15		
//!	\author   Yanakov Leonid		
//!	\version  1.0	
//!	\brief    IDVRBackup implementation

#include "StdAfx.hpp"
#include "../interface/DVRBackup.hpp"
#include "Storage.hpp"
#include "DVRBackupImp.hpp"
#include "../interface/BackupParam.h"

//////////////////////////////////////////////////////////////////////////
DVRBackupImp::DVRBackupImp( boost::shared_ptr<Elvees::IMessage> message ,
						   boost::shared_ptr<IBackupParam> pBackupParam):
	mMessage( message ),
	m_pParamInterface( pBackupParam ),
	m_TotalStepsDone(0),
	m_bWorking(true)
{
	//Create and start thread
	mThread = 
		boost::shared_ptr< boost::thread >( new boost::thread( boost::bind( &DVRBackupImp::HandledThreadProc, (void*)this ) ) );

	
}

DVRBackupImp::~DVRBackupImp()
{
	m_bWorking = false;
	mThread->join();
}

void DVRBackupImp::HandledThreadProc( void* p )
{	
	__try
	{
		ThreadProc(p);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("Backup thread"))
#ifndef _DEBUG
		, EXCEPTION_EXECUTE_HANDLER
#endif
		)
	{
	}
}

//////////////////////////////////////////////////////////////////////////
void DVRBackupImp::ThreadProc( void* p )
{
	DVRBackupImp* dvrBackupImp = static_cast<DVRBackupImp*>( p );

	std::string sStorageRootPath;
	boost::shared_ptr<Storage> mStorage;
	try
	{
		sStorageRootPath = dvrBackupImp->m_pParamInterface->GetArchivePath();

		const char* z = 
#if 0
			"f:\\Project\\VCproject\\_Elvees\\DVR\\HEAD\\DVRBackup\\trunk\\Tests\\TestContent";
#else	
			sStorageRootPath.c_str();
#endif
		if( sStorageRootPath.empty() )
		{
			dvrBackupImp->mMessage->Print( Elvees::IMessage::mt_error, "ArchivePath is empty" );
		}
		else
		{
			std::string msg = (boost::format("%s=%s") % "ArchivePath" % sStorageRootPath).str(); 
			dvrBackupImp->mMessage->Print( Elvees::IMessage::mt_info, msg.c_str() );
		}
		mStorage = boost::shared_ptr<Storage>( new Storage( z, dvrBackupImp->mMessage ) );
	}
	catch (IBackupParam::BackupParamException& ex) 
	{
		dvrBackupImp->mMessage->Print( Elvees::IMessage::mt_error, ex.what() );
		return;
	}

	const unsigned int c_StepPauseMSec = 500;
	dvrBackupImp->mMessage->Print( Elvees::IMessage::mt_info, "DVRBackup starts the main execution loop" );
	while( dvrBackupImp->m_bWorking )
	{
	//	dvrBackupImp->mMessage->Print( Elvees::IMessage::mt_info, "DVRBackup starts the main execution loop 2" );
		try
		{
			dvrBackupImp->BackupStep(mStorage);
		}
		catch( IBackupParam::BackupParamException& ex )
		{
			dvrBackupImp->mMessage->Print( Elvees::IMessage::mt_error, ex.what() );
		}
		catch (...) 
		{
			dvrBackupImp->mMessage->Print( Elvees::IMessage::mt_error, "Unknown error in DVRBackupImp::ThreadProc" );
		}
		Sleep( c_StepPauseMSec );
	}
	dvrBackupImp->mMessage->Print( Elvees::IMessage::mt_info, "DVRBackup now exits" );
}

void DVRBackupImp::BackupStep(boost::shared_ptr<Storage> mStorage)
{

	float		 FreeDiskSpace	= 15; // default 15%
	unsigned int StoreDays		= 21; // 21 days
	try
	{
		StoreDays		 = m_pParamInterface->GetStorePeriod();
		FreeDiskSpace	 = m_pParamInterface->GetPercentageFreeDiskSpace();
	}
	catch( std::exception& ex )
	{
		mMessage->Print( Elvees::IMessage::mt_error, ex.what() );
		return;
	}
	catch (...) 
	{
		mMessage->Print( Elvees::IMessage::mt_error, "Unknown error occuried while getting DB parameters" );
		return;
	}
	std::vector<int> camIds;
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

	mStorage->PerformClean( camIds, FreeDiskSpace, now - boost::posix_time::hours( StoreDays * 24  )  );
 
	++m_TotalStepsDone;

	if( (m_TotalStepsDone % 100) == 0 )
	{
		using namespace boost::posix_time;
		const time_t LastDataTimeT = time(0) - StoreDays * 24 * 60 * 60 ;
		ptime LastDate( from_time_t( LastDataTimeT ) );
		std::string InfoMessage;
		InfoMessage = 
			boost::str( boost::format( "call DeleteBeforeTime with date=%s" ) % to_simple_string( LastDate ) );
		mMessage->Print( Elvees::IMessage::mt_info, InfoMessage.c_str() );

		m_pParamInterface->DeleteBeforeTime( LastDataTimeT );
	}
}

//////////////////////////////////////////////////////////////////////////

class BackupParamDebug : public IBackupParam
{
public:
	virtual std::string				GetArchivePath()
	{	
		return "C:\\Project\\VCproject\\_Elvees\\DVR\\HEAD\\bin\\archive";
	}

	virtual double	GetPercentageFreeDiskSpace () 
	{
		return 90;
	}

	virtual unsigned int					GetStorePeriod()
	{
		return 21;
	}

	virtual void					DeleteBeforeTime( time_t nTime )
	{

	}

};

class StdMessage : public Elvees::IMessage
{
public:
	virtual void Print( sint messageType, const char * szMessage )
	{
		OutputDebugStringA( szMessage );
		OutputDebugStringA( "\n" );
	}
};

void __stdcall DebugBackup(HWND hwnd, HINSTANCE hinst, LPSTR lpCmdLine, int nCmdShow)
{
	boost::shared_ptr<IBackupParam>		pBackupParam( new BackupParamDebug );
	boost::shared_ptr<Elvees::IMessage> pPrintParam( new StdMessage );
	boost::shared_ptr<DVRBackupImp> pBackup( new DVRBackupImp( pPrintParam, pBackupParam ) ); 

	pBackup->DebugThreadProc();
}