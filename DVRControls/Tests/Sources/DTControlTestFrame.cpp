/*
*	Components 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: DTControlTestFrame.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-01-09
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "StdAfx.h"

#include "DateTimeControl.hpp"
#include "ScheduleControl.hpp"
#include "CameraStateControl.hpp"
#include "DTControlTestFrame.hpp"

DTControlTestFrame::DTControlTestFrame( const wxString& title, const wxPoint& pos, const wxSize& size ):
	wxFrame( (wxFrame*)NULL, -1, title, pos, size )
{
	//int answer = wxMessageBox("Test DateTimeControl instead ScheduleControl?", "Confirm",
	//	wxYES_NO | wxNO, this);
	//if (answer == wxYES)
	//{

	//	wxPoint ptDT( 50, 50 );		
	//	wxSize szDT( 150,23 );
	//	mDateTimeControl = new DateTimeControl( this, 100, ptDT, szDT );

	//}
	//else
	//{
		wxPoint ptSC( 150, 50 );		
		wxSize szSC( 300,300 );
		mSheduleControl = new ScheduleControl( this, 101, ptSC, szSC );
	//}
	
	//wxPoint ptCSC( 150, 50 );		
	//wxSize szCSC( 300,300 );
	//mCameraStateControl = new CameraStateControl( this, 102, ptCSC, szCSC );

}
