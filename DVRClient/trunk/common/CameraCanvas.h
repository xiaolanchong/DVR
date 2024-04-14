//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Not available

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   22.11.2005
*/                                                                                      //
//======================================================================================//
#ifndef _CAMERA_CANVAS_7040511296756271_
#define _CAMERA_CANVAS_7040511296756271_

#include "layout/LayoutCameraManager.h"
#include "RenderEventHandler.h"

class IDVRRender;
class OGLWrapper2D;
class IMenuHandler;
class ICameraName;

template<class T> class DrawRectangle 
{
	T* m_pRender;
public:
	bool operator() ( int nCameraID, const IAreaLayout::Rect& rc ) const
	{
		T::ICamera* p = m_pRender->GetCamera(nCameraID);
		if (!p)
		{
			return false;
		}
		p->Draw( rc.x, rc.y, rc.w, rc.h );
		//m_pRender->DrawFrame( nCameraID, rc.x, rc.y, rc.w, rc.h  );
		return true;
	}

	DrawRectangle(T* pRender) : m_pRender(pRender){}
};

class	RectStorage
{
	boost::shared_ptr<IArchiveUserData>	m_pDataExtractor;
	Frames_t	m_Frames;
public:

	RectStorage() :
	  m_pDataExtractor( CreateArchiveUserDataInterface() )
	  {
	  }
	  void	AddFramesFromOneCamera( int nCameraID, const std::vector<unsigned char>& UserData ) 
	  {
		  std::vector< DBBridge::Frame::Rect> RectsFromOneCamera;
		  boost::int64_t FrameTime;
		  m_pDataExtractor->Unserialize( UserData, RectsFromOneCamera, FrameTime );

		  time_t FrameTimeFromTimeStamp = FrameTime;
		  for ( size_t i =0; i < RectsFromOneCamera.size(); ++i )
		  {
			  m_Frames.push_back( DBBridge::Frame( nCameraID, FrameTimeFromTimeStamp, FrameTimeFromTimeStamp, RectsFromOneCamera[i] ) );
		  }
	  }

	  const Frames_t& GetAlarmFrames() 
	  {
		  return m_Frames;
	  }

	  void ClearFrames()
	  {
		  m_Frames.clear();
	  }
};

//======================================================================================//
//                                  class CameraCanvas                                  //
//======================================================================================//

//! \brief общая работа с областью отображения кадров и ситуаций
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   22.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class CameraCanvas : public RenderEventCallback
{
	size_t					m_nWindowID;

	IMenuHandler*			m_pMenuHandler;
	ICameraName*			m_pCameraName;
protected:
	IDVRRenderManager*		m_pMainRender;
protected:
	LayoutCameraManager&	m_LayoutMgr;
public:
	CameraCanvas( LayoutCameraManager& LayoutMgr, IDVRRenderManager* pMainRender );
	virtual ~CameraCanvas();

	void	SetMenuHandler( IMenuHandler* pMenuHandler)
	{
		m_pMenuHandler = pMenuHandler;
	}
protected:	
	wxWindow*	GetWindow();
	//! возвращает от производного класса рендер для отрисовки
	//! \return рендер
	IDVRGraphicsWrapper* GetWrapper2D();
protected:

	//! получить размер клиентского окна
	//! \return размер клиентского окна в пикселях
	wxSize GetClientSize()
	{
		wxWindow* pWnd = GetWindow();
		_ASSERTE(pWnd);
		return pWnd->GetClientSize();
	}

	//! очистить окно цветом системного заднего фона
	//! \param pRender интерфейс закраски
	static void	ClearWindow(IDVRGraphicsWrapper* pRender);

	//! нарисовать изображения с камер или архива, как вариант может быть шаблонной
	//! \param sizeWindow	размеры окна, в кот. отображается
	//! \param pRender		интерфейс отображения
	//! \param fnFrameRender callback для отображения кадра (покольку интерфейс не известен) 
	void	DrawFrames( 
						wxSize sizeWindow, 
						IDVRGraphicsWrapper* pRender, 
						boost::function2<bool, int, IAreaLayout::Rect> fnFrameRender );

	//! нарисовать области тревожных ситуаций
	//! \param sizeWindow	размер окна отображения
	//! \param pRender		интерфейс отображения
	//! \param CameraAlarmRects массив инф-и о ситуациях
	void	DrawAlarmRects( 
						wxSize sizeWindow, 
						IDVRGraphicsWrapper* pRender, 
						const Frames_t& CameraAlarmRects );


	//! 
	//! \param pRender интерфейс для отображения
	//! \param sizeWindow 
	//! \param x0 X-координата левого верхнего угла
	//! \param y0 Y-координата левого верхнего угла
	//! \param x1 X-координата правого нижнего угла
	//! \param y1 Y-координата правого нижнего угла
	static void	DrawEmptyFrame(IDVRGraphicsWrapper* pRender, wxSize sizeWindow, float x0, float y0, float x1, float y1);

	//! нарисовать при ошибки прорисовки кадра
	//! \param pRender интерфейс для отображения
	//! \param x0 X-координата левого верхнего угла
	//! \param y0 Y-координата левого верхнего угла
	//! \param x1 X-координата правого нижнего угла
	//! \param y1 Y-координата правого нижнего угла
	static void	DrawFaultFrame(IDVRGraphicsWrapper* pRender, float x0, float y0, float x1, float y1);

	//! for descendant classes, event handler
	virtual void OnLayoutChange()
	{
	}
private:

	std::string GetCameraName(int CameraID);

	//! обработчик нажатия правой клавиши мыши
	virtual void OnRButtonHandler( wxMouseEvent& ev );
	//! обработчик команды меню
	virtual void OnMenuHandler( wxCommandEvent& ev );
	//! обработчик изменения размера окна
	virtual void OnSizeHandler( wxSizeEvent& ev );
};

#endif // _CAMERA_CANVAS_7040511296756271_