//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ����������/��������� ������ � ���� 

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   12.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_MENU_HANDLER_5855878233749755_
#define _I_MENU_HANDLER_5855878233749755_

//======================================================================================//
//                                  class IMenuHandler                                  //
//======================================================================================//

//! \brief ����������/��������� ������ � ���� 
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   12.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class IMenuHandler
{
public:
	
	virtual ~IMenuHandler() {};

	//! �������� ����� � ����
	//! \param pMenu ����
	//! \param MinMenuItemID ����������� �������������, � �������� ���� �������� ���������� 
	virtual void	AddItemsToMenu( wxMenu* pMenu, int MinMenuItemID ) = 0;
	//! ���������� ��������� �����
	//! \param SelectedItemID �����-� �� AddItemsToMenu
	virtual void	ProcessMenuItem( int SelectedItemID )  = 0;
};

#endif // _I_MENU_HANDLER_5855878233749755_