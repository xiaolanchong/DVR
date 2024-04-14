//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: 
//	Реализация IStreamManager для нескольких режимов 
//	1)регистрация устройств
//	2)сервер
//	3)клиент

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   01.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _STREAM_MANAGER_2396098304354053_
#define _STREAM_MANAGER_2396098304354053_

class BaseManager;
class ClientManager;
class SystemSettings;

//======================================================================================//
//                               class StreamManagerImpl                                //
//======================================================================================//

//! \brief 
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   01.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class StreamManagerImpl : public CHCS::IStreamManager
{
	virtual bool __stdcall GetAvailableStreamIDs(long* pStreamIDs, size_t* StreamNumber);
	virtual bool __stdcall GetStreamByID(CHCS::IStream** ppStream, long lStream) ;

	virtual void  __stdcall	Release() ;
private:
	BaseManager* _Manager;

	BaseManager* GetManager()
	{
		return _Manager;
	}
public:
	StreamManagerImpl( BaseManager* pManager );
	virtual ~StreamManagerImpl() ;
};

class StreamManagerServerImpl : public StreamManagerImpl
{
public:
	StreamManagerServerImpl( BaseManager* pManager );

private:
	virtual bool  __stdcall StartArchiveStream(CHCS::IStream** ppStream, long lStream, INT64 startPos) ;
};

class StreamManagerClientImpl : public StreamManagerImpl
{
	ClientManager* m_pManager;
public:
	StreamManagerClientImpl( ClientManager* pManager );
	virtual bool  __stdcall StartArchiveStream(CHCS::IStream** ppStream, long lStream, INT64 startPos) ;
};

class StreamManagerRegisterImpl : public CHCS::IStreamManager
{
	virtual bool __stdcall GetAvailableStreamIDs(long* pStreamIDs, size_t* StreamNumber)
	{
		return false;
	}
	virtual bool __stdcall GetStreamByID(CHCS::IStream** ppStream, long lStream) 
	{
		return false;
	}
	virtual bool  __stdcall StartArchiveStream(CHCS::IStream** ppStream, long lStream, INT64 startPos) 
	{
		return false;
	}
	virtual void  __stdcall	Release()
	{
		delete this;
	}
public:
	StreamManagerRegisterImpl( boost::shared_ptr<SystemSettings> ss);
};

#endif // _STREAM_MANAGER_2396098304354053_