//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������� �� �-��������� libsqlite

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   21.04.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SQLITE_WRAPPER_4991479322359492_
#define _SQLITE_WRAPPER_4991479322359492_

//======================================================================================//
//                                 class SQLiteDatabase                                 //
//======================================================================================//

//! \brief ������� �� ����������� � ��, ��� ����������� �������� � ��
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   21.04.2006
//! \version 1.0
//! \bug 
//! \todo 

class SQLiteDatabase
{
	friend class SQLiteGenericStatement;
	boost::shared_ptr<sqlite3> m_pDatabase;
	class SQLiteQuery
	{
		boost::function2<void, int, char**> m_fn;

		void SqliteCallbackNonStatic( int argc, char **argv )
		{
			m_fn( argc, argv );
		}
	public:
		SQLiteQuery( boost::function2<void, int, char**> fn ) : m_fn(fn) 
		{
		}
		static int SqliteCallback( void *pArg, int argc, char **argv, char ** /*name*/ )
		{
			reinterpret_cast<SQLiteQuery*>(pArg)->SqliteCallbackNonStatic(argc, argv);
			return 0;
		}
	};

	std::string GetError() 
	{
		const char* szDescription = sqlite3_errmsg( m_pDatabase.get() );
		return std::string(szDescription); 
	}

	int SQLiteOpenDataBase( const wchar_t* szDatabasePath, sqlite3** ppRawDatabase)
	{
		return sqlite3_open16( szDatabasePath, ppRawDatabase);
	}

	int SQLiteOpenDataBase( const char* szDatabasePath, sqlite3** ppRawDatabase)
	{
		return sqlite3_open( szDatabasePath, ppRawDatabase);
	}

	template<typename CHAR_TYPE> void OpenDatabase( const CHAR_TYPE* szName )
	{
		sqlite3* pRawDatabase;
		int res = SQLITE_ERROR ;
		res = SQLiteOpenDataBase( szName, &pRawDatabase);
		if( SQLITE_OK == res)
		{
			m_pDatabase = boost::shared_ptr<sqlite3>(pRawDatabase, &sqlite3_close );
		}
		else
		{
			throw std::runtime_error( "Failed to open database" );
		}
	}
public:
	//! ������� ���������� � ��
	//! \param szName ���� � ����� �� (���� �� ����������, �� ����� ������)
	//! \exception std::runtime_error � ������ ������
	SQLiteDatabase(const char* szName)
	{
		OpenDatabase( szName );
	}

	//! ������� ���������� � ��
	//! \param szName ���� � ����� �� (���� �� ����������, �� ����� ������)
	//! \exception std::runtime_error � ������ ������
	SQLiteDatabase(const wchar_t* szName)
	{
		OpenDatabase( szName );
	}
};

//! \brief	������� ����� ��� SQL ��������
//! \version 1.0
//! \date 05-30-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class SQLiteGenericStatement
{
	boost::shared_ptr<sqlite3_stmt> m_pStatement;
	SQLiteDatabase&					m_db;
protected:
	sqlite3*	GetDBHandler() 
	{
		return m_db.m_pDatabase.get();
	}

	sqlite3_stmt*	GetStatementHandler() 
	{
		return m_pStatement.get();
	}

	SQLiteGenericStatement( SQLiteDatabase& db, const char* szSQLQuery ):
	m_db(db)
	{
		sqlite3_stmt* pRawStatement = 0;
		int res = sqlite3_prepare( GetDBHandler() , szSQLQuery, -1, &pRawStatement, NULL);
		if( SQLITE_OK == res )
		{
			m_pStatement = boost::shared_ptr<sqlite3_stmt>( pRawStatement, &sqlite3_finalize );
		}
		else
		{
			throw std::runtime_error( GetError() );
		}
	}

	std::string GetError() 
	{
		return m_db.GetError();
	}
};

//! ����� ��� �������� �� ��������� ������ (��������, ���������, �������)
/*! \code
	std::string sQuery("INSERT INTO Camera(CameraID, CameraNo) VALUES(1, 123)");
	SQLiteUpdateStatement st(db, sQuery.c_str());
	st.Update();
	\endcode
*/
class SQLiteUpdateStatement : private SQLiteGenericStatement
{
public:
	SQLiteUpdateStatement( SQLiteDatabase& db, const char* szSQLQuery ):
	  SQLiteGenericStatement(db, szSQLQuery)
	  {
	  }

	//! ��������� ������ (�������� �������)
	//! \return ���-�� ���������� �������� ������� � �������
	int Update()
	{
		int res = sqlite3_step( GetStatementHandler() );
		if (res != SQLITE_DONE)
		{
			throw std::runtime_error( GetError() );
		}
		return  sqlite3_changes( GetDBHandler() );
	}

	void Bind( int nIndex, int Value )
	{
		int res = sqlite3_bind_int( GetStatementHandler(), nIndex, Value );
		if( SQLITE_OK != res )
		{
			throw std::runtime_error( GetError() );
		}
	}

	void Bind( int nIndex, double Value )
	{
		int res = sqlite3_bind_double( GetStatementHandler(), nIndex, Value );
		if( SQLITE_OK != res )
		{
			throw std::runtime_error( GetError() );
		}
	}

	void Bind( int nIndex, const std::string& Value )
	{
		int res = sqlite3_bind_text( GetStatementHandler(), nIndex, Value.c_str(), Value.length(), SQLITE_TRANSIENT );
		if( SQLITE_OK != res )
		{
			throw std::runtime_error( GetError() );
		}
	}
};



//! ����� ��� ������� ������ �� ������
/*! \code
	std::string sQuery( 
	"SELECT CameraID, TimeBegin, TimeEnd, LeftBound, TopBound, RightBound, BottomBound "
	"FROM	Frames "
	);
	SQLiteSelectStatement FrameQuery( db, sQuery.c_str() );
	while( FrameQuery.FetchData() )
	{
	int			nCameraID	= FrameQuery.GetInt(0);
	std::string	sTimeBegin	= FrameQuery.GetText(1);
	std::string	sTimeEnd	= FrameQuery.GetText(2);
	float		x			= FrameQuery.GetDouble(3);
	float		y			= FrameQuery.GetDouble(4);
	float		w			= FrameQuery.GetDouble(5);
	float		h			= FrameQuery.GetDouble(6);
	}
\endcode
*/
class SQLiteSelectStatement : private SQLiteGenericStatement
{

public:
	//! 
	//! \param db			�������� ��
	//! \param szSQLQuery	������ �� ������
	//! \exception			std::runtime_error ���� ��������� ������ (���������)
	SQLiteSelectStatement( SQLiteDatabase& db, const char* szSQLQuery ):
		SQLiteGenericStatement(db, szSQLQuery)
	{
	}

	//! ������� ������ ������� �� ��, ����� ������ �-� ����� ������������ Get*
	//! \return ������ ��������/�� �������� � Get*
	bool FetchData()
	{
		int res = sqlite3_step( GetStatementHandler() );
		switch (res)
		{
		case SQLITE_ROW : return true;
		case SQLITE_DONE: return false;
						//	return true;
		default:
		//	throw std::runtime_error( GetError() );
			return false;
		}
	}

	//! �������� �������� ������ ���� ����� FetchData
	//! \param nColumn	���������� ����� ���� � �������
	//! \return ������������ ��������
	int		GetInt(int nColumn)
	{
		return sqlite3_column_int( GetStatementHandler(), nColumn );
	}

	//! �������� �������� ������������� ���� ����� FetchData
	//! \param nColumn	���������� ����� ���� � ������� SQLiteSelectStatement::SQLiteSelectStatement
	//! \return  �������� ����
	double	GetDouble(int nColumn)
	{
		return sqlite3_column_double( GetStatementHandler(), nColumn );
	}

	//! �������� �������� ���������� ���� ����� FetchData
	//! \param nColumn	���������� ����� ���� � ������� SQLiteSelectStatement::SQLiteSelectStatement
	//! \return ������
	const char* GetText(int nColumn)
	{
		return reinterpret_cast<const char*>(sqlite3_column_text(GetStatementHandler(), nColumn));
	}

	//! �������� �������� ��������� ���� ����� FetchData
	//! \param nColumn	���������� ����� ���� � ������� SQLiteSelectStatement::SQLiteSelectStatement
	//! \return �������� ����
	std::pair<const void *, size_t> GetBlob(int nColumn)
	{
		return std::make_pair(
							sqlite3_column_blob(GetStatementHandler(), nColumn) ,
							sqlite3_column_bytes(GetStatementHandler(), nColumn)
							);
	}
};

#endif // _SQLITE_WRAPPER_4991479322359492_