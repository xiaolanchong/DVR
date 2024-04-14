//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ���������� � �������� ���������� ���������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   26.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _INSTALL_HELPER_2842084057089803_
#define _INSTALL_HELPER_2842084057089803_

/* ************************************************************************************

������� ������� � ��������� ��������� � ���������������� ������� ��� InstallShield
��������� UID ��� ���� ������� - 
������������� UID � ����� 
1)"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"	( ������ UuidToString )
2)"{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}"	( StringFromCLSID )

************************************************************************************* */

//! ����� ������
const DWORD Res_Error			= 0;
//! �����
const DWORD	Res_Success			= 1;
//! ������ ������� � ������ �����
const DWORD Res_ServerLaunched	= 2;

extern "C"
{

bool	WINAPI	SetBridgeForConfigure( bool UseBridge = true );

//! ������� � ������������ �������������� ������
//! \param ServerIPAddress	ip �����, ����������� ��� ������� (� ��� ������, ���� �� �����)	
//! \param szArchivePath	�������� ���� � ������	
//! \return -Res_Error-������, 
//!         -Res_Success - �����, 
//!         -Res_ServerLaunched - ������� ������(������)
DWORD	WINAPI Register( const char* ServerIPAddress, const char* szArchivePath);

//! ������� �� ������������ �������������� ������
//! \return -Res_Error-������, 
//!         -Res_Success - �����, 
//!         -Res_ServerLaunched - ������� ������(������)
DWORD	WINAPI Unregister();

//! �������� ��������� ����� ������ (����� ����������� �����)
//! \param pAvailableStreams		������ �������
//! \param pAlreadyInstalledStreams ������ ��� ������������� �������
//! \return -Res_Error-������, 
//!         -Res_Success - �����, 
//!         -Res_ServerLaunched - ������� ������(������)
DWORD	WINAPI GetAvailableStreams( CAUUID* pAvailableStreams/*, CAUUID* pAlreadyInstalledStreams*/ );

//! �������� ����� � �������, ���� ��� ��������, �� ������ ���
//! \param szStreamUID �������������, ���������� GetAvailableStreams
//! \return -Res_Error-������, 
//!         -Res_Success - �����, 
//!         -Res_ServerLaunched - ������� ������(������)
DWORD	WINAPI AddStream( const char* szStreamUID);

//! ������� ����� �� �������, ���� ��� ������, �� ������ ���
//! \param szStreamUID �������������, ���������� GetAvailableStreams
//! \return -Res_Error-������, 
//!         -Res_Success - �����, 
//!         -Res_ServerLaunched - ������� ������(������)
DWORD	WINAPI RemoveStream( const char* szStreamUID );

//! ���������� ���������� � ����
//! \param hWndForStreamRender ���� ��� ����������� ������
//! \param szStreamUID	�����
//! \return -Res_Error-������, 
//!         -Res_Success - �����, 
//!         -Res_ServerLaunched - ������� ������(������)
DWORD	WINAPI ShowStream( HWND hWndForStreamRender, const char* szStreamUID );

//! ��������� �������� ���� � ������, ��� �������� �����,
//! �������� � ����� ������ ����� Register � Unregister
//! ��������� ������������ �� ���������� ������ GetArchivePath
//! \return 0, "" - ������, ����� ������ ����
const char*		WINAPI GetArchivePath();

//! ���������� ����� � �������
//! \param szArchivePath ������ ����, ������ ��� NULL - �������� ����
//! \return -Res_Error-������, 
//!         -Res_Success - �����, 
//!         -Res_ServerLaunched - ������� ������(������)
DWORD			WINAPI SetArchivePath(const char* szArchivePath);

//! ���������� ��� �������
void	WINAPI FreeMemory();

/*!	Install actions
	\code

	Register("192.168.0.1", "e:\\DVR");
	//...
	GetAvailableStreams( pAvailableStreams );
	//...
	while( ... )
	{
		ShowStream( hWnd, szStream );
	}
	//..
	//! ��� ���� ��������� ������������� �������
	AddStream( szStream )

	//
	FreeMemory();

	\endcode
*/

/*! Uninstall actions
\code

	szArchivePath = GetArchivePath();
	// delete archive directory
	Unregister()

\endcode
*/
}

#endif // _INSTALL_HELPER_2842084057089803_