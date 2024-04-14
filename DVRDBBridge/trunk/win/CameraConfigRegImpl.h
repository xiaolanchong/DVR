//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация интерфейса на реестре

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _CAMERA_CONFIG_REG_IMPL_3790791795283277_
#define _CAMERA_CONFIG_REG_IMPL_3790791795283277_

#include "../interface/IVideoConfig.h"

//======================================================================================//
//                              class CameraConfigRegImpl                               //
//======================================================================================//

//! \brief Реализация интерфейса на реестре
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   30.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class CameraConfigRegImpl : public DBBridge::ICameraConfig
{
	virtual int				GetCameraID()
	{
		return m_nID;
	}
	virtual int				GetCameraPin();

	virtual std::tstring	GetDescription() ;

	virtual void			SetDescription(const std::tstring& sDesc) ;
public:

	CameraConfigRegImpl(LPCTSTR szPath, int nID, int nPinNumber );
	CameraConfigRegImpl(LPCTSTR szPath, int nID);
	virtual ~CameraConfigRegImpl();

private:
	CRegKey	m_keyCamera;

	std::tstring	m_sPath;
	int				m_nID;
};

#endif // _CAMERA_CONFIG_REG_IMPL_3790791795283277_