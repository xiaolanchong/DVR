//!
//!	DVR 
//!	Copyright(C) 2006 Elvees
//!	All rights reserved.
//!
//!	\file     DVRBackupImp.hpp		
//!	\date     2006-02-15		
//!	\author   Yanakov Leonid		
//!	\version  1.0	
//!	\brief    IDVRBackup implementation

#ifndef __DVRBACKUPIMP_HPP__
#define __DVRBACKUPIMP_HPP__

class Storage;

//! \class DVRBackupImp
//! \brief 
class DVRBackupImp : public IDVRBackup
{
	size_t		m_TotalStepsDone;
public:
	DVRBackupImp( boost::shared_ptr<Elvees::IMessage> message, 
					boost::shared_ptr<IBackupParam>		pParamInterface );
	~DVRBackupImp();

	void DebugThreadProc( )
	{
		ThreadProc( this );
	}
private:
	static void ThreadProc( void* p );

	static void HandledThreadProc( void* p );
private:
	boost::shared_ptr<boost::thread> mThread;
	boost::shared_ptr<Elvees::IMessage> mMessage;
	boost::shared_ptr<IBackupParam>		m_pParamInterface;

	void BackupStep(boost::shared_ptr<Storage> mStorage);
	bool							m_bWorking;
};

#endif //__DVRBACKUPIMP_HPP__
