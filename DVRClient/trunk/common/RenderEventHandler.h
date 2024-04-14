//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���������� ��������� �� ���� �������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   09.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _RENDER_EVENT_HANDLER_2624896666480381_
#define _RENDER_EVENT_HANDLER_2624896666480381_

class RenderEventCallback
{
public:
	//! ���������� ������� ������ ������� ����
	virtual void OnRButtonHandler( wxMouseEvent& ev ) = 0;
	//! ���������� ������� ����
	virtual void OnMenuHandler( wxCommandEvent& ev ) = 0;
	//! ���������� ��������� ������� ����
	virtual void OnSizeHandler( wxSizeEvent& ev ) = 0;
};

//======================================================================================//
//                               class RenderEventHandler                               //
//======================================================================================//

//! \brief ���������� ��������� �� ���� �������
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

	//! ���������� ������� ������ ������� ����
	void OnRButton( wxMouseEvent& ev );
	//! ���������� ������� ����
	void OnCameraMenu( wxCommandEvent& ev );
	//! ���������� ��������� ������� ����
	void OnSize( wxSizeEvent& ev );

	DECLARE_EVENT_TABLE();
};

#endif // _RENDER_EVENT_HANDLER_2624896666480381_