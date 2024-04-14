//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ����������� ���� ��������� ������� � ����������� �� � �������
//	���������, ��� �������, ������� ���� ���������������� ���������� �������
//	(����� ����� ������������ ��� ����), ��� ����� �������� �� ServerManager/CManager.
//	�.�. ��������� ��������� ������ InitStreamManager

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   01.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _DEVICE_REGISTRAR_5447491258109570_
#define _DEVICE_REGISTRAR_5447491258109570_

class SystemSettings;

//======================================================================================//
//                                class DeviceRegistrar                                 //
//======================================================================================//

//! \brief ����������� ���� ��������� ������� � ����������� �� � �������
//	���������, ��� �������, ������� ���� ���������������� ���������� �������
//	(����� ����� ������������ ��� ����), ��� ����� �������� �� ServerManager/CManager.
//	�.�. ��������� ��������� ������ InitStreamManager
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   01.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class DeviceRegistrar
{
	bool RegisterDevice(SystemSettings& ss, const UUID& streamID, LPCTSTR stMoniker);
public:
	DeviceRegistrar();
	virtual ~DeviceRegistrar();

	void FindAndInstallDevices(SystemSettings& ss);
};

#endif // _DEVICE_REGISTRAR_5447491258109570_