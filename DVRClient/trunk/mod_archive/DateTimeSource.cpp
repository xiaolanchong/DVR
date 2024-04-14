//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Получить дата-время от пользователя

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   16.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "DateTimeSource.h"
//HACK : windows version only
#include <commctrl.h>

//======================================================================================//
//                                 class DateTimeSource                                 //
//======================================================================================//

void ModifyStyle( HWND hWnd, DWORD RemoveStyle, DWORD AddStyle)
{
	DWORD dwStyle = ::GetWindowLongPtr(hWnd, GWL_STYLE);
	DWORD dwNewStyle = (dwStyle & ~RemoveStyle) | AddStyle;
	if (dwStyle == dwNewStyle)
	{
		return ;
	}
	else
	{
		::SetWindowLong(hWnd, GWL_STYLE, dwNewStyle);
	}
}

DateTimeSource::DateTimeSource( wxWindow* pParent, const std::string& sDateCtrlName, const std::string& sTimeCtrlName )
{
	bool res;
	m_pDateCtrl = new wxDatePickerCtrl( pParent, wxID_ANY, wxDateTime(), wxDefaultPosition, wxSize(1, 1), wxDP_DROPDOWN  );
	res = wxXmlResource::Get()->AttachUnknownControl( sDateCtrlName.c_str(), m_pDateCtrl, pParent );
	_ASSERTE(res);

//	HWND hDateWnd = (HWND)m_pDateCtrl->GetHandle();
//	ModifyStyle( hDateWnd, DTS_UPDOWN , DTS_DROPDOWN ); 

	m_pTimeCtrl = new wxDatePickerCtrl( pParent, wxID_ANY, wxDateTime(), wxDefaultPosition, wxSize(1, 1) );
	res = wxXmlResource::Get()->AttachUnknownControl( sTimeCtrlName.c_str(), m_pTimeCtrl, pParent );
	_ASSERTE(res);

	HWND hWnd = (HWND)m_pTimeCtrl->GetHandle();

	
	DateTime_SetFormat ( hWnd,_T("HH:mm") );
}

DateTimeSource::~DateTimeSource()
{
}

tm		DateTimeSource::GetCurrentDateTime()
{
	wxDateTime OnlyDate = m_pDateCtrl->GetValue();

	SYSTEMTIME stTime;
	HWND hWnd = (HWND)m_pTimeCtrl->GetHandle();
	DateTime_GetSystemtime( hWnd, &stTime );

	tm RealTime = { 0 };
	RealTime.tm_year	= OnlyDate.GetYear()	- 1900;
	RealTime.tm_mon		= OnlyDate.GetMonth();
	RealTime.tm_mday	= OnlyDate.GetDay();

	RealTime.tm_hour	= stTime.wHour;
	RealTime.tm_min		= stTime.wMinute;
	RealTime.tm_sec		= stTime.wSecond;
	return RealTime;
}

void	DateTimeSource::SetCurrentDateTime( const tm& NewTime)
{
	SYSTEMTIME stTime = { 0 };
	stTime.wYear	= NewTime.tm_year + 1900;
	stTime.wMonth	= NewTime.tm_mon + 1;
	stTime.wDay		= NewTime.tm_mday;
	stTime.wHour	= NewTime.tm_hour;
	stTime.wMinute	= NewTime.tm_min;
	stTime.wSecond	= NewTime.tm_sec;

	HWND hWnd = (HWND)m_pTimeCtrl->GetHandle();
	bool res = DateTime_SetSystemtime( hWnd, GDT_VALID, &stTime );
	_ASSERTE(res);

	m_pDateCtrl->SetValue( wxDateTime( NewTime ) );
}