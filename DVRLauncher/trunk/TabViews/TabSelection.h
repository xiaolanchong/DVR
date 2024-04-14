//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	helper class for easy tab selection
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 20.03.2005
//                                                                                      //
//======================================================================================//
#ifndef _TAB_SELECTION_3802624824689572_
#define _TAB_SELECTION_3802624824689572_
//======================================================================================//
//                                  class TabSelection                                  //
//======================================================================================//

class SelectionTag
{
#if 1
	LONG				m_lCookie;
	static	LONG	GenerateCookie()
	{
		static LONG s_lCurCookie = 0;
		return InterlockedIncrement( &s_lCurCookie );
	}
#endif
	CString	m_TagName;
public:
	SelectionTag(CString sTagName):
		m_TagName(sTagName),
		m_lCookie( GenerateCookie() )
		{}
	virtual ~SelectionTag(){};
public:
#if 1
	LONG				GetCookie() const { return m_lCookie; };
#endif
	virtual bool		operator == ( const SelectionTag& rhs )
	{
		return GetCookie() == rhs.GetCookie();
	}
	virtual CString		GetTabName() const
	{
		return m_TagName;
	}
};

class TabSelection
{
	boost::shared_ptr<SelectionTag>	m_Tag;
public:
	void					SetTag(boost::shared_ptr<SelectionTag> p ) 
	{ 
		m_Tag = p;
	}
	const SelectionTag*		GetTag() const 
	{ 
		return m_Tag.get() ;
	}
	TabSelection();
	virtual ~TabSelection();
};

#endif // _TAB_SELECTION_3802624824689572_