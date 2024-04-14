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

#include "stdafx.h"
#include "DVRRenderStubThread.h"

DVRRenderStubThread& GetStub()
{
	static DVRRenderStubThread r;
	return r;
}

IDVRRender*	CreateRender( wxGLCanvas* pWnd  )
{
	return GetStub().CreateRender(pWnd);
}

void		DeleteRender( IDVRRender* pRender )
{
	GetStub().DeleteRender(pRender);
}

//======================================================================================//
//                              class DVRRenderStubThread                               //
//======================================================================================//

DVRRenderStubThread::DVRRenderStubThread(): m_bWork(true)
{
	m_Thread = std::auto_ptr< boost::thread > ( new boost::thread(  boost::bind( ThreadProc, this ) ) );
}

DVRRenderStubThread::~DVRRenderStubThread()
{
}

void DVRRenderStubThread::ThreadProc()
{
	for (;;)
	{
		lock_t lock( m_MutexWorking );
		if( !m_bWork ) break;
		for( size_t i =0; i < m_Renders.size(); ++i )
			if( m_Renders[i]->IsEnable() )
				m_Renders[i]->Render();
	}
}

IDVRRender*	DVRRenderStubThread::CreateRender(wxGLCanvas* pWnd)
{
	DVRRenderStub* pRender = new DVRRenderStub(pWnd);
	lock_t l(m_MutexWorking);
	m_Renders.push_back( pRender );
	return pRender;
}

void		DVRRenderStubThread::DeleteRender( IDVRRender* pRender )
{
	bool bJoin = false;
	{
		lock_t lock( m_MutexWorking );
		m_Renders.erase( std::find( m_Renders.begin(), m_Renders.end(), pRender ) ) ;	
		bJoin = m_Renders.empty();
		m_bWork = !bJoin;
	}
	if( bJoin) m_Thread->join();
}