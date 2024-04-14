//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Запрос инф-и от зала, сервер->зал

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   20.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _MSG_HALL_ALARM_DATA_3799080997567645_
#define _MSG_HALL_ALARM_DATA_3799080997567645_

#include "IThreadMessage.h"

//======================================================================================//
//                            class MsgHallRequestAlarmData                             //
//======================================================================================//

//! \brief Запрос инф-и от зала, сервер->зал
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   20.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class MsgHallRequestAlarmData : public IThreadMessage
{
public:
	MsgHallRequestAlarmData(  );
	virtual ~MsgHallRequestAlarmData();

	virtual void	Process(IMessageProcessor* p) { p->Process( this ); };

};

#endif // _MSG_HALL_ALARM_DATA_3799080997567645_