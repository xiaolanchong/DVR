//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Управление архивом по группе камер

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   19.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_RENDER_6298332396054487_
#define _ARCHIVE_RENDER_6298332396054487_

#include "ArchiveState.h"


//======================================================================================//
//                                 class ArchiveRender                                  //
//======================================================================================//

//! \brief Управление архивом по группе камер
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   19.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveRender : public IDVRArchiveRender
{
	StreamMap_t														m_Streams;
	FrameMap_t														m_Frames;

	//! период, в кот. работает архив, задается в AcquireArchive
	std::pair<time_t, time_t>	m_WorkTimeInterval;

	//! текущее состояние архива - idle, play, pause, stop
	std::auto_ptr<ArchiveState>	m_State;

	//! ф-Я для вызова рисования архивных данных
	boost::function0<void>		m_DrawCallbackFn;
	//! интерфейс для воспроизведения архивного видео
	boost::shared_ptr<CHCS::IStreamManager> m_StreamManager;
public:
	//! \param streamManager интерфейс для воспроизведения архивного видео
	ArchiveRender( boost::shared_ptr<CHCS::IStreamManager> streamManager );
	virtual ~ArchiveRender();

	virtual void	AddCameras( const std::vector<int>& CameraIndexArr );
	virtual void	RemoveCameras( const std::vector<int>& CameraIndexArr );
	virtual bool	DrawFrame( int CameraIndex, float x, float y, float width, float height
								/*std::vector<unsigned char>& UserFrameData */);

	virtual void	AcquireArchive( time_t nStartTime, time_t nEndTime, const std::vector<int>& CameraIndexArr ) ;
	virtual void	ReleaseArchive( );

	virtual void	Play();
	virtual void	Pause();
	virtual void	Stop();
	virtual void	Seek( time_t NewArchivePosition );
	virtual time_t	GetArchivePosition();

	virtual bool	GetFrameUserData( int CameraID, std::vector<unsigned char>& UserFrameData );

	boost::optional<time_t>	GetCameraArchivePosition(int CameraID);

	virtual void	MakeActive();

	void GetAvailableCameras(  std::vector<int>& camerasVec )
	{
	}

	void BindCallback( boost::function0<void> fn );
	void UnbindCallback();

	void	OnRenderUpdate();
	void	OnCameraUpdate();
};

#endif // _ARCHIVE_RENDER_6298332396054487_