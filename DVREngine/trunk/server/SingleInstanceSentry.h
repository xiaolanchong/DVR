//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: √арантирует единственность экземпл€ра сервера

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   05.10.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SINGLE_INSTANCE_SENTRY_3553039332284181_
#define _SINGLE_INSTANCE_SENTRY_3553039332284181_

//======================================================================================//
//                              class SingleInstanceSentry                              //
//======================================================================================//

//! \brief √арантирует единственность экземпл€ра сервера
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.10.2006
//! \version 1.0
//! \bug 
//! \todo 

class SingleInstanceSentry
{
	CHandle	m_ServerEvent;
	bool	m_bAlreadyLaunched;
public:
	SingleInstanceSentry();
	~SingleInstanceSentry();

	bool AlreadyLaunched() const
	{
		return m_bAlreadyLaunched;
	}
};

#endif // _SINGLE_INSTANCE_SENTRY_3553039332284181_