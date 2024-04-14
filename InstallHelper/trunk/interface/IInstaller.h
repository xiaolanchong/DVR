//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ��������� ��� ��������, ����������� ��� ��������� � ����������� DVR

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   26.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _I_INSTALLER_2000773579556309_
#define _I_INSTALLER_2000773579556309_

//======================================================================================//
//                                   class IInstaller                                   //
//======================================================================================//

//! \brief ��������� ��� ��������, ����������� ��� ��������� � ����������� DVR
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   26.05.2006
//! \version 1.0
//! \bug 
//! \todo GetAvalableStreams � RemoveStream ��� ��������� ������������

class IInstaller
{
public:
	virtual ~IInstaller() {};

	virtual bool SetBridgeForConfigure(bool UseBridge) = 0;

	//! ������ ������ ������������
	//! \param ServerIPAddress ip �����, ��� ������� �������� ������
	//! \return ��������� - �����/�������
	virtual bool Register(const char* ServerIPAddress)				= 0;

	//! ������ ������ � �������
	//! \return  ��������� - �����/�������
	virtual bool Unregister()			= 0;

	//! �������� ���������� �� ���� �����������
	//! \param pAvailableStreams	��������� ������
	//! \param pInslalledStreams	��� �������������, ���� �� ��������
	//! \return ��������� - �����/�������
	virtual bool GetAvalableStreams( CAUUID* pAvailableStreams, CAUUID* pInslalledStreams )	= 0; 

	//! �������� ����� � ������� DVR
	//! \param pStreamUID ���� �� UUID, ���������� � GetAvalableStreams
	//! \return ��������� - �����/�������
	virtual bool AddStream		( const UUID* pStreamUID ) = 0;

	//! ������� ����� �� ������� DVR
	//! \param pStreamUID ���� �� UUID, ���������� � GetAvalableStreams
	//! \return ��������� - �����/�������
	virtual bool RemoveStream	( const UUID* pStreamUID ) = 0;

	//! ���������� ����� ������ � ����
	//! \param pStreamUID	 ���� �� UUID, ���������� � GetAvalableStreams
	//! \param hWndForRender ���� ��� �������������� �����������
	//! \return ��������� - �����/�������
	virtual bool ShowStream		( const UUID* pStreamUID, HWND hWndForRender ) = 0;

	//! �������� ���� � ����� ������ (��� ��������)
	//! \return �������� ���� � ����� ������
	virtual std::string	 GetArchivePath() = 0;

	//! ���������� ���� � ����� ������
	//! \param sArchivePath ���� � ����� ����������� � ��
	//! \return ��������� - �����/�������
	virtual bool		 SetArchivePath( const std::string& sArchivePath ) = 0;
};

#endif // _I_INSTALLER_2000773579556309_