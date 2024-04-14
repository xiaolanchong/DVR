/*
*	Components 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: DTControlTestApp.cpp
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
#include "DTControlTestFrame.hpp"
#include "DTControlTestApp.hpp"


bool DTControlTestApp::OnInit()
{
	DTControlTestFrame* frame = new DTControlTestFrame( "DateTime control test frame", wxPoint( 50,50 ), wxSize( 300, 400 ) );
	
	frame->Show();

	SetTopWindow( frame );
	return true;		
}

