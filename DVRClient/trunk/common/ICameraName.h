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
#ifndef _I_CAMERA_NAME_3109214981092688_
#define _I_CAMERA_NAME_3109214981092688_

//======================================================================================//
//                                  class ICameraName                                   //
//======================================================================================//

//! \brief Получить/установить имя камеры
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   12.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class ICameraName
{
public:

	virtual ~ICameraName() {};

	virtual std::string		GetCameraName( int CameraID ) = 0;
	virtual bool			SetCameraName( int CameraID, const std::string& sNewCameraName) = 0; 
};

#endif // _I_CAMERA_NAME_3109214981092688_