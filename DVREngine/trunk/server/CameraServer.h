//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Cameras thread & queue server

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _CAMERA_SERVER_6979826536353589_
#define _CAMERA_SERVER_6979826536353589_

#include "MessageImpl.h"
#include "CameraThread.h"
#include "grabber\ServerVideoFactory.h"
#include "message/IMessageDispatcher.h"
#include "message/IMessageStream.h"

//======================================================================================//
//                                  class CameraServer                                  //
//======================================================================================//

//! \brief Cameras thread & queue server, ��������� ������� � ������ �����
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   17.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class CameraServer :	public	IMessageStream,
						public	IMessageDispatcher
{
	typedef std::map<int, boost::shared_ptr<CameraThread> >	CameraThreads_t;

	//! ���������� �����
	MessageImpl			m_Msg;
	//! ������ ������� �����
	CameraThreads_t		m_CameraThreads;
	//! ��������� �����
	ServerVideoFactory	m_VideoFactory;
public:
	virtual	 void	OnMessage( boost::shared_ptr<IThreadMessage> pMsg );
private:
	virtual	 void	SendMessage( IMessageStream* pSender, boost::shared_ptr<IThreadMessage> pMsg );

	//! ��������� ����� ��� ������ � ��������������� nCameraID
	//! \param nCameraID ������������� ������, ��� ������� ����������� �����
	void			StartThread( int nCameraID, const CameraSettings& CamSettings);
public:
	//! \param pOwnMsg		�������� ���������� �����
	//! \param pVideoMsg	���������� ����� ��� �����
	//! \param pDsp			�������� ���������
	CameraServer(
				DWORD dwVideoModeFlags,
				boost::shared_ptr<Elvees::IMessage> pOwnMsg ,
				boost::shared_ptr<Elvees::IMessage> pVideoMsg ,
				IMessageDispatcher* pDsp);
	virtual ~CameraServer();

	//! ��������� ������ �����
	//! \param CameraArr ������ ��������������� �����, ��� ������� ��������� ������
	void	Start( const std::vector<int>& CameraArr, const CameraSettings& CamSettings );
};

#endif // _CAMERA_SERVER_6979826536353589_