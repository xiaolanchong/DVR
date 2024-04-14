//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Ничего не делает, заглушка

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _EMPTY_INSTALLER_7549681892029299_
#define _EMPTY_INSTALLER_7549681892029299_

#include "../interface/IDBInstaller.h"

//======================================================================================//
//                                struct EmptyInstaller                                 //
//======================================================================================//

//! \brief Ничего не делает, заглушка
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.05.2006
//! \version 1.0
//! \bug 
//! \todo 

struct EmptyInstaller : public DBBridge::IDBInstaller
{
public:
	
	virtual void Install( const std::string& )
	{
	}

	virtual void Uninstall()
	{
	}
};

#endif // _EMPTY_INSTALLER_7549681892029299_