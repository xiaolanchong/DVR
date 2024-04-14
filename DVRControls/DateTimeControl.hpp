/*
*	Components 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: DateTimeControl.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-12-23
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: DateTime picker control
*
*
*/
#ifndef __DATETIMECONTROL_HPP__
#define __DATETIMECONTROL_HPP__

class DateTimeControl : public wxWindow
{
	enum SelectionType 
	{
        ST_Day = 0,
		ST_Month,
		ST_Year,
		ST_Hour,
		ST_Minute,
		ST_Second,
		ST_Last,
		ST_Unknown
	};

	//Min&Max valid dates
	static const int mMaxYear = 2199;
	static const int mMinYear = 1900;

	//Default control dimensions
	static const int mSpinButtonWidth = 20;

public:
	DateTimeControl( wxWindow* parent, wxWindowID id,  const wxPoint& pos, const wxSize& size );
	virtual ~DateTimeControl();
public:
	tm		GetCurrentDateTime();
	void	SetCurrentDateTime(const tm& NewTime);

protected:
	void DateTime_Up();
	void DateTime_Down();
	void DateTime_Left();
	void DateTime_Right();
	void DateTime_FormatTitle();
	void DateTime_Correction( bool upDirection );

protected:
	void TextPane_HandleInput( long digit );
	void TextPane_ReleaseInput();
	void TextPane_Update( wxCommandEvent& event );
	void TextPane_LeftClick( wxMouseEvent& event );
	void TextPane_RightClick( wxMouseEvent& event );
	void SpinButton_Up( wxSpinEvent& event );
	void SpinButton_Down( wxSpinEvent& event );
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	DECLARE_EVENT_TABLE()

private:
	SelectionType mSelectionType;
	wxTextCtrl* mTextPane;
	wxSpinButton* mSpinButton;
	tm mCurrentDT;
	bool mHoldingInput;
	std::string mInputBuffer;
};

#endif //__DATETIMECONTROL_HPP__
