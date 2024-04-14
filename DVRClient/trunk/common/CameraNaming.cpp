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

#include "stdafx.h"
#include "CameraNaming.h"

//======================================================================================//
//                                  class CameraNaming                                  //
//======================================================================================//

CameraNaming::CameraNaming() : m_nMenuItemID(-1)
{
}

CameraNaming::~CameraNaming()
{
}

#undef CAMERA_NAME

void	CameraNaming::AddItemsToMenu( wxMenu* pMenu, int MinMenuItemID )
{
#ifdef CAMERA_NAME
	pMenu->AppendSeparator();
	pMenu->Append(  MinMenuItemID, _("Set camera name...") );
#endif
}

void	CameraNaming::ProcessMenuItem( int SelectedItemID )
{

}

std::string		CameraNaming::GetCameraName( int CameraID ) 
{
	return std::string("cam") + boost::lexical_cast<std::string>(CameraID);
}

bool			CameraNaming::SetCameraName( int CameraID, const std::string& sNewCameraName)
{
	return false;
}