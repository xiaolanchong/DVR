#pragma once

#include "ISSHelper.h"
#include "IVideoConfig.h"


class CCategories
{
public:
   ISequentialStream*   pBlobField;
   DBLENGTH m_dwFieldBlobLength;
   DBSTATUS m_dwFieldBlobStatus;

   void GetRowsetProperties(CDBPropSet* pPropSet)
	{
		pPropSet->AddProperty(DBPROP_CANFETCHBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
		pPropSet->AddProperty(DBPROP_CANSCROLLBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
	}

   void OpenDataSource( CSession& cSession)
   {
	  m_session = cSession;
   }

	void CloseDataSource()
	{
		m_session.Close();
	}

	operator const CSession&()
	{
		return m_session;
	}

	CSession m_session;

BEGIN_COLUMN_MAP(CCategories)
	BLOB_ENTRY_LENGTH_STATUS(1, IID_ISequentialStream, STGM_READ, pBlobField, m_dwFieldBlobLength, m_dwFieldBlobStatus )
END_COLUMN_MAP()
};

class CBlobProperty : public CCommand<CAccessor<CCategories > >
{
public:

	HRESULT DoCommand(LPCTSTR szSQL, bool bUpdate)
	{
		// Запросить настройки
		CDBPropSet propset(DBPROPSET_ROWSET);

		if(!bUpdate)
		{
			propset.AddProperty(DBPROP_IRowsetChange, true);
			propset.AddProperty(DBPROP_CANFETCHBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
			propset.AddProperty(DBPROP_CANSCROLLBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
			propset.AddProperty(DBPROP_IRowsetChange, true, DBPROPOPTIONS_OPTIONAL);
			propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE | DBPROPVAL_UP_INSERT | DBPROPVAL_UP_DELETE);
		}

		GetRowsetProperties(&propset);

		HRESULT hRes = Open(m_session, szSQL, &propset);

		if(FAILED(hRes))
		AtlTraceErrorRecords(hRes);

		return hRes;
	}

	HRESULT OpenRowset(LPCTSTR szSQLQuery, DBPROPSET *pPropSet = NULL)
	{

		HRESULT hr = Open(m_session, szSQLQuery, pPropSet);
#ifdef _DEBUG
		if(FAILED(hr))
			AtlTraceErrorRecords(hr);
#endif
		return hr;
	}

	void CloseAll()
	{
		Close();
		ReleaseCommand();
		CloseDataSource();
	}
};