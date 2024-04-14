//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ������� ����� ���������� ���������� IStreamManager ��� �������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.06.2006
*/                                                                                      //
//======================================================================================//
#ifndef _SERVER_MANAGER_3338654774616518_
#define _SERVER_MANAGER_3338654774616518_

#include "BaseManager.h"

class IOutputFramePin;
class IntermediateFilter;
class CaptureServer;
class RemoteVideoProvider;
class RemoteArchiveProvider;

//! \brief ����������, ������� �� �� ������ ���������� ��������� ���������� 
//!		   (���� ��� ������� ��������������)
//! \version 1.0
//! \date 06-30-2006
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class ServerRunSentry
{
	//! ������� ��� ��������
	HANDLE	m_hDeviceOwner;
	//! ������ ��� ��������, ���� ����
	DWORD	m_dwLastError;
public:

	ServerRunSentry() :
	  m_hDeviceOwner ( NULL )
	{
		m_hDeviceOwner	= ::CreateEvent(NULL, TRUE, FALSE, TEXT("Global\\CHCSDeviceOwner"));
		m_dwLastError	= GetLastError() ;
	}

	~ServerRunSentry()
	{
		if(m_hDeviceOwner)
			::CloseHandle(m_hDeviceOwner);
	}
	bool	IsAlreadyRunning() const
	{
		return !(m_hDeviceOwner && GetLastError() != ERROR_ALREADY_EXISTS);
	}
};

//======================================================================================//
//                                 class ServerManager                                  //
//======================================================================================//

//! \brief ������� ����� ���������� ���������� IStreamManager ��� �������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   30.06.2006
//! \version 1.0
//! \bug 
//! \todo 

class ServerManager : public BaseManager
{
public:
	ServerManager(boost::shared_ptr<SystemSettings> pVideoConfig, DWORD WorkingMode);
	virtual ~ServerManager();

private:
	long    GetVideoRate();
	long    GetVideoMaxSize();
	
	bool LoadSettings();
	void SaveSettings();

	bool InitRegisteredDevices();

	//! Read Stream ID from database
	//! \param bShowDialogOnFailed  �������� ���� ��������� ��� ������
	//! \return �������/���
	bool ReadStreamsFromDB(bool bShowDialogOnFailed = false);

	// base class overriding 
	virtual void InitializeWork() ;
	virtual void UninitializeWork();


	//! ������������ ������� ��������� � ��������� ������ �����������
	//! \param pCaptureFilter	������ �������
	//! \param StreamID			������������� ������, � �������� ���� ������
	//! \param lDevicePin		����� ����� �� ����������
	//! \param &camUID			�� ����������
	void	AttachToCaptureFilter( 
		IOutputFramePin* pCaptureFilter, 
		long StreamID,
		long  lDevicePin, 
		const UUID &camUID,
		unsigned int CompressFPS
		);

	//! ������� ����������� ��� �������� �� ����
	void	CreateVideoProvider();
	//! ������� �����������
	void	DestroyVideoProvider();

	//! ������� ������ �������� ������
	void	CreateArchiveProvider();
	//! ������� ������ �������� ������
	void	DestroyArchiveProvider();
private:

	bool	IsArchiveEnabled() const
	{
		return CheckMask< CHCS_MODE_STORE >();
	}

	bool	CreateNetServer() const
	{
		return CheckMask< CHCS_MODE_NETSERVER >();
	}

	bool	CreateNetArchive() const
	{
		return CheckMask< CHCS_MODE_NETARCHIVE >();
	}

	bool	UseOnlyDatabaseStreams() const
	{
		return CheckMask< CHCS_MODE_DATABASE >();
	}

	// Archive settings

	DWORD m_CodecFCC;				//!< Codec FOURCC

	long m_lVideoFPM;				//!< Video Frame per minute
	long m_lVideoMaxSize;			//!< Video max size in seconds

	long m_lCodecQuality;			//!<
	long m_lCodecKeyRate;			//!< Forced KeyRate
	long m_lCodecDataRate;			//!< Forced DataRate
	//! ��������� ������
	std::vector<BYTE>				m_CodecStateData;

	//! ����, ��� ��������� ������� ���� ��������
	bool	m_bUpdateSettings;

	//! ����������, ������� �� ��� ������
	ServerRunSentry					m_Sentry;
	//! ������ �������
	std::auto_ptr<CaptureServer>	m_pCaptureServer;
	//! ������ ��������������� �������� �� ����
	std::auto_ptr<RemoteVideoProvider>			m_VideoProvider;
	//! ������� �����
	std::auto_ptr<RemoteArchiveProvider>		m_pArchiveProvider;

	//! ������� ������ ��� ����������, ������ � ������� ������������� �����, � �������� ��� � ��������� ������
	//! \param pCaptureFilter ��������� ��������� �����������
	//! \param StreamID		��-� ������ �����-�
	//! \param lDevicePin	����� ����� ������� �� ����������
	//! \param camUID		�����-� ����������
	//! \return ������ ��� ����������
	IntermediateFilter*			CreateServerStream(
										IOutputFramePin* pCaptureFilter, 
										long StreamID,
										long  lDevicePin, 
										const UUID &camUID
										);

	//! ������� ������ ������ ������ ������� � �������� � ������ ��������
	//! \param StreamID �� ������ �����������
	//! \param CompressFPS ��� ����������/������ ������
	//! \return ��������� �� ������
	IntermediateFilter*			CreateCompressFilter(long StreamID, unsigned int CompressFPS);
	//! ������� ������ ������ ������ � �������� � ������ ��������
	//! \param StreamID ��-� ������ �����������, � ������� ����� �������� ������, � �������� � ������ ��������
	//! \param CompressFPS ��� ����������/������ ������
	//! \return ��������� �� ������
	IntermediateFilter*			CreateArchiveFilter(long StreamID, unsigned int CompressFPS);
	//! ������� ������ ������ � ��� ������ ��� ������� � ��� �� ��������, � �������� � ������ ��������
	//! \return ��������� �� ������
	IntermediateFilter*			CreateCommonProcessFilter();

};

#endif // _SERVER_MANAGER_3338654774616518_