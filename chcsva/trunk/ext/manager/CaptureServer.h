//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Сервер потоков захвата видео с плат захвата

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _CAPTURE_SERVER_8455656473531140_
#define _CAPTURE_SERVER_8455656473531140_

#include "../../StreamDevice.h"

struct DeviceSettings;

//======================================================================================//
//                                 class CaptureServer                                  //
//======================================================================================//

//! \brief Сервер потоков захвата видео с плат захвата
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   29.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class CaptureServer
{
	std::vector< boost::shared_ptr<CDeviceStream> >		m_CaptureDevices;
	std::vector< boost::shared_ptr<CDevicePin> >		m_CaptureFilters;

	HWND CreateDeviceEventWnd();

	static LRESULT CALLBACK DeviceEventWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	CaptureServer();

	bool	CreateDevices( const std::vector<DeviceSettings>& CamParams);
	bool	IsDeviceAlreadyCreated( LPCTSTR stMoniker ) const;

	void	GetAllDevicesInSystem( std::vector< std::pair<std::tstring, std::tstring> >& Devices ) const;

	void	SetUsedPins ( const std::vector< std::pair<UUID, long> >& RequiredPins  );

	void	GetUsedPins ( std::vector< std::pair<UUID, long> >& RequiredPins  ) const;
	IOutputFramePin*	FindOuputPin( const UUID& StreamUID, long DevicePin ) const;

	void DeleteAllDevices();
	void DeleteAllPins();

	virtual ~CaptureServer();
};

#endif // _CAPTURE_SERVER_8455656473531140_