//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: класс взаимодействия с БД - получение списка камер, чтение из архива т.д.

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   31.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _D_B_HANDLER_3252107636112560_
#define _D_B_HANDLER_3252107636112560_

//======================================================================================//
//                                   class DBHandler                                    //
//======================================================================================//

//! \brief класс взаимодействия с БД - получение списка камер, чтение из архива т.д.
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   31.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class DBHandler
{
public:
	DBHandler();
	virtual ~DBHandler();

	//! получить список камер
	//! \param CameraArrID 
	void	GetCameraArr( std::vector<int>& CameraArrID );

	//! получить данные по камере из БД
	//! \param StartTime начало промежутка, за кот. запрашивается
	//! \param EndTime конец промежутка
	//! \param AlarmDataArr вызодные данные
	void	GetAlarmData( time_t StartTime, time_t EndTime, Frames_t& AlarmDataArr );
};

#endif // _D_B_HANDLER_3252107636112560_