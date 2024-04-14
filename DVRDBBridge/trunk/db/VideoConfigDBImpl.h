//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ODBC Database implementation

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   06.12.2005
*/                                                                                      //
//======================================================================================//
#ifndef _VIDEO_CONFIG_D_B_IMPL_8364759408328925_
#define _VIDEO_CONFIG_D_B_IMPL_8364759408328925_

#include "../interface/IVideoConfig.h"
#include "ComputerTable.h"
//#include "../win/OwnIPAddress.h"

//======================================================================================//
//                               class VideoConfigDBImpl                                //
//======================================================================================//

//! \brief ODBC Database implementation
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.12.2005
//! \version 1.0
//! \bug 
//! \todo 

class VideoConfigDBImpl :	public	DBBridge::IVideoConfig,
							private	ComputerTable
{
	friend class CameraConfigDBImpl;
	boost::shared_ptr<odbc::Connection>	m_con;

	WinSock wsa;
public:
	VideoConfigDBImpl();
	VideoConfigDBImpl(boost::shared_ptr<odbc::Connection>	con);
	virtual ~VideoConfigDBImpl();

private:
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, DWORD& dwValue) ;
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, std::tstring& sValue ) ;
	virtual void	GetValue( LPCTSTR szPath, LPCTSTR szName, std::vector<BYTE>& binValue );

	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, DWORD dwValue) ;
	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, LPCTSTR sValue ) ;
	virtual void	SetValue( LPCTSTR szPath, LPCTSTR szName, const std::vector<BYTE>& binValue );

	virtual void						GetCameras( std::vector<int>& CamIDArr ) ;
	virtual DBBridge::ICameraConfig*	GetCamera( int nID  );
	virtual DBBridge::ICameraConfig*	CreateCamera( LPCTSTR szUID );
	virtual void						DeleteCamera( int nID );

};

class CameraConfigDBImpl : public DBBridge::ICameraConfig
{
	virtual void	GetValue( LPCTSTR szName, DWORD& dwValue);
	virtual void	GetValue( LPCTSTR szName, std::tstring& sValue );

	virtual void	SetValue( LPCTSTR szName, DWORD dwValue);
	virtual void	SetValue( LPCTSTR szName, LPCTSTR sValue );	

	virtual std::tstring	GetCameraUID();

	virtual int				GetCameraPin()
	{
		// TODO: implement Cameras, Devices as separete tables
		return 0;
	}
	virtual int				GetCameraID()	{	return m_nID;	}
	virtual std::tstring	GetIPAddress()
	{
		return VideoConfigDBImpl::GetOwnIPAddress();
	}

	VideoConfigDBImpl:: QueryRes_t GetValue(LPCTSTR szName, bool bUpdate)
	{
		std::tstring sIP = GetIPAddress();
		std::string sQuery = (boost::format("SELECT %s FROM Cameras WHERE CameraID=%d") % szName % m_nID).str();

		boost::shared_ptr<odbc::Statement> st ( 
			m_con->createStatement ( bUpdate ? 
				odbc::ResultSet::CONCUR_UPDATABLE : odbc::ResultSet::CONCUR_READ_ONLY,
				odbc::ResultSet::TYPE_FORWARD_ONLY ) );
		boost::shared_ptr<odbc::ResultSet> ResSet (st->executeQuery( sQuery ) );
		if(!ResSet->next() ) throw DBBridge::IVideoConfig::NoSuchProperty("No such property in the 'Cameras' table");
		return std::make_pair( st, ResSet);
	}
public:
	CameraConfigDBImpl(  boost::shared_ptr<odbc::Connection> con, int nID );

private:
	boost::shared_ptr<odbc::Connection>	m_con;
	int									m_nID;
};

#endif // _VIDEO_CONFIG_D_B_IMPL_8364759408328925_