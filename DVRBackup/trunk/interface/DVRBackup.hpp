/*
*	DVR 
*	Copyright(C) 2006 Elvees
*	All rights reserved.
*
*	$Filename: DVRBackup.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-02-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: DVRBackup interface
*
*
*/
#ifndef __DVRBACKUP_HPP__
#define __DVRBACKUP_HPP__

class IBackupParam;
namespace Elvees
{
	class IMessage;
}

class IDVRBackup
{
public:
	virtual ~IDVRBackup() = 0 {};
};

#ifdef DVRBACKUP_EXPORTS
#define BACKUP_API __declspec(dllexport)
#else
#define BACKUP_API __declspec(dllimport)
#endif	//DVRBACKUP_EXPORTS


boost::shared_ptr<IDVRBackup> CreateBackupInterface( 
						boost::shared_ptr<Elvees::IMessage>	pLogInterface, 
						boost::shared_ptr<IBackupParam>		pParamInterface );

#endif //__DVRBACKUP_HPP__
