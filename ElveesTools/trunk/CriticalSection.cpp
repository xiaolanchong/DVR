// CriticalSection.cpp: implementation of the CCriticalSection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CriticalSection.h"

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

CCriticalSection::CCriticalSection()
{
	::InitializeCriticalSection(&m_crit);
}
      
CCriticalSection::~CCriticalSection()
{
	::DeleteCriticalSection(&m_crit);
}

#if(_WIN32_WINNT >= 0x0400)
bool CCriticalSection::TryEnter()
{
	return (TRUE == ::TryEnterCriticalSection(&m_crit));
}
#endif

void CCriticalSection::Enter()
{
	::EnterCriticalSection(&m_crit);
}

void CCriticalSection::Leave()
{
	::LeaveCriticalSection(&m_crit);
}

//////////////////////////////////////////////////////////////////////
// CCriticalSection::Owner
//////////////////////////////////////////////////////////////////////

CCriticalSection::Owner::Owner(CCriticalSection &crit) : m_crit(crit)
{
	m_crit.Enter();
}

CCriticalSection::Owner::~Owner()
{
	m_crit.Leave();
}

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
