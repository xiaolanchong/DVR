//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
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

//! \brief ����� ������ � �������� ����������� ������ � ��������
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
	//! ���������� �� ������������ ������ ������ ��� ���������
	//! \return ������
	IDVRGraphicsWrapper* GetWrapper2D();
protected:

	//! �������� ������ ����������� ����
	//! \return ������ ����������� ���� � ��������
	wxSize GetClientSize()
	{
		wxWindow* pWnd = GetWindow();
		_ASSERTE(pWnd);
		return pWnd->GetClientSize();
	}

	//! �������� ���� ������ ���������� ������� ����
	//! \param pRender ��������� ��������
	static void	ClearWindow(IDVRGraphicsWrapper* pRender);

	//! ���������� ����������� � ����� ��� ������, ��� ������� ����� ���� ���������
	//! \param sizeWindow	������� ����, � ���. ������������
	//! \param pRender		��������� �����������
	//! \param fnFrameRender callback ��� ����������� ����� (�������� ��������� �� ��������) 
	void	DrawFrames( 
						wxSize sizeWindow, 
						IDVRGraphicsWrapper* pRender, 
						boost::function2<bool, int, IAreaLayout::Rect> fnFrameRender );

	//! ���������� ������� ��������� ��������
	//! \param sizeWindow	������ ���� �����������
	//! \param pRender		��������� �����������
	//! \param CameraAlarmRects ������ ���-� � ���������
	void	DrawAlarmRects( 
						wxSize sizeWindow, 
						IDVRGraphicsWrapper* pRender, 
						const Frames_t& CameraAlarmRects );


	//! 
	//! \param pRender ��������� ��� �����������
	//! \param sizeWindow 
	//! \param x0 X-���������� ������ �������� ����
	//! \param y0 Y-���������� ������ �������� ����
	//! \param x1 X-���������� ������� ������� ����
	//! \param y1 Y-���������� ������� ������� ����
	static void	DrawEmptyFrame(IDVRGraphicsWrapper* pRender, wxSize sizeWindow, float x0, float y0, float x1, float y1);

	//! ���������� ��� ������ ���������� �����
	//! \param pRender ��������� ��� �����������
	//! \param x0 X-���������� ������ �������� ����
	//! \param y0 Y-���������� ������ �������� ����
	//! \param x1 X-���������� ������� ������� ����
	//! \param y1 Y-���������� ������� ������� ����
	static void	DrawFaultFrame(IDVRGraphicsWrapper* pRender, float x0, float y0, float x1, float y1);

	//! for descendant classes, event handler
	virtual void OnLayoutChange()
	{
	}
private:

	std::string GetCameraName(int CameraID);

	//! ���������� ������� ������ ������� ����
	virtual void OnRButtonHandler( wxMouseEvent& ev );
	//! ���������� ������� ����
	virtual void OnMenuHandler( wxCommandEvent& ev );
	//! ���������� ��������� ������� ����
	virtual void OnSizeHandler( wxSizeEvent& ev );
};

#endif // _CAMERA_CANVAS_7040511296756271_