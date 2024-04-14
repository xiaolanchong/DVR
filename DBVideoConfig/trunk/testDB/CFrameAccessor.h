#pragma once

#include "ISSHelper.h"
#include "IVideoConfig.h"


class CFrameAccessor
{
public:
	DWORD m_dwCameraID;
	VARIANT m_TimeBegin;
	VARIANT m_TimeEnd;

	DBLENGTH m_dwCameraIDLength;
	DBLENGTH m_dwTimeEndLength;
	DBLENGTH m_dwTimeBeginLength;

	DBSTATUS m_dwCameraIDStatus;
	DBSTATUS m_dwTimeEndStatus;
	DBSTATUS m_dwTimeBeginStatus;

	void GetRowsetProperties(CDBPropSet* pPropSet)
	{
		pPropSet->AddProperty(DBPROP_CANFETCHBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
		pPropSet->AddProperty(DBPROP_CANSCROLLBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
	}

	void OpenDataSource( CSession cSession)
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

	BEGIN_COLUMN_MAP(CFrameAccessor)
		COLUMN_ENTRY_LENGTH_STATUS(1, m_dwCameraID, m_dwCameraIDLength, m_dwCameraIDStatus)
		COLUMN_ENTRY_LENGTH_STATUS(2, m_TimeBegin, m_dwTimeBeginLength, m_dwTimeBeginStatus)
		COLUMN_ENTRY_LENGTH_STATUS(3, m_TimeEnd, m_dwTimeEndLength, m_dwTimeEndStatus)
	END_COLUMN_MAP()
};

class CFrameProperty : public CCommand<CAccessor<CFrameAccessor > >
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

		// Последняя false означает что комманда ничего не возвращает и не надо пытаться сбиндить результат
		HRESULT hRes = Open(m_session, szSQL, &propset, NULL, DBGUID_DEFAULT, bUpdate);

		if(FAILED(hRes))
			AtlTraceErrorRecords(hRes);

		return hRes;
	}

	void CloseAll()
	{
		Close();
		ReleaseCommand();
		CloseDataSource();
	}
};