//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Ñîçäàíèå ÁÄ SQLite 3.3

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _S_Q_LITE_INSTALLER_6278540705132501_
#define _S_Q_LITE_INSTALLER_6278540705132501_

#include "../interface/IDBInstaller.h"

//======================================================================================//
//                                class SQLiteInstaller                                 //
//======================================================================================//

//! \brief Ñîçäàíèå ÁÄ SQLite 3.3
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class SQLiteInstaller : public DBBridge::IDBInstaller
{
public:
	SQLiteInstaller();
	virtual ~SQLiteInstaller();

	virtual void	Install( const std::string & sDBConnectionString );
	virtual void	Uninstall();
};

#endif // _S_Q_LITE_INSTALLER_6278540705132501_