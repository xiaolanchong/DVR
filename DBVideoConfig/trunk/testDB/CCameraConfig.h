#ifndef _CCAMERACONFIG_H_
#define _CCAMERACONFIG_H_

#include "CTypeAccessor.h"

class CCameraConfig: public DBBridge::ICameraConfig
{
public:
	CCameraConfig(CSession& Session, INT nDeviceID, INT nPinNumber);
	virtual ~CCameraConfig();
	
	//! получить идентфикатор камеры в БД
	//! \return идентфикатор БД
	virtual int GetCameraID();

	//! получить пин камеры
	//! \return номер пина
	virtual int GetCameraPin();

	//! получить дескриптор камеры
	//! \return дескриптори камеры 
	virtual std::tstring GetDescription();

	//! установить дескриптор камеры
	//! \param sDesc записуемое значение
	virtual void SetDescription(const std::tstring& sDesc);

protected:
	//ID камеры
	DWORD m_nDeviceID;
	//Pin камеры
	DWORD m_nPinNumber;
	//Сеанс
	CSession m_session;
};
#endif