// Events.h: interface for the CEvent, CManualResetEvent, CAutoResetEvent classes.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEESTOOLS_EVENTS_INCLUDED_
#define ELVEESTOOLS_EVENTS_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////
// CEvent
//////////////////////////////////////////////////////////////////////

class CEvent
{
public:
	explicit CEvent(
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		bool manualReset,
		bool initialState);
   
	explicit CEvent(
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		bool manualReset,
		bool initialState,
		LPCTSTR lpName);
      
	virtual ~CEvent();

	HANDLE GetEvent() const;

	void Wait() const;
	bool Wait(DWORD timeoutMillis) const;

	void Reset();
	void Set();
	void Pulse();

private:
	HANDLE m_hEvent;

	// No copies do not implement
	CEvent(const CEvent &rhs);
	CEvent &operator=(const CEvent &rhs);
};

//////////////////////////////////////////////////////////////////////
// CManualResetEvent
//////////////////////////////////////////////////////////////////////

class CManualResetEvent : public CEvent
{
public:
	explicit CManualResetEvent(bool initialState = false);
	explicit CManualResetEvent(LPCTSTR lpName, bool initialState = false);

private:
	// No copies do not implement
	CManualResetEvent(const CManualResetEvent &rhs);
	CManualResetEvent &operator=(const CManualResetEvent &rhs);
};

//////////////////////////////////////////////////////////////////////
// CAutoResetEvent
//////////////////////////////////////////////////////////////////////

class CAutoResetEvent : public CEvent
{
public:
	class Owner
	{
	public:
		explicit Owner(CAutoResetEvent &event);
		~Owner();
      
	private:
		CAutoResetEvent &m_event;

		// No copies do not implement
		Owner(const Owner &rhs);
		Owner &operator=(const Owner &rhs);
	};

	explicit CAutoResetEvent(bool initialState = false);
	explicit CAutoResetEvent(LPCTSTR lpName, bool initialState = false);

private:
	// No copies do not implement
	CAutoResetEvent(const CAutoResetEvent &rhs);
	CAutoResetEvent &operator=(const CAutoResetEvent &rhs);
};

//////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEESTOOLS_EVENTS_INCLUDED_
