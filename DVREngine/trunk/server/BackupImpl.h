//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���������� IBackupParam ��� DVRBackup - ������ ���������� ����� IVideoConfig

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   27.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _BACKUP_IMPL_4005088508067656_
#define _BACKUP_IMPL_4005088508067656_

#include "../interface/EngineSettings.h"

//======================================================================================//
//                                   class BackupImpl                                   //
//======================================================================================//

//! \brief ���������� IBackupParam ��� DVRBackup - ������ ���������� ����� IVideoConfig
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   27.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class BackupImpl : private IBackupParam
{
	//DBBridge::IDBServer*				m_pServer;
	//DVREngine::IEngineSettingsReader*  m_pReader;	

	boost::shared_ptr< IDVRBackup > m_pBackup;
public:
	BackupImpl( boost::shared_ptr<Elvees::IMessage> pDebugInterface);
	virtual ~BackupImpl();

	virtual std::string				GetArchivePath();
	virtual double					GetPercentageFreeDiskSpace() ;

	virtual unsigned int				GetStorePeriod();
	virtual void					DeleteBeforeTime( time_t nTime );
};

#endif // _BACKUP_IMPL_4005088508067656_