//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Общая инф-я о тревожных ситуациях

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   24.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ALARM_INFO_8462662459278858_
#define _ALARM_INFO_8462662459278858_

#include "../../../DVRDBBridge/trunk/interface/Frame.h"

//======================================================================================//
//                                   class AlarmInfo                                    //
//======================================================================================//

//! \brief Общая инф-я о тревожных ситуациях
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   24.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class AlarmInfo
{
public:
	AlarmInfo();
	virtual ~AlarmInfo();
};

typedef std::vector< DBBridge::Frame >	Frames_t;

#endif // _ALARM_INFO_8462662459278858_