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
#ifndef _DATE_TIME_SOURCE_3623855380955521_
#define _DATE_TIME_SOURCE_3623855380955521_

//======================================================================================//
//                                 class DateTimeSource                                 //
//======================================================================================//

//! \brief Получить дата-время от пользователя
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   16.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class DateTimeSource
{
	wxDatePickerCtrl	*m_pDateCtrl;
	wxDatePickerCtrl	*m_pTimeCtrl;
public:
	DateTimeSource( wxWindow* pParent, const std::string& sDateCtrlName, const std::string& sTimeCtrlName );

	tm		GetCurrentDateTime();
	void	SetCurrentDateTime( const tm& NewTime); 

	virtual ~DateTimeSource();
};

#endif // _DATE_TIME_SOURCE_3623855380955521_