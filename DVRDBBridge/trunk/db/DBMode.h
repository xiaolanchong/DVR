//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Получение режима работы с БД, строки соединения с БД, первоначальное создание БД

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _D_B_MODE_5802563796122068_
#define _D_B_MODE_5802563796122068_

//======================================================================================//
//                                     class DBMode                                     //
//======================================================================================//

//! \brief Получение режима работы с БД, строки соединения с БД, первоначальное создание БД
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.03.2006
//! \version 1.0
//! \bug 
//! \todo 

class DBMode
{
public:
	DBMode();
	virtual ~DBMode();

	enum ConfigMode 
	{
		cm_registry,
		cm_mixed,
		cm_database
	};

	ConfigMode		GetConfigMode();
	bool			MustUseDatabase()
	{
		return cm_database == GetConfigMode();
	}
	std::string		GetConnectionString();

	//std::string		GetLocalIP();
};

#endif // _D_B_MODE_5802563796122068_