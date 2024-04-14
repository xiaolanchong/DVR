//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ��������� ��������� ����� ��������

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_THREAD_MESSAGE_2024151395885207_
#define _I_THREAD_MESSAGE_2024151395885207_
//======================================================================================//
//                                 class IThreadMessage                                 //
//======================================================================================//

//! \brief ��������� ��������� ����� ��������, ������  Visitor
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.01.2006
//! \version 1.0
//! \bug 
//! \todo 

#include "IMessageProcessor.h"

class IThreadMessage
{
public:
	virtual ~IThreadMessage(){};
	//! ���������� ���������, �������� ������� ���������������
	//! \param p ��������� �� ����������
	virtual void	Process(IMessageProcessor* p) = 0;
};

typedef boost::shared_ptr<IThreadMessage>	ThreadMessage_t;

#endif // _I_THREAD_MESSAGE_2024151395885207_