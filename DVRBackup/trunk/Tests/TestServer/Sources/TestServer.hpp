//!
//!	DVR 
//!	Copyright(C) 2006 Elvees
//!	All rights reserved.
//!
//!	\file     TestServer.hpp		
//!	\date     2006-02-16		
//!	\author   Yanakov Leonid		
//!	\version  1.0	
//!	\brief    This is default brief			
#ifndef __TESTSERVER_HPP__
#define __TESTSERVER_HPP__

class IDVRBackup;
class DebugInfo;

//////////////////////////////////////////////////////////////////////////
#if 0
//! \class TestServerApp
//! \brief TestServer application class
class TestServerApp : public wxApp
{
public:
	TestServerApp();
	virtual ~TestServerApp();
public:
	bool OnInit();

private:
	boost::shared_ptr<wxDynamicLibrary> mDVRBackupDynLib;
	boost::shared_ptr<IDVRBackup> mDVRBackup;
	boost::shared_ptr<DebugInfo> mDebugInfo;
};

IMPLEMENT_APP_CONSOLE(TestServerApp)

//////////////////////////////////////////////////////////////////////////

//! \class TestServerFrame
//! \brief TestServer main frame
class TestServerFrame : public wxFrame
{
public:
	TestServerFrame( const wxString& title );
	virtual ~TestServerFrame();

};
#endif
//////////////////////////////////////////////////////////////////////////

//! \class DebugInfo
//!	\brief Printing debug messages
class DebugInfo : public Elvees::IMessage
{
public:
	void Print( sint messageType, const char * szMessage );
};

//////////////////////////////////////////////////////////////////////////





#endif //__TESTSERVER_HPP__
