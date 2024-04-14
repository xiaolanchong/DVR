//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейсы обмена данных между архивным сервером и клиентом

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   17.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_DATA_EXCHANGE_5078342910337534_
#define _ARCHIVE_DATA_EXCHANGE_5078342910337534_

//! интерфейс передачи команд управления архива
class IArchiveDataRequest
{
public:

	virtual bool	SendCommand( LPCSTR szCommand ) = 0;
	virtual void	ShutdownRequests() = 0;
	virtual ~IArchiveDataRequest() {}
};

class IArchiveDataResponse
{
public:
	virtual bool	SendStreamFormat( DWORD dwFCC, BITMAPINFO *pbiInput ) = 0;
	virtual bool	SendFrameData(
								const void* pImageData, size_t ImageDataSize, 
								const void* pUserData,  size_t UserDataSize, 
								unsigned uFrameSeq, unsigned uTime 
								) = 0;

	virtual bool	SendEndOfFormat()
	{
		return true;
	}

	virtual long	GetSessionID()
	{
		return 0;
	}

	virtual  void	Shutdown() = 0;
	virtual  bool IsConnected(int ) = 0;

	virtual ~IArchiveDataResponse() {}
};

#endif // _ARCHIVE_DATA_EXCHANGE_5078342910337534_