//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация IBackupParam для DVRBackup - чтение параметров через IVideoConfig

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   27.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "BackupImpl.h"
#include "../common/EngineSettingsImpl.h"

template <typename T> void	ReleaseNothing(T* )
{

}

//======================================================================================//
//                                   class BackupImpl                                   //
//======================================================================================//

BackupImpl::BackupImpl( boost::shared_ptr<Elvees::IMessage> pDebugInterface)
{
	m_pBackup = CreateBackupInterface( pDebugInterface, 
					boost::shared_ptr<IBackupParam> ( static_cast<IBackupParam*>(this), &ReleaseNothing<IBackupParam>)  );
}

BackupImpl::~BackupImpl()
{
	m_pBackup.reset();
}

std::string				BackupImpl::GetArchivePath()
try
{
	boost::shared_ptr<DBBridge::IVideoConfig>	p( DBBridge::CreateVideoConfig() );
	std::string sPath;
	p->GetValue( 0, "ArchivePath", sPath );
	return sPath;
}
catch ( std::exception& ex ) 
{
	throw BackupParamException::BackupParamException( ex.what() );
};

double	BackupImpl::GetPercentageFreeDiskSpace()
try
{
	DVREngine::ArchiveStorageParam param;

	EngineSettingsImpl impl(true);
	impl.GetArchiveStorageParameters( param );
	return param.FreePercentageDiskSpace;
}
catch ( std::exception& ex) 
{
	throw BackupParamException::BackupParamException( ex.what() );
};

unsigned int			BackupImpl::GetStorePeriod()
try
{
	DVREngine::ArchiveStorageParam param;

	EngineSettingsImpl impl(true);
	impl.GetArchiveStorageParameters( param );
	return param.StorageDayPeriod;
}
catch( std::exception& ex )
{
	throw BackupParamException::BackupParamException( ex.what() );
};

void					BackupImpl::DeleteBeforeTime( time_t nTime )
try
{
	boost::shared_ptr<DBBridge::IDBServer> pSrv = DBBridge::CreateDBServer(0);
	pSrv->DeleteInfoBeforeTime( nTime);
}
catch ( std::exception& ex) 
{
	throw BackupParamException::BackupParamException( ex.what() );
};