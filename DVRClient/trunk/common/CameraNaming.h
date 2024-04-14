//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Получить/установить имя камеры

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   12.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _CAMERA_NAMING_2851970675210179_
#define _CAMERA_NAMING_2851970675210179_

#include "IMenuHandler.h"
#include "ICameraName.h"

//======================================================================================//
//                                  class CameraNaming                                  //
//======================================================================================//

//! \brief Получить/установить имя камеры
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   12.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class CameraNaming : public ICameraName , 
					 public IMenuHandler
{
	int		m_nMenuItemID;
public:
	CameraNaming();
	virtual ~CameraNaming();

	virtual void	AddItemsToMenu( wxMenu* pMenu, int MinMenuItemID ) ;
	virtual void	ProcessMenuItem( int SelectedItemID )  ;

	virtual std::string		GetCameraName( int CameraID ) ;
	virtual bool			SetCameraName( int CameraID, const std::string& sNewCameraName) ; 
};

#endif // _CAMERA_NAMING_2851970675210179_