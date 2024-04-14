//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Set/get database computer properties

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.03.2006
*/                                                                                      //
//======================================================================================//
#ifndef _COMPUTER_TABLE_6677994307285718_
#define _COMPUTER_TABLE_6677994307285718_

#include "../common/OwnIPAddress.h"

//======================================================================================//
//                                 class ComputerTable                                  //
//======================================================================================//

//! \brief Set/get database computer properties
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   30.03.2006
//! \version 1.0
//! \bug 
//! \todo 

class ComputerTable
{
	WinSock wsa;
protected:
	boost::shared_ptr<odbc::Connection>	m_con;
public:
	ComputerTable(const std::string& szConnectString);
	ComputerTable(boost::shared_ptr<odbc::Connection>	con);
	virtual ~ComputerTable();
	
	MACRO_EXCEPTION( ComputerTableException, std::runtime_error );

	void	GetValue( LPCTSTR szName, double& fValue) ;
	void	GetValue( LPCTSTR szName, DWORD& dwValue) ;
	void	GetValue( LPCTSTR szName, std::string& sValue ) ;
	void	GetValue( LPCTSTR szName, std::vector<BYTE>& binValue );
	

	void			SetValue( LPCTSTR szName, DWORD dwValue) ;
	void			SetValue( LPCTSTR szName, double dwValue) ;
	void			SetValue( LPCTSTR szName, LPCTSTR sValue ) ;
	void			SetValue( LPCTSTR szName, const std::vector<BYTE>& binValue );

protected:
	typedef std::pair<	boost::shared_ptr<odbc::Statement>,	boost::shared_ptr<odbc::ResultSet> > QueryRes_t;
private:

	QueryRes_t GetValue(LPCTSTR szName, bool bUpdate )
	{
		std::string sIP = GetOwnIPAddress();
		std::string sQuery = (boost::format("SELECT %s FROM Computers WHERE ComputerIP='%s'") % szName % sIP).str();

		boost::shared_ptr<odbc::Statement> st ( 
			m_con->createStatement (bUpdate ? odbc::ResultSet::TYPE_SCROLL_SENSITIVE : odbc::ResultSet::TYPE_FORWARD_ONLY ,
			bUpdate ? odbc::ResultSet::CONCUR_UPDATABLE : odbc::ResultSet::CONCUR_READ_ONLY) );
		boost::shared_ptr<odbc::ResultSet> ResSet (st->executeQuery( sQuery ) );
		if(!ResSet->next() ) throw ComputerTableException("No such property in the 'Computers' table");
		return std::make_pair( st, ResSet);
	}
protected:
	static void	UpdateValue( odbc::Connection* pCon, const std::string& sQuery )
	{
		boost::shared_ptr<odbc::Statement> st ( pCon->createStatement (  ) );
		if(!st->executeUpdate( sQuery ) ) throw ComputerTableException("No such property for update");
	}
protected:
	static std::string GetOwnIPAddress() ;
	
};

#endif // _COMPUTER_TABLE_6677994307285718_