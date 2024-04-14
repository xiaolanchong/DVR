//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Interface for getting backup parameters 

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   05.02.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_D_B_BACKUP_1867581314286850_
#define _I_D_B_BACKUP_1867581314286850_

#include "../../../shared/Common/ExceptionTemplate.h"

//======================================================================================//
//                                   class IBackupParam                                 //
//======================================================================================//

//! \brief Interface for getting backup parameters 
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   05.02.2006
//! \version 1.0
//! \bug 
//! \todo 

class IBackupParam
{
public:
	MACRO_EXCEPTION( BackupParamException, std::runtime_error );

	//!	�������� ���� � �������� ����� ������
	//!	\return �������� �������� ����� ������
	//! \exception BackupParamException
	virtual std::string				GetArchivePath() = 0;

	//!	�������� ��������� �������� �� ���������� ����� �� �����
	//!	\return ����������� ���-�� ���������� ����� �� �������
	//! \exception BackupParamException
	virtual double	GetPercentageFreeDiskSpace () = 0;

	//!	�������� ��������� �������� �� �������
	//!	\return ���-�� ���� �������� ������, ��� ��������� ���� �������� 
	//!         ���� ������� �� ������� ����
	//! \exception BackupParamException
	virtual unsigned int					GetStorePeriod() = 0;
	
	//!	������� ��� ������ �� �������
	//!	\param nTime �����, �� �������� ���� ������� (���������)
	//! \exception BackupParamException
	virtual void					DeleteBeforeTime( time_t nTime ) = 0;

	virtual ~IBackupParam() {};
};

#endif // _I_D_B_BACKUP_1867581314286850_