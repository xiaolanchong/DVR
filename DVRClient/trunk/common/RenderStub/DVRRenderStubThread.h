//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: 

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   25.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _D_V_R_RENDER_STUB_THREAD_5024014141319607_
#define _D_V_R_RENDER_STUB_THREAD_5024014141319607_

#include "DVRRenderStub.h"

//======================================================================================//
//                              class DVRRenderStubThread                               //
//======================================================================================//

//! \brief 
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   25.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class DVRRenderStubThread
{
public:
	DVRRenderStubThread();
	virtual ~DVRRenderStubThread();

	IDVRRender*	CreateRender(wxGLCanvas* pWnd);
	
	void		DeleteRender( IDVRRender* pRender );
private:

	void	ThreadProc();

	std::vector< DVRRenderStub* >	m_Renders;

	//boost::condition				m_CondWorking;
	bool							m_bWork;
	boost::mutex					m_MutexWorking;

	typedef boost::mutex::scoped_lock lock_t;
	std::auto_ptr<boost::thread>	m_Thread;
};

#endif // _D_V_R_RENDER_STUB_THREAD_5024014141319607_