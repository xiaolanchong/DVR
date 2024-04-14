//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Рендер изображений и информации в режиме архива

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   10.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_CANVAS_2097692965820325_
#define _ARCHIVE_CANVAS_2097692965820325_

#include "../Common/CameraCanvas.h"
//#include "AlarmArchiveData.h"

//======================================================================================//
//                                 class ArchiveCanvas                                  //
//======================================================================================//

//! \brief Рендер изображений и информации в режиме архива
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   10.03.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveCanvas : public CameraCanvas
{
	//! отрисовка экрана в режиме архива
	void	Draw( );

	IDVRArchiveRender*	m_pArchiveRender;

	//! архив инициализирован
	bool	m_bArchiveStarted;

	//! функция для сообщения времени архива, \see BindTimeCallback
	boost::function1<void, time_t> m_fnTimeCallback;
public:
	ArchiveCanvas(
		LayoutCameraManager& LayoutMgr, 
		IDVRRenderManager*	pMainRender, 
		boost::function1<void, time_t> fnTimeCallback 
		);
	virtual ~ArchiveCanvas();

	//! начать работу с архивом
	//! \param pStartTime	время начала
	//! \param pEndTime		время конца
	//! \param AlarmData	инф-я о рамках
	void	StartArchive( time_t StartTime, time_t EndTime);
	//! закончить работу с архивом
	void	ReleaseArchive();

	//! нажата кнопка проигрывания
	void	PlayArchive();
	//! нажата пауза
	void	PauseArchive();
	//! остановить архив
	void	StopArchive();

	//! получить текущее время архива
	//! \return текущее время архива
	time_t	GetArchiveTime();

	//! передвинуть архив на время
	//! \param NewTime новое положение архива
	void	SeekArchive(time_t NewTime);

	//! установить ф-ю обратного вызова для сообщения нового времени архива
	//! \param fn ф-я
	void	BindTimeCallback( boost::function1<void, time_t> fn);
private:

	//! добавили/удалили камеру на экран
	virtual void OnLayoutChange();
};

#endif // _ARCHIVE_CANVAS_2097692965820325_