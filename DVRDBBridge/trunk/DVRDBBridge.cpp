// DVRDBBridge.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "DVRDBBridge.h"

#include "common/EmptyInstaller.h"
#include "db/DBMode.h"

#include "win/VideoConfigRegImpl.h"
#include "win/RegClientImpl.h"
#include "win/RegServerImpl.h"

#ifdef USE_LIBODBC
#include "db/VideoConfigDBImpl.h"
#endif
#include "db/DBServerImpl.h"
#include "db/DBClientImpl.h"


#ifdef USE_LIBSQLITE
#include "mixed/MixServerImpl.h"
#include "mixed/MixClientImpl.h"
#include "mixed/SQLiteInstaller.h"
#endif

#include "chcsva/VideoSystemConfigBridgeImpl.h"

using DBBridge::IVideoConfig;
#include "../../DBVideoConfig/trunk/testDB/MainFunction.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(lpReserved);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

namespace
{

//! абстрактная фабрика создания интерфейсов
class IDBBridgeFactory
{
protected:
	void GetLocalIPAddreses(std::vector<std::tstring>& IPAddressArr)
	{
		WinSock _wsa;
		IPAddressArr = ::GetOwnIPAddress();
	}
public:
	virtual ~IDBBridgeFactory() = default;
	virtual DBBridge::IVideoConfig*						IntCreateVideoConfig(LPCTSTR szIPAddress)								= 0;
	virtual boost::shared_ptr<DBBridge::IDBServer>		IntCreateDBServer(const char* szConnectionString)	= 0;
	virtual boost::shared_ptr<DBBridge::IDBClient>		IntCreateDBClient(const char* szConnectionString)	= 0;
	virtual boost::shared_ptr<DBBridge::IDBInstaller>	IntCreateDBInstaller()	= 0;

	virtual void IntGetAllIPAddresses(std::vector<std::tstring>& IPAddressArr) = 0;
};

std::tstring GetLocalIP()
{
	std::vector<std::tstring >	AllIPArr, DBAndLocalIPArr;
	DBBridge::GetAllIPAddresses( AllIPArr );

	std::vector<std::tstring >	LocalIPArr = GetOwnIPAddress();

	if( LocalIPArr.empty() )
	{
		//	return std::auto_ptr<DBBridge::IVideoConfig> ( );
		throw  std::runtime_error("Local IP address list is empty");
	}
	else if( AllIPArr.empty() )
	{
		std::tstring IpAddr = LocalIPArr[0];
		return IpAddr;
	}
	else
	{
		std::sort( AllIPArr.begin(), AllIPArr.end() );
		std::sort( LocalIPArr.begin(), LocalIPArr.end() );
		std::set_intersection( 
			AllIPArr.begin(), AllIPArr.end(),
			LocalIPArr.begin(), LocalIPArr.end(),
			std::back_inserter( DBAndLocalIPArr )
			);
		if( DBAndLocalIPArr.empty() )
		{
			throw  std::runtime_error("No local IP are in the database");
		}

		std::tstring IpAddr =
			(  DBAndLocalIPArr.empty() ) ? LocalIPArr[0] : DBAndLocalIPArr[0];
		return IpAddr;
	}
}

//! ODBC интефейс взаимод-я с БД
class OdbcDBBridgeFactory : public IDBBridgeFactory
{
	virtual DBBridge::IVideoConfig*					IntCreateVideoConfig(LPCTSTR szIPAddress)		
	{
		std::tstring IPAddress;
		if( szIPAddress && _tcslen(szIPAddress) )
		{
			IPAddress = szIPAddress;
		}
		else
		{
			WinSock _wsa;
			std::tstring OwnIPList = GetLocalIP();
			IPAddress = OwnIPList;
		}
		DBMode db;


		return DBCreateVideoConfig( db.GetConnectionString(), IPAddress ) ;
	}
	virtual boost::shared_ptr<DBBridge::IDBServer>	IntCreateDBServer(const char* )
	{
		DBMode db;
		return boost::shared_ptr<DBBridge::IDBServer>(new DBServerImpl(db.GetConnectionString()));
	}

	virtual boost::shared_ptr<DBBridge::IDBClient>	IntCreateDBClient(const char* )
	{
		DBMode db;
		return boost::shared_ptr<DBBridge::IDBClient>(new DBClientImpl(db.GetConnectionString()));
	}

	virtual boost::shared_ptr<DBBridge::IDBInstaller>	IntCreateDBInstaller()
	{
		return boost::shared_ptr<DBBridge::IDBInstaller>( new EmptyInstaller );
	}

	virtual void IntGetAllIPAddresses(std::vector<std::tstring>& IPAddressArr)
	try
	{
		IPAddressArr.clear();
		DBMode db;
		GetAllIPAdress( db.GetConnectionString(), IPAddressArr);
	}
	catch( std::exception& )
	{
		IPAddressArr.clear();
	};
};

//! реализация на реестре, формат согласно SDD
class RegDBBridgeFactory : public IDBBridgeFactory
{
	virtual DBBridge::IVideoConfig*					IntCreateVideoConfig(LPCTSTR szIPAddress)		
	{
		UNREFERENCED_PARAMETER(szIPAddress);
		return new VideoConfigRegImpl;
	}
	virtual boost::shared_ptr<DBBridge::IDBServer>	IntCreateDBServer(const char* )
	{
		return boost::shared_ptr<DBBridge::IDBServer>(new RegServerImpl());
	}

	virtual boost::shared_ptr<DBBridge::IDBClient>	IntCreateDBClient(const char* )
	{
		return boost::shared_ptr<DBBridge::IDBClient>(new RegClientImpl());
	}

	virtual boost::shared_ptr<DBBridge::IDBInstaller>	IntCreateDBInstaller()
	{
		return boost::shared_ptr<DBBridge::IDBInstaller>( new EmptyInstaller );
	}

	virtual void IntGetAllIPAddresses(std::vector<std::tstring>& IPAddressArr)
	{
		GetLocalIPAddreses( IPAddressArr );
	}
};

#ifdef USE_LIBSQLITE
//! сме
class MixDBBridgeFactory : public IDBBridgeFactory
{
	virtual DBBridge::IVideoConfig*					IntCreateVideoConfig(LPCTSTR szIPAddress)		
	{
		UNREFERENCED_PARAMETER(szIPAddress);
		return new VideoConfigRegImpl;
	}
	virtual boost::shared_ptr<DBBridge::IDBServer>	IntCreateDBServer(const char* )
	{
		DBMode db;
		return boost::shared_ptr<DBBridge::IDBServer>(new MixServerImpl(db.GetConnectionString()));
	}

	virtual boost::shared_ptr<DBBridge::IDBClient>	IntCreateDBClient(const char* )
	{
		DBMode db;
		return boost::shared_ptr<DBBridge::IDBClient>(new MixClientImpl(db.GetConnectionString()));
	}

	virtual boost::shared_ptr<DBBridge::IDBInstaller>	IntCreateDBInstaller()	
	{
	//	DBMode db;
		return boost::shared_ptr<DBBridge::IDBInstaller>(new SQLiteInstaller());
	}

	virtual void IntGetAllIPAddresses(std::vector<std::tstring>& IPAddressArr)
	{
		GetLocalIPAddreses( IPAddressArr );
	}
};
#endif

std::auto_ptr<IDBBridgeFactory>	GetFactory()
{
	DBMode db;
	switch (db.GetConfigMode())
	{
	case DBMode::cm_database:
		return std::auto_ptr<IDBBridgeFactory>( new OdbcDBBridgeFactory );
#ifdef USE_LIBSQLITE
	case DBMode::cm_mixed:
		return std::auto_ptr<IDBBridgeFactory>( new MixDBBridgeFactory );
#endif
	default:
		return std::auto_ptr<IDBBridgeFactory>( new RegDBBridgeFactory );
	}
}

}

namespace DBBridge
{

void		GetAllIPAddresses( std::vector<std::tstring>& IPAddressArr )
{
	std::auto_ptr<IDBBridgeFactory> pFactory = GetFactory();
	return pFactory->IntGetAllIPAddresses(IPAddressArr);
}

IVideoConfig* CreateVideoConfig(LPCTSTR szIPAddress)
{
	std::auto_ptr<IDBBridgeFactory> pFactory = GetFactory();
	return pFactory->IntCreateVideoConfig(szIPAddress);
}

boost::shared_ptr<IDBServer>	CreateDBServer(const char* szConnectionString)
{
	std::auto_ptr<IDBBridgeFactory> pFactory = GetFactory();
	return pFactory->IntCreateDBServer(szConnectionString);
}


boost::shared_ptr<IDBClient>	CreateDBClient(const char* szConnectionString)
{
	std::auto_ptr<IDBBridgeFactory> pFactory = GetFactory();
	return pFactory->IntCreateDBClient(szConnectionString);
}

boost::shared_ptr<IDBInstaller>	CreateDBInstaller()
{
	std::auto_ptr<IDBBridgeFactory> pFactory = GetFactory();
	return pFactory->IntCreateDBInstaller();
}

//////////////////////////////////////////////////////////////////////////

CHCS::IVideoSystemConfigEx*	WINAPI CreateVideoSystemConfig(LPCWSTR ServerIPAddress)
{
	// from installhelper, so alwayse add to the db
	if( ServerIPAddress && lstrlenW( ServerIPAddress ) > 0 )
	{
		return new VideoSystemConfigBridgeImpl(ServerIPAddress);
	}
	else
	{
		return new VideoSystemConfigBridgeImpl( true ); 
	}
}

CHCS::IVideoSystemConfigEx*	WINAPI CreateClientVideoSystemConfig( )
{
	// for DVRClient
	return new VideoSystemConfigBridgeImpl( false );
}

CHCS::IVideoSystemConfigEx*	WINAPI CreateServerVideoSystemConfig( )
{
	// for DVREngine
	return new VideoSystemConfigBridgeImpl( true );
}

}