/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:     wxTimer class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: timer.h,v 1.19 2005/09/23 12:50:17 MR Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_
#define _WX_TIMER_H_

class WXDLLEXPORT wxTimer : public wxTimerBase
{
friend void wxProcessTimer(wxTimer& timer);

public:
    wxTimer() { Init(); }
    wxTimer(wxEvtHandler *owner, int id = wxID_ANY) : wxTimerBase(owner, id)
        { Init(); }
    ~wxTimer();

    virtual bool Start(int milliseconds = -1, bool oneShot = false);
    virtual void Stop();

    virtual bool IsRunning() const { return m_id != 0; }

protected:
    void Init();

    unsigned long m_id;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxTimer)
};

#endif
    // _WX_TIMERH_
