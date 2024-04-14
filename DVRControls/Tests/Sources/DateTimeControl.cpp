/*
*	Components 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: DateTimeControl.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-12-23
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: DateTime control implementation
*
*
*/
#include "StdAfx.h"
#include "DateTimeControl.hpp"
//////////////////////////////////////////////////////////////////////////
//01.01.2005 12:00:00
const int DTFormat[6][2] = { {0,2}, {3,5}, {6,10}, {12,14}, {15,17}, {18,20} };
//////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( DateTimeControl, wxWindow )
	EVT_KEY_UP( DateTimeControl::OnKeyUp )
	EVT_KEY_DOWN( DateTimeControl::OnKeyDown )
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////////
DateTimeControl::DateTimeControl( wxWindow* parent, wxWindowID id,  const wxPoint& pos, const wxSize& size ):
	wxWindow( parent, id, pos, size ), mSelectionType( DateTimeControl::ST_Unknown ), mHoldingInput( false )
{

	//Default spin control width

	wxPoint textPanePt( pos.x, pos.y );
	wxSize textPaneSize( size.x - mSpinButtonWidth, size.y );

	wxPoint spinPanePt( pos.x + size.x - mSpinButtonWidth, pos.y );
	wxSize spinPaneSize( mSpinButtonWidth, size.y );

    mTextPane = new wxTextCtrl( this, -1, wxEmptyString, textPanePt, textPaneSize, wxTE_NOHIDESEL );
	mSpinButton = new wxSpinButton( this, -1, spinPanePt, spinPaneSize );

	//Set default title
	mCurrentDT = to_tm( boost::posix_time::second_clock::local_time() );
	DateTime_FormatTitle();

	//Connect event handlers
	mTextPane->Connect( wxEVT_LEFT_DOWN, (wxObjectEventFunction)&DateTimeControl::TextPane_LeftClick );
	mTextPane->Connect( wxEVT_LEFT_DCLICK, (wxObjectEventFunction)&DateTimeControl::TextPane_LeftClick );
	mTextPane->Connect( wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&DateTimeControl::TextPane_RightClick );
	mTextPane->Connect( wxEVT_RIGHT_DCLICK, (wxObjectEventFunction)&DateTimeControl::TextPane_RightClick );

	mSpinButton->Connect( wxEVT_SCROLL_LINEUP, (wxObjectEventFunction)&DateTimeControl::SpinButton_Up );
	mSpinButton->Connect( wxEVT_SCROLL_LINEDOWN, (wxObjectEventFunction)&DateTimeControl::SpinButton_Down );

	mTextPane->SetClientData( this );
	mSpinButton->SetClientData( this );
}

DateTimeControl::~DateTimeControl()
{
	//KB
}

//////////////////////////////////////////////////////////////////////////
void DateTimeControl::TextPane_Update( wxCommandEvent& event )
{

}

void DateTimeControl::TextPane_LeftClick( wxMouseEvent& event )
{
	wxPoint pt = event.GetPosition();
	wxTextCtrl* textCtrl = (wxTextCtrl*)event.GetEventObject();

	wxTextCoord txCoordRow;
	wxTextCoord txCoordCol;

	//Retrieve parent control 
	DateTimeControl* control = (DateTimeControl*)textCtrl->GetClientData();

	if( wxTE_HT_UNKNOWN != textCtrl->HitTest( pt, &txCoordCol, &txCoordRow ) )
	{
		int section = 0;

		//Detect digits group
		if( txCoordCol >= DTFormat[0][0] && txCoordCol <= DTFormat[0][1] ) //Day
		{
			section = 0;
			control->mSelectionType = ST_Day;
		}
		else if( txCoordCol >= DTFormat[1][0] && txCoordCol <= DTFormat[1][1] ) //Month
		{
			section = 1;
			control->mSelectionType = ST_Month;
		}
		else if( txCoordCol >= DTFormat[2][0] && txCoordCol <= DTFormat[2][1] ) //Year
		{
			section = 2;
			control->mSelectionType = ST_Year;
        }
		else if( txCoordCol >= DTFormat[3][0] && txCoordCol <= DTFormat[3][1] ) //Hour
		{
			section = 3;
			control->mSelectionType = ST_Hour;
		}
		else if( txCoordCol >= DTFormat[4][0] && txCoordCol <= DTFormat[4][1] ) //Minute
		{
			section = 4;
			control->mSelectionType = ST_Minute;
		}
		else if( txCoordCol >= DTFormat[5][0] && txCoordCol <= DTFormat[5][1] ) //Second
		{
			section = 5;
			control->mSelectionType = ST_Second;
		}

		textCtrl->SetSelection( DTFormat[section][0], DTFormat[section][1] );            
	}

}

void DateTimeControl::TextPane_RightClick( wxMouseEvent& event )
{
	//NOTE: Keep it blank while context menu don't needed
}
//////////////////////////////////////////////////////////////////////////
void DateTimeControl::SpinButton_Up( wxSpinEvent& event )
{
	//Retrieve parent control 
	wxSpinButton* spinCtrl = (wxSpinButton*)event.GetEventObject();
	DateTimeControl* control = (DateTimeControl*)spinCtrl->GetClientData();

	control->DateTime_Up();
	control->DateTime_FormatTitle();
}

void DateTimeControl::SpinButton_Down( wxSpinEvent& event )
{
	//Retrieve parent control 
	wxSpinButton* spinCtrl = (wxSpinButton*)event.GetEventObject();
	DateTimeControl* control = (DateTimeControl*)spinCtrl->GetClientData();

	control->DateTime_Down();
	control->DateTime_FormatTitle();
}

//////////////////////////////////////////////////////////////////////////
void DateTimeControl::OnKeyDown(wxKeyEvent& event)
{
	long keycode = event.GetKeyCode();

	//Handle digits specially
	if( keycode >= 0x30  && keycode <= 0x39 )
	{
        TextPane_HandleInput( keycode - 0x30 );
	}
	else
	{
		//Release input & clear input buffer
		TextPane_ReleaseInput();

		//Process key code
		switch ( keycode )
		{
		case WXK_UP:
			DateTime_Up();
			break;
		case WXK_DOWN:
			DateTime_Down();
			break;
		case WXK_LEFT:
			DateTime_Left();
			break;
		case WXK_RIGHT:
			DateTime_Right();
			break;
		}
	}

	//Format edit control text
	DateTime_FormatTitle();
}

void DateTimeControl::OnKeyUp(wxKeyEvent& event)
{
	//NOTE: Keep it blank, handle pressing in OnKeyDown
}

void DateTimeControl::TextPane_HandleInput( long digit )
{
	mHoldingInput = true;
	if( ( ( mSelectionType == ST_Year ) && ( mInputBuffer.size() < 4 ) ) || ( mInputBuffer.size() < 2 ) )
	{
		mInputBuffer += boost::str( boost::format( "%d") % digit );
	}
}

void DateTimeControl::TextPane_ReleaseInput()
{
	//If input didn't holded
	if( !mHoldingInput ){
		return;
	}

	mHoldingInput = false;

	//If buffer consist four digits for year or two digits for day,month, etc
	if( ((mSelectionType == ST_Year) && ( mInputBuffer.size() == 4 ) ) || ( mInputBuffer.size() == 2 ) )
	{
		int bufferValue = atoi( mInputBuffer.c_str() );
		unsigned short lastDay = boost::gregorian::gregorian_calendar::end_of_month_day( 1900 + mCurrentDT.tm_year, mCurrentDT.tm_mon + 1 );

		switch( mSelectionType )
		{
		case ST_Year:
			if( bufferValue >= mMinYear && bufferValue < mMaxYear )
			{
				mCurrentDT.tm_year = bufferValue - 1900;
			}
			break;
		case ST_Month:
			if( bufferValue > 0 && bufferValue <= 12   )
			{
                mCurrentDT.tm_mon = bufferValue - 1;
			}
			break;
		case ST_Day:
			if( bufferValue > 0 && bufferValue < 32   )
			{
				//Correct day
				bufferValue = bufferValue > lastDay ? lastDay: bufferValue;
				mCurrentDT.tm_mday = bufferValue;
			}
			break;
		case ST_Hour:
			if( bufferValue >= 0 && bufferValue < 24   )
			{
				mCurrentDT.tm_hour = bufferValue;
			}
			break;
		case ST_Minute:
			if( bufferValue >= 0 && bufferValue < 60   )
			{
				mCurrentDT.tm_min = bufferValue;
			}
			break;
		case ST_Second:
			if( bufferValue >= 0 && bufferValue < 60   )
			{
				mCurrentDT.tm_sec = bufferValue;
			}
			break;
		}

		//Correct date\time
		DateTime_Correction( true );
	}

	//Clear input buffer
	mInputBuffer.clear();
}
//////////////////////////////////////////////////////////////////////////
void DateTimeControl::DateTime_FormatTitle()
{
    std::string title;

	if( mHoldingInput )
	{
		//Format default values
		std::string day =  boost::str( boost::format( "%02d" ) % mCurrentDT.tm_mday );
		std::string month = boost::str( boost::format( "%02d" ) % ( mCurrentDT.tm_mon + 1 ) );
		std::string year = boost::str( boost::format( "%04d" ) %  ( 1900 + mCurrentDT.tm_year ) );
		std::string hour = boost::str( boost::format( "%02d" ) %mCurrentDT.tm_hour );
		std::string minute = boost::str( boost::format( "%02d" ) % mCurrentDT.tm_min );
		std::string second = boost::str( boost::format( "%02d" ) % mCurrentDT.tm_sec );

		switch( mSelectionType )
		{
		case ST_Year:
			year = mInputBuffer;
			break;
		case ST_Month:
			month = mInputBuffer;
			break;
		case ST_Day:
			day = mInputBuffer;
			break;
		case ST_Hour:
			hour = mInputBuffer;
			break;
		case ST_Minute:
			minute = mInputBuffer;
			break;
		case ST_Second:
			second = mInputBuffer;
			break;
		}

		title = boost::str( boost::format( "%s.%s.%s  %s.%s.%s" )
			% day % month % year % hour % minute % second );
	}
	else
	{
		title = boost::str( boost::format( "%02d.%02d.%04d  %02d:%02d:%02d" ) 
			% mCurrentDT.tm_mday % ( mCurrentDT.tm_mon + 1 ) % ( 1900 + mCurrentDT.tm_year ) % mCurrentDT.tm_hour % mCurrentDT.tm_min % mCurrentDT.tm_sec );
	}

	//Set edit control text and selection
	mTextPane->SetTitle( title.c_str() );
	mTextPane->SetSelection( DTFormat[mSelectionType][0], DTFormat[mSelectionType][1] ); 
}

void DateTimeControl::DateTime_Left()
{
	if( mSelectionType > 0 )
	{
		mSelectionType = (SelectionType)( mSelectionType - 1 );
	}

	mTextPane->SetSelection( DTFormat[mSelectionType][0], DTFormat[mSelectionType][1] ); 
}

void DateTimeControl::DateTime_Right()
{
	if( mSelectionType < ( ST_Last - 1) )
	{
		mSelectionType = (SelectionType)( mSelectionType + 1);
	}

	mTextPane->SetSelection( DTFormat[mSelectionType][0], DTFormat[mSelectionType][1] ); 
}

void DateTimeControl::DateTime_Down()
{
	switch( mSelectionType )
	{
	case ST_Year:
		--mCurrentDT.tm_year;
		break;

	case ST_Month:
		mCurrentDT.tm_mon = (--mCurrentDT.tm_mon < 0 ? 11 : mCurrentDT.tm_mon );
		break;

	case ST_Day:
		mCurrentDT.tm_mday= (--mCurrentDT.tm_mday < 1 ? 31 : mCurrentDT.tm_mday  );
		break;

	case ST_Hour:
		mCurrentDT.tm_hour= (--mCurrentDT.tm_hour < 0 ? 23 : mCurrentDT.tm_hour );
		break;

	case ST_Minute:
		mCurrentDT.tm_min = (--mCurrentDT.tm_min < 0 ? 59 : mCurrentDT.tm_min );
		break;

	case ST_Second:
		mCurrentDT.tm_sec = (--mCurrentDT.tm_sec < 0 ? 59 : mCurrentDT.tm_sec );
		break;

	default:
		mCurrentDT.tm_mday= (--mCurrentDT.tm_mday < 1 ? 31 : mCurrentDT.tm_mday  );
		break;
	}

	//Correct modified date\time ( to down )
	DateTime_Correction( false );
}

void DateTimeControl::DateTime_Up()
{

	switch( mSelectionType )
	{
	case ST_Year:
		++mCurrentDT.tm_year;
		break;

	case ST_Month:
		mCurrentDT.tm_mon = ( ++mCurrentDT.tm_mon > 11 ? 0 : mCurrentDT.tm_mon );
		break;

	case ST_Day:
		mCurrentDT.tm_mday= ( ++mCurrentDT.tm_mday > 31 ? 1 : mCurrentDT.tm_mday  );
		break;

	case ST_Hour:
		mCurrentDT.tm_hour= ( ++mCurrentDT.tm_hour > 23 ? 0 : mCurrentDT.tm_hour );
		break;

	case ST_Minute:
		mCurrentDT.tm_min = ( ++mCurrentDT.tm_min > 59 ? 0 : mCurrentDT.tm_min );
		break;

	case ST_Second:
		mCurrentDT.tm_sec = ( ++mCurrentDT.tm_sec > 59 ? 0 : mCurrentDT.tm_sec );
		break;

	//By default modify day ( ex. no selection )
	default:
		mCurrentDT.tm_mday= ( ++mCurrentDT.tm_mday > 31 ? 1 : mCurrentDT.tm_mday  );
		break;
	}

	//Correct modified date\time ( to up )
    DateTime_Correction( true );
}

void DateTimeControl::DateTime_Correction( bool upDirection )
{
	//Perform correction
	unsigned short lastDay = boost::gregorian::gregorian_calendar::end_of_month_day( 1900 + mCurrentDT.tm_year, mCurrentDT.tm_mon + 1 );
	if( mCurrentDT.tm_mday > lastDay )
	{
		//If day is modified 
		if( ( mSelectionType == ST_Day ) && upDirection )
			mCurrentDT.tm_mday = 1;
		else
			mCurrentDT.tm_mday = lastDay;
	}
}

//////////////////////////////////////////////////////////////////////////
tm DateTimeControl::GetCurrentDateTime()
{
	return mCurrentDT;
}
