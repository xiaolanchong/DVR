//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация интерфейса IDBBackup для DVRBackup

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   06.02.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "DBBackupImpl.h"

namespace DBBridge
{

boost::shared_ptr<IDBBackup> CreateBackup(/* boost::shared_ptr<Elvees::IMessage> pDbg */)
{
	return boost::shared_ptr<IDBBackup>(new DBBackupImpl);
}

}

//======================================================================================//
//                                  class DBBackupImpl                                  //
//======================================================================================//

DBBackupImpl::DBBackupImpl()
{
}

DBBackupImpl::~DBBackupImpl()
{
}

std::string				DBBackupImpl::GetArchivePath() 
{
	return std::string();
}

std::pair<float, float>	DBBackupImpl::GetDiskSpaceParameters()
{
	return std::make_pair(0.0f, 0.0f);
}

size_t					DBBackupImpl::GetStorePeriod()
{
	return 0;
}

void					DBBackupImpl::DeleteBeforeTime( time_t nTime )
{

}