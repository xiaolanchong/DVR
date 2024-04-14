//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Camera analyzer thread actions

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.01.2006
*/                                                                                      //
//======================================================================================//
#ifndef _CAMERA_THREAD_5299650199386594_
#define _CAMERA_THREAD_5299650199386594_

#include "BaseQueueThread.h"
#include "message\IMessageStream.h"
#include "message\BaseMessageProcessor.h"

#include "..\common\Timer.h"

#include "GlobalTimer.h"
#include "FpsLogger.h"
#include "AlarmInfo.h"

struct CameraSettings
{
	//! ��������� �������� �� ������ ������?
	bool			m_LaunchAlgo;
	//! ������� ������ ���������� fps
	unsigned int	m_FpsPeriod;

	//! ��� ���������� ������
	std::string	m_ProtectionDeviceName;
};

//! \brief ����� ��� ����� ��������� � ��������� �������
//!	 chcsva ���� ��� 16 ������� ���� �� ����� 5-8 ������
//!  � ���������������� ������ 2 �������� ����� ����������, 
//!  ������� ���������� ������������ ������ ProcessImage
//! \version 1.0
//! \date 05-25-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class SleepPause
{
	size_t			m_nTotalFrames;
	const size_t	m_PauseFrame;
	static const unsigned	s_PauseTimeInMS = 50;
public:
	SleepPause(size_t PauseBetweenProcessInMS):
		m_PauseFrame( PauseBetweenProcessInMS/ s_PauseTimeInMS ),
		m_nTotalFrames(0)
		{
		}

	bool Tick()
	{
		if( m_nTotalFrames >= m_PauseFrame )
		{
			m_nTotalFrames = 0;
			return true;
		}
		else
		{
			Sleep( s_PauseTimeInMS );
			++m_nTotalFrames;
			return false;
		}
	}
};

// ����������� �� ������� �����, �������� �� ����� ����
class	EqualFrameDetection
{
	boost::optional<ulong>	m_LastFrameTime;	
public:
	bool IsEqual( ulong CurrentFrameTime )
	{
		if( !m_LastFrameTime.get_ptr() )
		{
			return false;
		}
		else 
		{
			return *m_LastFrameTime.get_ptr() == CurrentFrameTime;
		}
	}
};

class IVideoProvider;
class CameraData;
//======================================================================================//
//                                  class CameraThread                                  //
//======================================================================================//

//! \brief Camera analyzer thread actions
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   17.01.2006
//! \version 1.0
//! \bug 
//! \todo 

class CameraThread :	private BaseQueueThread,
						public IMessageStream,
						public BaseMessageProcessor
{
	//! ��������� �� ��������� �-�
	bool DoAction();

	//! ������������� ������ ������
	int												m_nCameraID;
	//! ��������������
	boost::shared_ptr<IVideoProvider>				m_pVideoReader;
	//! �������� �� ������ ������
	boost::shared_ptr<Elvees::ICameraAnalyzer>		m_pCameraAnalyzer;
	//! ��� �������� fps ���������
	FpsLogger										m_FpsCounter;

	//! ��� ����������� ������� ProcessImage ��� ���������� �����
	EqualFrameDetection								m_FrameFilter;

	bool											m_bInvalidImageMode;
public:
	CameraThread(	boost::shared_ptr<Elvees::IMessage> pMsg, 
					int nCameraID, IMessageDispatcher*  pDsp,
					boost::shared_ptr<IVideoProvider>   pVideoReader,
					const CameraSettings&				CamSettings);
	virtual ~CameraThread();

	virtual void OnMessage( boost::shared_ptr<IThreadMessage> pMsg );

	virtual void	Process( MsgCameraResponseData*		p );
	virtual void	Process( MsgCameraSetHallData*		p );

	//! ������� ������������� ������, � ������� �������� �����
	int				GetCameraID() const { return m_nCameraID; }
private:

	bool	Handled_ThreadProc();
	//! ���������� ���������������� (��������� � ��������)
	void	ProcessImage();
	//! ��������� ��������� �������
	void	ProcessQueue();

	void	SetHallDataToAlgorithm( const CameraData& CamDataFromHall); 

	void	SetFramesToVideo( const Frames_t& AlarmFrames); 
};

#endif // _CAMERA_THREAD_5299650199386594_