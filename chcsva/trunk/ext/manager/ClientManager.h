//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Главный класс реализации интерфейса IStreamManager для клиента

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _CLIENT_MANAGER_8619867992196614_
#define _CLIENT_MANAGER_8619867992196614_

#include "BaseManager.h"

class RemoteVideoConsumer;

//======================================================================================//
//                                 class ClientManager                                  //
//======================================================================================//

//! \brief Главный класс реализации интерфейса IStreamManager для клиента
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   30.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ClientManager : public BaseManager
{
public:
	ClientManager(boost::shared_ptr<SystemSettings> pVideoConfig, DWORD WorkingMode );
	virtual ~ClientManager();

	CHCS::IStream* CreateArchiveStream( long lSourceID, INT64 startPos);
private:

	virtual void InitializeWork() ;
	virtual void UninitializeWork();

	/// settings

	bool	CreateNetServer() const
	{
		return CheckMask<CHCS_MODE_NETCLIENT>() != 0;
	}

	bool ReadStreamsFromDB(bool bShowDialogOnFailed);

	void CreateVideoConsumer();
	CHCS::IStream* CreateRemoteStream(long StreamID, LPCTSTR stCameraIP);

//	void	CreateArchiveConsumer();
//	void	DestroyArchiveConsumer();
private:

	std::auto_ptr<RemoteVideoConsumer>		m_VideoConsumer;
//	std::auto_ptr<RemoteArchiveConsumer>	m_ArchiveConsumer;
};

#endif // _CLIENT_MANAGER_8619867992196614_