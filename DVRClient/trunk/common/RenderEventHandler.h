//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Обработчик сообщений от окна рендере

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   09.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _RENDER_EVENT_HANDLER_2624896666480381_
#define _RENDER_EVENT_HANDLER_2624896666480381_

class RenderEventCallback
{
public:
	//! обработчик нажатия правой клавиши мыши
	virtual void OnRButtonHandler( wxMouseEvent& ev ) = 0;
	//! обработчик команды меню
	virtual void OnMenuHandler( wxCommandEvent& ev ) = 0;
	//! обработчик изменения размера окна
	virtual void OnSizeHandler( wxSizeEvent& ev ) = 0;
};

//======================================================================================//
//                               class RenderEventHandler                               //
//======================================================================================//

//! \brief Обработчик сообщений от окна рендере
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   09.03.2006
//! \version 1.0
//! \bug 
//! \todo 

class RenderEventHandler : wxEvtHandler 
{
	wxWindow*							m_pRenderWnd;
	boost::optional<size_t>				m_ActiveHandler;
	std::vector<RenderEventCallback*>	m_Handlers;
public:
	RenderEventHandler( wxWindow* pRenderWnd);
	virtual ~RenderEventHandler();

	void	AddHander(RenderEventCallback* pHanlder);
	void	SetActive(size_t nActiveHandler);
//	virtual wxWindow*	GetWindow();
protected:

/*
	wxSize GetClientSize()
	{
		wxWindow* pWnd = GetWindow();
		_ASSERTE(pWnd);
		return pWnd->GetClientSize();
	}
*/
private:

	//! обработчик нажатия правой клавиши мыши
	void OnRButton( wxMouseEvent& ev );
	//! обработчик команды меню
	void OnCameraMenu( wxCommandEvent& ev );
	//! обработчик изменения размера окна
	void OnSize( wxSizeEvent& ev );

	DECLARE_EVENT_TABLE();
};

#endif // _RENDER_EVENT_HANDLER_2624896666480381_