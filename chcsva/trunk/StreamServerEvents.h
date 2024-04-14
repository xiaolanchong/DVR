// StreamServerEvents.h: interface for the CStreamServerEvents interface.
//
//////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////
// CStreamServerEvents
///////////////////////////////////////////////////////////////////////////////

#define SERVER_FULL		-1

class CStreamServerEvents
{
public:
	virtual long OnConnectionEstablished(LPCTSTR stAddress) = 0;
	virtual void OnConnectionClosed(long lSession) = 0;
};