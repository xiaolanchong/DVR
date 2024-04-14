//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
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
	//! запускать алгоритм на данной камере?
	bool			m_LaunchAlgo;
	//! периоды выдачи статистики fps
	unsigned int	m_FpsPeriod;

	//! имя устройства защиты
	std::string	m_ProtectionDeviceName;
};

//! \brief класс для паузы обработки в несколько вызовов
//!	 chcsva даже при 16 камерах дает не менее 5-8 кадров
//!  в действительности только 2 содержат новую информация, 
//!  поэтому необходимо ограничивать вызовы ProcessImage
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

// определение по времени кадра, поступил ли новый кадр
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
	//! обработка из потоковой ф-и
	bool DoAction();

	//! идентификатор камеры потока
	int												m_nCameraID;
	//! видеопоставщик
	boost::shared_ptr<IVideoProvider>				m_pVideoReader;
	//! алгоритм на уровне камеры
	boost::shared_ptr<Elvees::ICameraAnalyzer>		m_pCameraAnalyzer;
	//! для подсчета fps обработки
	FpsLogger										m_FpsCounter;

	//! для ограничения вызовов ProcessImage при одинаковом кадре
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

	//! вернуть идентификатор камеры, с которой работает поток
	int				GetCameraID() const { return m_nCameraID; }
private:

	bool	Handled_ThreadProc();
	//! обработать видеоизображения (поставить в алгоритм)
	void	ProcessImage();
	//! обработка сообщений очереди
	void	ProcessQueue();

	void	SetHallDataToAlgorithm( const CameraData& CamDataFromHall); 

	void	SetFramesToVideo( const Frames_t& AlarmFrames); 
};

#endif // _CAMERA_THREAD_5299650199386594_