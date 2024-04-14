//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���������� ���������� IDBBackup ��� DVRBackup

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   06.02.2006
*/                                                                                      //
//======================================================================================//
#ifndef _D_B_BACKUP_IMPL_5044519115472496_
#define _D_B_BACKUP_IMPL_5044519115472496_

#include "../Interface/IDBBackup.h"

//======================================================================================//
//                                  class DBBackupImpl                                  //
//======================================================================================//

//! \brief ���������� ���������� IDBBackup ��� DVRBackup
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.02.2006
//! \version 1.0
//! \bug 
//! \todo 

class DBBackupImpl : public DBBridge::IDBBackup
{
public:
	DBBackupImpl();
	virtual ~DBBackupImpl();

	//!	�������� ���� � �������� ����� ������
	//!	\return ����
	virtual std::string				GetArchivePath() ;
	//!	�������� ��������� �������� �� ���������� ����� �� �����
	//! ��������� - 2 �����: 0.0f <= A <= B <= 100.0f
	//!	\return ���� ������/�����
	virtual std::pair<float, float>	GetDiskSpaceParameters() ;

	//!	�������� ��������� �������� �� �������
	//!	\return 
	virtual size_t					GetStorePeriod() ;

	//!	������� ��� ������ �� �������
	//!	\param nTime �����, �� �������� ���� ������� (���������)
	virtual void					DeleteBeforeTime( time_t nTime ) ;
};

#endif // _D_B_BACKUP_IMPL_5044519115472496_