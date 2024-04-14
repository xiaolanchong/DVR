//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������� ��� �� - �������� ��������������� � OLE DB ����� IVideoConfig

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _DATABASE_WRAPPER_3869901929545680_
#define _DATABASE_WRAPPER_3869901929545680_

//======================================================================================//
//                                  DB wrappers                                         //
//======================================================================================//

//! �������� ��� CDataSource
struct CWrapDataSource
{
	HRESULT OpenFromInitializationString( LPCWSTR )
	{
		return S_OK;
	}
	HRESULT Open()
	{
		return E_FAIL;
	}
	HRESULT GetInitializationString( BSTR* ps)
	{
		*ps = 0;
		return E_FAIL;
	}
};

//! �������� ��� CSession
struct CWrapSession
{
	HRESULT Open( CWrapDataSource& )
	{
		return S_OK;
	}
};

//////////////////////////////////////////////////////////////////////////

typedef	 boost::tuple<LONG, std::tstring, LONG, std::tstring>	CameraInfo_t;

void	GetAllCameraInfo(DBBridge::IVideoConfig* pVideoConfig, std::vector<CameraInfo_t>& Cameras);

//! �������� ��� CCommand
class CWrapAllCameraCommand
{
	std::vector<CameraInfo_t>	m_Cameras;
	size_t						m_idxCamera;

	void	Update()
	{
		_ASSERTE( m_idxCamera < m_Cameras.size() );
		m_lCameraID			= boost::get<0>( m_Cameras[m_idxCamera] ) ;
		std::tstring sUID	= boost::get<1>( m_Cameras[m_idxCamera] ) ;
		m_lDevicePin		= boost::get<2>( m_Cameras[m_idxCamera] ) ;
		std::tstring sIP	= boost::get<3>( m_Cameras[m_idxCamera] ) ;
		
		lstrcpyn (m_stCameraUID, sUID.c_str(), 256 );
		lstrcpyn (m_stCameraIP, sIP.c_str(), 256 );
	}
public:
	HRESULT Open( CWrapSession& )
		try
	{
		std::vector<std::tstring>	AllIPAddresses;
		DBBridge::GetAllIPAddresses(AllIPAddresses);

		for ( size_t i = 0; i < AllIPAddresses.size(); ++i )
		{
			std::vector<CameraInfo_t>	CamerasFromOneComputers;
			std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig( DBBridge::CreateVideoConfig( AllIPAddresses[i].c_str() ) );

			GetAllCameraInfo( pVideoConfig.get(), CamerasFromOneComputers );
			m_Cameras.insert( m_Cameras.end(), CamerasFromOneComputers.begin(), CamerasFromOneComputers.end() );
		}

		m_idxCamera = 0;
		return S_OK;
	}
	catch ( std::exception ) 
	{
		return E_FAIL;
	};

	HRESULT MoveFirst()
		try
	{
		m_idxCamera = 0;
		if( m_Cameras.size() == 0 ) return DB_S_ENDOFROWSET;
		Update();
		return S_OK ;
	}
	catch ( std::exception ) 
	{
		return DB_S_ENDOFROWSET;
	}

	HRESULT MoveNext()
		try
	{
		++m_idxCamera;
		if( m_idxCamera < m_Cameras.size())
		{
			Update();
			return S_OK;
		}
		else
		{
			return DB_S_ENDOFROWSET;
		}
	}
	catch ( std::exception ) 
	{
		return DB_S_ENDOFROWSET;
	}

	//! ������������ ������
	LONG	m_lCameraID;
	//! ��������� UID ����������
	//! \warning StringToUuid hack!
	TCHAR	m_stCameraUID[ 256 ];
	//! ��� ����������
	LONG	m_lDevicePin;
	//! ����� �������
	TCHAR	m_stCameraIP[256];
};

class CWrapCameraInRoomCommand
{
	//! �������� ��� ������ ������ �� �������������� � �������
	class FindTuple : public std::unary_function<bool, const CameraInfo_t&> 
	{
		int	m_nSearchedCameraID;
	public:
		FindTuple ( int nSearchedCameraID ):
		  m_nSearchedCameraID( nSearchedCameraID )
		  {
		  }
		bool	operator( ) (const CameraInfo_t& CamInfo) const
		{
			return boost::get<0>( CamInfo ) == m_nSearchedCameraID;
		}
	};
public:
	HRESULT Open( CWrapSession& )
		try
	{
		std::auto_ptr<DBBridge::IVideoConfig> pConfig( DBBridge::CreateVideoConfig() );

		if ( !pConfig.get() )
		{
			return E_FAIL;
		}

		std::vector<CameraInfo_t>	Cameras;
		std::vector<std::tstring>	AllIPAddresses;
		DBBridge::GetAllIPAddresses(AllIPAddresses);

		for ( size_t i = 0; i < AllIPAddresses.size(); ++i )
		{
			std::vector<CameraInfo_t>	CamerasFromOneComputers;
			std::auto_ptr<DBBridge::IVideoConfig> pVideoConfig( DBBridge::CreateVideoConfig( AllIPAddresses[i].c_str() ) );

			GetAllCameraInfo( pVideoConfig.get(), CamerasFromOneComputers );
			Cameras.insert( Cameras.end(), CamerasFromOneComputers.begin(), CamerasFromOneComputers.end() );
		}

		std::vector<CameraInfo_t>::iterator it = std::find_if( Cameras.begin(), Cameras.end(), FindTuple( m_lCameraID ) );

		if( it != Cameras.end() )
		{
			std::tstring sUID	= boost::get<1>(*it);
			std::tstring sIP	= boost::get<3>(*it);
			
			lstrcpyn (m_stCameraUID, sUID.c_str(), 256 );
			lstrcpyn (m_stCameraIP, sIP.c_str(), 256 );

			return S_OK;
		}
		else
		{
			// not found
			return E_FAIL;
		}
	}
	catch ( std::exception ) 
	{
		return E_FAIL;
	};

	HRESULT MoveFirst()
	{	
		return S_OK;
	}

	HRESULT MoveNext()
	{
		return DB_S_ENDOFROWSET;
	}

	LONG	m_lCameraID;
	TCHAR	m_stCameraUID[256];	// "{UUID}"
	TCHAR	m_stCameraIP[256];		// xxx.xxx.xxx.xxx - dotted format
};


#endif // _DATABASE_WRAPPER_3869901929545680_