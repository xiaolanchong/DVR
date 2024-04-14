/*
*	Components 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: DTControlTestFrame.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-01-09
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __DTCONTROLTESTFRAME_HPP__
#define __DTCONTROLTESTFRAME_HPP__

class DateTimeControl;
class ScheduleControl;
class CameraStateControl;

class DTControlTestFrame : public wxFrame
{
public:
	DTControlTestFrame( const wxString& title, const wxPoint& pos, const wxSize& size );

private:
	DateTimeControl* mDateTimeControl; 
	ScheduleControl* mSheduleControl;
	CameraStateControl* mCameraStateControl;

};


#endif //__DTCONTROLTESTFRAME_HPP__
