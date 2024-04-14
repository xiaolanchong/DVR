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
#ifndef _D_B_BACKUP_IMPL_5044519115472496_
#define _D_B_BACKUP_IMPL_5044519115472496_

#include "../Interface/IDBBackup.h"

//======================================================================================//
//                                  class DBBackupImpl                                  //
//======================================================================================//

//! \brief Реализация интерфейса IDBBackup для DVRBackup
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.02.2006
//! \version 1.0
//! \bug 
//! \todo 

class DBBackupImpl : public DBBridge::IDBBackup
{
public:
	DBBackupImpl();
	virtual ~DBBackupImpl();

	//!	получить путь к корневой папке архива
	//!	\return путь
	virtual std::string				GetArchivePath() ;
	//!	получить параметры удаления по свободному месту на диске
	//! парамтеры - 2 числа: 0.0f <= A <= B <= 100.0f
	//!	\return пара начало/конец
	virtual std::pair<float, float>	GetDiskSpaceParameters() ;

	//!	получить параметры удаления по времени
	//!	\return 
	virtual size_t					GetStorePeriod() ;

	//!	удалить все записи до времени
	//!	\param nTime время, до которого надо удалить (локальное)
	virtual void					DeleteBeforeTime( time_t nTime ) ;
};

#endif // _D_B_BACKUP_IMPL_5044519115472496_