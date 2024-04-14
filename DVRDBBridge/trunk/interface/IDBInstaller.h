//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс для инсталляции системы( создание и инициализация БД)

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_D_B_INSTALLER_4989108816652334_
#define _I_D_B_INSTALLER_4989108816652334_

#include "../../../Shared/Common/ExceptionTemplate.h"

namespace DBBridge
{

//======================================================================================//
//                                  class IDBInstaller                                  //
//======================================================================================//

//! \brief Интерфейс для инсталляции системы( создание и инициализация БД)
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class IDBInstaller
{
public:
	MACRO_EXCEPTION(DBInstallerException, std::runtime_error);

	virtual ~IDBInstaller() {};

	//! операции инсталляции 
	//! \param sDBConnectionString		строка соединения с БД (путь к файлу)
	//! \exception DBInstallerException
	virtual void	Install( const std::string & sDBConnectionString ) = 0;

	virtual void	Uninstall() = 0;
};

//! создать реализацию с интерфейсом IDBServer
//! \return smart pointer с реализацией интерфейса
//! \exception DBInstallerException
boost::shared_ptr<IDBInstaller>	CreateDBInstaller(); 

}	// DBBridge

#endif // _I_D_B_INSTALLER_4989108816652334_