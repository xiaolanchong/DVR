/**
 * This widget is a time-picker control that is built by combining
 * a customised text control with a customised spin button.
 * 
 * @version $Id: MyTimeCtrl.h, v 0.01 2003/03/24 14:40:05 Eddy Young $
 */

#ifndef _MYTIMECTRL_
#define _MYTIMECTRL_

#include <wx/wx.h>
#include <wx/spinbutt.h>

#define SPACING 2

enum IncrementType
{
	POSITIVE = 1,
	NEGATIVE = 2
};

class MyTimeCtrl;

/**
 * This custom time change event triggers whenever the value in the text
 * control changes.
 */
class MyTimeChangeEvent : public wxNotifyEvent
{
public:
	/**
	 * Default constructor
	 */
	MyTimeChangeEvent();
	
	/**
	 * Normal constructor
	 */
	MyTimeChangeEvent(wxEventType type, wxWindowID id = -1, const wxString& value = wxT("") );
	
	/**
	 * To cater for Clone() function
	 * 
	 * @see @ref #Clone()
	 */
	MyTimeChangeEvent(const MyTimeChangeEvent& event);
	
	/**
	 * Set value
	 */
	void SetValue(const wxString& value);
	
	/**
	 * Get value
	 */
	wxString GetValue() const;
	
	/**
	 * Clone 
	 */
	virtual wxEvent *Clone();
	
private:
	wxString val;

	DECLARE_DYNAMIC_CLASS(MyTimeChangeEvent)
};

typedef void (wxEvtHandler::*MyEventFunction) (MyTimeChangeEvent&);

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(MyEVT_TIMECHANGE, 1)
END_DECLARE_EVENT_TYPES()

#define EVT_TIMECHANGE(id, func)  \
	DECLARE_EVENT_TABLE_ENTRY(MyEVT_TIMECHANGE, id, -1, (wxObjectEventFunction) (wxEventFunction) (MyEventFunction) & func, (wxObject *) NULL ),
	
/**
 * This control is the text part of the time-picker.
 */
class MyTextCtrl : public wxTextCtrl
{
public:
	/**
	 * Default constructor
	 */
	MyTextCtrl();
	
	/**
	 * Normal constructor
	 */
	MyTextCtrl(MyTimeCtrl *timectrl, const wxString& value = wxT("") );
	  
	/**
	 * Destructor
	 */
	~MyTextCtrl();
	
	/**
	 * This function is called from the spin control to 
	 * increment the value
	 *
	 * @param IncrementType type
	 */
	void ApplyIncrement(IncrementType type);
	
private:
	MyTimeCtrl *tc;
	wxString buffer;
	int pos, min, max;

	/**
	 * This event handler is called when a key is pressed in the text ctrl
	 *
	 * @param event
	 */
	void OnChar(wxKeyEvent& event);

	/**
	 * This funciton updates the caret position
	 */
	void UpdatePosition();
	
	/**
	 * This function sets the selection to the appropriate time part
	 */
	void SelectPart();
	
	/**
	 * This function updates the buffer
	 */
	void UpdateBuffer();
	
	/**
	 * This function updates the current time part (hour, minute, second or
	 * am/pm) based on the type of the change
	 *
	 * @param type
	 */
	void Increment(IncrementType type);

	/**
	 * Flush the buffer
	 */
	void FlushBuffer(bool clear = TRUE);
	 
	/**
	 * This function checks whether the valuefalls within the limit
	 *
	 * @param value
	 */
	void FixValue(long *value);
	
	
	DECLARE_DYNAMIC_CLASS(MyTextCtrl)
	DECLARE_EVENT_TABLE()
};

/**
 * This control is the spin button of the time picker.
 */
class MySpinButton : public wxSpinButton
{
public:
 	/** 
 	 * Default constructor
 	 */
 	MySpinButton();
 	 
 	/**
 	 * Normal constructor
 	 */
 	MySpinButton(MyTimeCtrl *timectrl);
 	
 	/**
 	 * Destructor
 	 */
 	~MySpinButton();
 	
 	/**
 	 * These functions are called when the spin button is pressed
 	 *
 	 * @param wxSpinEvent&
 	 */
 	void OnSpinUp(wxSpinEvent& event);
 	
 	/**
 	 * @see @ref #OnSpinUp(wxSpinEvent& event)
 	 */
 	void OnSpinDown(wxSpinEvent& event);
 	
 private:
 	MyTimeCtrl *tc;	

	DECLARE_DYNAMIC_CLASS(MySpinButton)
	DECLARE_EVENT_TABLE()
};

class MyTimeCtrl : public wxControl
{
public:
	/**
	 * Normal constructor
	 */
	MyTimeCtrl(wxWindow *parent = NULL, wxWindowID id = -1,
		const wxString& value = wxT(""), const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize);
		
	/**
	 * Destructor
	 */
	~MyTimeCtrl();
	
	/**
	 * This function triggers an update on the text control part, indicating
	 * an increment or a decrement with the parameter @p type.
	 *
	 * @param type
	 */
	void UpdateTextCtrl(IncrementType type);
	
	/**
	 * This function returns the value of the time control.
	 *
	 * @return Time, const wxString&
	 */
	wxString GetValue() const;
	
	/**
	 * Static function to get current time as expected by the control
	 */
	static wxString GetCurrentTime();
		
	/**
	 * This function shows or hides the control.
	 *
	 * @param show (TRUE to show; FALSE to hide)
	 * @return bool
	 */
	virtual bool Show(bool show = TRUE);

	/**
	 * This function enables or disables the control
	 *
	 * @param true or false
	 */
	virtual bool Enable(bool enable = TRUE);
	
protected:
	/**
	 * This function overrides the wxControl::DoMoveWindow() function so
	 * that the two parts of the control are synchronised.
	 *
	 * @see wxControl::DoMoveWindow(int x, int y, int width, int height)
	 */
	void DoMoveWindow(int x, int y, int width, int height);
	
	/**
	 * This function overrides the wxControl::DoGetBestSize() function so
	 * that the correct "best size" of the combined controls is returned.
	 *
	 * @see wxControl::DoGetBestSize()
	 */
	wxSize DoGetBestSize() const;
	
private:
	MyTextCtrl *tc;
	MySpinButton *sb;

	DECLARE_DYNAMIC_CLASS(MyTimeCtrl)
};

#endif // _MYTIMECTRL_
