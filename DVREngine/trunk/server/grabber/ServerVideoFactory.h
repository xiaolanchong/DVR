//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ����� �������� ��������� ������������

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_VIDEO_FACTORY_7158850564915478_
#define _SERVER_VIDEO_FACTORY_7158850564915478_

#include "../MessageImpl.h"
#include "IVideoProvider.h"

//======================================================================================//
//                               class ServerVideoFactory                               //
//======================================================================================//

//! \brief ����� �������� ��������� ������������, �������� ����������� �� Abstract Factory
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   18.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerVideoFactory
{
	//! ���������� �����
	MessageImpl		m_Msg;
	//! �������� ������������ CHCSVA
	boost::shared_ptr<CHCS::IStreamManager> m_pManager;
	//! ���������� ��������� ��� CHCSVA (���������� ��� ������������� � ������� � m_Msg)
	static void OutputDebugProc( DWORD_PTR, int, LPCWSTR );

	boost::shared_ptr<CHCS::IStreamManager> m_pClientManager;
public:

	MACRO_EXCEPTION(	ServerVideoException ,std::runtime_error );

	//! \param pMsg ���������� �����
	ServerVideoFactory( DWORD dwVideoModeFlags, boost::shared_ptr<Elvees::IMessage> pMsg );
	virtual ~ServerVideoFactory();

	//! ������� ����� ����� ��� ������
	//! \param nCameraID ������������� ������, ��� ���. ��������� �����
	//! \return ���� �� NULL, �� ��������� ����������
	boost::shared_ptr<IVideoProvider>	CreateVideoReader( int nCameraID );
};

#endif // _SERVER_VIDEO_FACTORY_7158850564915478_