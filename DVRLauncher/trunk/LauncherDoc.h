// LauncherDoc.h : interface of the CLauncherDoc class
//
#pragma once

#define USE_SUPER_SERVER

#include <vector>
#include <afxmt.h>
#include <mmsystem.h>
using std::max;
using std::min;
#include <boost/shared_ptr.hpp>

#include "HtmlLog/CyclicLog.h"

class CAnalyzerView;
class CLauncherView;

enum ViewType
{
	View_MainServer,
	View_Camera
};

typedef std::map< int, std::vector<int > >	CameraMap_t;

class CLauncherDoc : public CDocument
{
	void	DeleteMainView();


	void	CreateSettingsWindow(CCreateContext& cc);

	

	std::vector< boost::shared_ptr<IDebugOutput> >			m_DebugArr;

	CString	GetCommonLogDirectory() const;

	template <class T>	T*	GetView()
	{
		POSITION pos = GetFirstViewPosition();
		while( pos )
		{
			CView* pView = GetNextView( pos );
			if( pView->IsKindOf( RUNTIME_CLASS( T ) ) ) return (T*)pView;
		}
		return 0;
	}

	CLauncherView*	GetLauncherView();
	
protected: // create from serialization only
	CLauncherDoc();
	DECLARE_DYNCREATE(CLauncherDoc)

	boost::shared_ptr<IDebugOutput>	CreateDebugOutput( 
		CCreateContext& cc,
		CString sTabName,
		bool bActiveTab = false);

	bool	InitializeData( CCreateContext& cc);

	virtual bool CreateEnvironment(CCreateContext& cc);
	virtual void DestroyEnvironment();
	
	int		GetMaxLineNumber();
public:
// Implementation
public:
	virtual ~CLauncherDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:

	virtual void OnCloseDocument();
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

};


