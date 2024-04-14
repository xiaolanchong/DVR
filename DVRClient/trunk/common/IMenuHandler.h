//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Добавление/обработка команд в меню 

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   12.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_MENU_HANDLER_5855878233749755_
#define _I_MENU_HANDLER_5855878233749755_

//======================================================================================//
//                                  class IMenuHandler                                  //
//======================================================================================//

//! \brief Добавление/обработка команд в меню 
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   12.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class IMenuHandler
{
public:
	
	virtual ~IMenuHandler() {};

	//! добавить пункт в меню
	//! \param pMenu меню
	//! \param MinMenuItemID минимальный идентификатор, с которого надо начинать добавление 
	virtual void	AddItemsToMenu( wxMenu* pMenu, int MinMenuItemID ) = 0;
	//! обработать выбранный пункт
	//! \param SelectedItemID идент-р из AddItemsToMenu
	virtual void	ProcessMenuItem( int SelectedItemID )  = 0;
};

#endif // _I_MENU_HANDLER_5855878233749755_