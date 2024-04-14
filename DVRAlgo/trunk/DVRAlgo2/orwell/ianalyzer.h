//! \file IAnalyzer.h
//! \brief ���������� ����������
//! 
//! ������ � 2002 ����, �. ��������, �. �������.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IANALYZER_H__B496D355_BB7D_4DB4_BD1D_618E285EE8E3__INCLUDED_)
#define AFX_IANALYZER_H__B496D355_BB7D_4DB4_BD1D_618E285EE8E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// declare in DLL
// #define SRSDLL_API __declspec(dllexport)
//
//  
// #define SRSDLL_API __declspec(dllimport)

#ifndef SRSDLL_API
#define SRSDLL_API
#endif

//////////////////////////////////////////////////////////////////////
// Forward declare
//////////////////////////////////////////////////////////////////////

namespace Elvees {
namespace Win32 {

	class CImage;
	class CPTZControl;
}
}

class CDataWare;

//////////////////////////////////////////////////////////////////////
// Common structures
//////////////////////////////////////////////////////////////////////

//! \brief ���������� �� ������ ���������� � �� ���������
struct AlgoParamsInfo
{
	long Number;		//!< ����� ���������� � ����������
	char **Names;		//!< ������ ���������� �� ������ � ��������� ����������
	double *Values;		//!< ������ �������� ����������
};

/*! \brief ��������� � ����������� �� �������.
*
* ������������ ���� ������� CIAnalyzer::GetObjects, ���� CISlaveManager::GetNextPhoto.
* �������� ���������� �� ��������/�������
*/
class ObjectReg{
public:
	ObjectReg();
	virtual ~ObjectReg();

	/*! \brief ������� ��������� ObjectReg
	*
	* ���� ����� ���������� ������ ����� ��������� ��������� �� CISlaveManager::GetNextPhoto.
	* ����� ������� ������, ���������� �� CIAnalyzer::GetObjects, ���������� ��������
	* CIAnalyzer::DeleteObjects
	*/
	virtual long _Release();

	/*! \brief ����� �������
	*
	* �������� ��� ObjectReg, ���������� �� CISlaveManager::GetNextPhoto
	*/
	int masterID;

	/*! \brief ����� �������
	*
	* �������� ��� ������� ����������� �������
	*/
	int UID;
	
	//! \brief ���������� ���������� �������
	int XApp,YApp;
	
	/*! \brief ���������� �������
	*
	* ������������ �� CISlaveManager::GetNextPhoto. ����� ObjectReg::_Release ��������
	* img->_Release().
	*/
	Elvees::Win32::CImage *img;

	/*! \brief ������ ������ �������
	*
	* ������ ����� � ������� ������������� �������/������/������ �����/...
	*/
	int Class;
	//! \brief ������ ����� �������
	//! ��� ������ ����� �������� �� ������� CIAnalyzer::GetShortName �
	//! CIAnalyzer::GetFullName
	int Color;
	//! \brief ������� ���� ������� R, G, B
	BYTE AvgCol[3];

	//! \brief ������� �������� ������� � �������� � �������
	double Avgspeed;
	
	//! \brief ���������� ������������ �������
	int XDApp,YDApp;
#pragma warning(disable:4201)
	//! \brief ���������� ����� �������
	union {
		RECT rect;
		struct {int xleft,yup,xright,ydown;};
	};
#pragma warning(default:4201)
	/*! \brief ������ �������
	*
	* status=0 - ������ ��� "�� ����������" <br>
	* status=1 - ������ � ������ ��� �������� <br>
	* status=2 - ������ �� ������ <br>
	* status=3 - ������ �������� ����� <br>
	* status=4 - ������ ����� �������� <br>
	* status=5 - ���������� �� ������ �������� �� ��, ��� ������ � ����� (����� �����������) <br>
	* status=6 - ���������� ������ �������� �� ��, ��� ������ � ����� <br>
	* �������� status � 0 �� 4 ������ ���������� �������, ��������� - �����.
	*/
	long status;
	/*! \brief ���� "�����"
	*
	* ���� ����� true, �� ������ - "�����"
	*/
	bool holeFlag;

	/*! \brief ���� "���������������" �������
	*
	* ���� ���� ����� true, �� ������ �� ������ �������� �������� "��������������" ��
	* "��������". ����� ������� ����� �������� �� ������������ � �� �������� �� ���
	* �������.
	*/
	bool suspeciousObject;

	/*! \brief ���� ������������ ��������
	*
	* ���� ����� true, �� ������ �������� �����������
	*/
	bool leftObjectFlag;

	/*! \brief ���� ��������������� �������
	*
	* ���� ����� true, �� ������ �����������
	*/
	bool stoppedObjectFlag;
	
	/*! \brief ���� �������, ���������� ��� ��������
	*
	* ���� ����� true, �� ������ ������ ��� �������� � �������� ������������
	* ��� ���������������� �������.
	*/
	bool followFlag;
	
	/*! \brief ����� ����������� ������� �� ����� � �������������*/
	DWORD ExistingTime;

	/*! \brief ����� ����������� ������� �� ����� � ������*/
    long ExistingFrame;

	/*! \brief �������� ����� ������� � ���� ������
	*
	* dbID ������������ ��� �������� ���� ��� �������� � ���� ������.
	* dbID ������ ��������������� ����� ��� ��������� status=1 ��� ������
	* ������� CIAnalyzer::SetDatabaseID, � ��������� ������ ��� �������
	* � ������ UID �� ������� ������� ����� masterID � ���� dbID �����
	* ������������ -1.
	*
	* �������� ���� dbID ��� ������� UID ����� ����������. ��� ������
	* ����, ��� ������ ������ ������ � �������� �� ������ ������, �� ����
	* �����������, ��� � ���� ����������� ���� ����� ��������� ���� � ���
	* �� ������. ������������ �������� dbID �������� �������� ���������
	* ��� ���������� �������. �� ����� ��������� ������� ���� �����
	* ���������� ����� ����� ���������.
	*/
	long dbID;

	/*! \brief "��������" ������
	*
	* �������� ������������ �������� ����������� �������. ��� ������ ��������, ��� ���� ��������.
	*/
	long photoQuality;
	//__int64 objectID;
};

//////////////////////////////////////////////////////////////////////
//! \brief ��������� ����������� ������-������ CIAnalyzer
//!
//! ��������� ����� �������� �� ������� GetAnalyzerInterface
//////////////////////////////////////////////////////////////////////

class CIAnalyzer  
{
public:
	/*! \brief �������� ������.
	*
	* ������� ������ �� ��������������.
	*/
	virtual long _Release()=0;
	
	//! \brief �������������.
	virtual BOOL Init(const char *iniFile, CDataWare *dw, long ID)=0; //!< ������������� ��������
	//!< \param iniFile - ���� ��������, \param dw - ��������� �� DataWare, \param ID - ���������� ����� ������
	//!< \return ���������� FALSE ��� ������.
	
	/** @name ������� ������ � �����������
	*  ��� ������� ���������� 0, ���� OK.
	*/
	//@{	
	virtual long GetParamsInfo(AlgoParamsInfo *api)=0;
	//!< \brief �������� ���������� � ���������� � �� ���������.
	//!< \param api - ���������� � ����������, ���������� � ��������� AlgoParamsInfo
	//!< \sa struct AlgoParamsInfo
	virtual long DeleteParamsInfo(AlgoParamsInfo *api)=0;
	//!< ������� ���������� ������
	virtual long SetParams(double *source)=0;
	//!< \brief ���������� ���������
	//!< \param source - ������ ����������
	virtual long SetParams(AlgoParamsInfo *api)=0;
	//!< ���������� ���������
	//@}
	
	
	/** @name ������� ��������
	*  ������ �� ��������� �� �������� � �����������������.
	*/
	//@{
	virtual void FollowObjectOff()=0; //!< \brief ���������� ��������
	virtual void FollowObject(long x, long y)=0; //!< \brief �������� �� ��������� ��������
	virtual bool FollowByID(long ID)=0;	//!< \brief �������� �� �������� c ��������� ID
	//!< \return TRUE, ���� �������� �������.
	virtual long GoToXY(long x, long y, long slaveID=-1)=0;
	//!< \brief ����������� �����
	//!<
	//!< ����������� ����� slaveID � ���������� x,y; 
	//!< ���� slaveID=-1, �� ���������� ������������.
	//@}
	
	/** @name �������� �������
	*  ����� ������������ � ��������� �����������.
	*  ���������� 0, ���� OK.
	*/
	//@{	
	/*! \brief ��������� �����
	*
	* ���������� ��������������� ��� ������� ����� �����������.
	* \param newFrame - ���� ������ � ������� CImage.
	* \return 0, ���� ��� ������
	*/
	virtual int HandleBmp(Elvees::Win32::CImage *newFrame)=0; 
	/*! \brief ��������� ������� ��������.
	* �������� ������ ��������
	* \param obj - ��������� �� ������ ��������
	* \param n - ����� �������� (����� ���� 0)
	* \return ����� ��������
	*/
	virtual int GetObjects(ObjectReg **obj, long *n)=0; 
	/*! \brief �������� ������� ��������
	* \param obj - ��������� �� ������ ��������
	* \return 0, ���� ��� ������
	*/
	virtual int DeleteObjects(ObjectReg* obj)=0; 
	//@}
	
	//! \brief �����
	virtual void DeleteStatistic()=0;
	//! \brief �������� �������������� ������ ���
	//! \return HBITMAP �������� � �����
	virtual HBITMAP GetBackground()=0;
	//! \brief ���������� hdc ��� ������ ��������� ����������
	//! ����� �� �������� ���������� ��������, ������� ������� hdc=0. �� ��������� hdc=0.
	virtual void SetHDC(HDC hdc)=0;

	/*! \brief ��� ���������� ��������.
	* \param printBG - ����� �������� �� 1 �� 3. ���� -1, �� �������� ������������� �� ��������� ���. ���� 0,
	* ��� �� ����������.
	* \return TRUE, ���� ��� ������ ���������, FALSE, ���� ���
	*/
	virtual BOOL SwitchPrintBG(long printBG=-1)=0;
	
	/** @name ������� �������� ������� ��������
	* ���������� �������� � ������� ��� ������ id ��� ������������ ������� � ������� system. 
	* (�� ������ ������) system=0 ���������� ��� ������� system=1 ��� ����.
	* ����������� ���������� ������ �� ����. ����� ����� ��������� �� ������ ��������� � ��������
	* ����� CIAnalyzer. ��� ������������ ���������� ���������� NULL;
	*/
	//@{
	virtual char* GetShortName(long system,long id)=0;
	virtual char* GetFullName(long system,long id)=0;
	//@}

	/*!\brief ��������� ����������� ������ � ���� ������� adb. 
	*
	* ��� adb-����� ����������� � ����� ObjecMapFileName ������ [main] ini-�����.
	*/
	virtual long SaveFollowedObjectToMap()=0;

	/*! \brief ���������� ������������� ���� ������.
	*
	* ������������� dbID ��� ������� ��� ������� analyzerID ��� ��������� status=1
	* \param analyzerID - ���� UID ��������� ObjectReg
	* \param dbID - ���������� �������� �������� �������
	* \return 0, ���� ��� � �������, 1, ���� ������� analyzerID �� ����������.
	*/
	virtual long SetDatabaseID(long analyzerID,long dbID)=0;

	//! \brief �������� ���������� ������������ ������� ��� ������ ���������� � ������ ����������
	virtual long GetNumOfInfoPages()=0;
	//! \brief  �������� ��������� �������������� �������� � ������� index
	virtual char* GetPageTitle(long index)=0;
	//! \brief �������� ����� ������� � ������� ��������� ���������� ��� ������������ �������� � ������� index
	virtual char* GetInfoPage(long index)=0;

	//! \brief �������� ����������� ����� ������ ���������� �� �������. 
	//! \return 0, ���� ��� ��������� ���������; >0, ���� ���� ���������, ������� �� ���������
    virtual long UpdateParams()=0;

	//! \brief �������� ���� ��������� (���� ��� � �������, �� ���� ����� 0)
	virtual long GetStateFlags()=0;
	//! \brief �������� ������ � ��������� ������ �� �����, ���� �� �� ����.
	virtual char* GetStateString(long flags)=0;

	//! \brief  �������� ������ ��� ��������� �������. ��� ������ ���������� NULL
	virtual char* GetObjectInfo(long obj_id)=0;
	//! \brief  ������� ������, ���������� �������� GetObjectInfo.
	virtual long FreeObjectInfo(char* data)=0;

  virtual bool GetLatestMotionLabels( unsigned char *& pImage, __int32 & width, __int32 & height );
};

//////////////////////////////////////////////////////////////////////
//! \brief ��������� ����������� �����-����� CISlaveManager
//!
//! ��������� ����� �������� ��� ������ ������� GetSlavManagerInteface
//////////////////////////////////////////////////////////////////////
class CISlaveManager  
{
public:
	virtual long _Release()=0;

	virtual long Init(Elvees::Win32::CImage * is, CDataWare *dw,long clusterID, long _SlaveID, const char* iniFile=NULL)=0;
	//!< � clusterID ����� ������� id ��������, � �������� ��������� ������ �����, � iniFile �������� ������� Connection String
	//!< � ���� ������. ���� �� �������, �� ����� ���� �� �������
	//!< ����� - 0

	virtual long GetNextPhoto(ObjectReg **obj)=0;
	//!< \brief �������� �����.������������ ��������� �� �������� Commutator. 
	//!< 
	//!< ��� ������ ������������� ��������� ������ ������� ���������, ����� ������� 
	//!< �������� ���������� ������� � ��������� �����, ������� ���������� ���������� (��� ���� 
	//!< ���������� ������������ � ��������� ObjectReg � 5 ��� 6 �������� - ��� � CIAnalyzer::GetObjects). 
	//!< ��������� �������� ��������, ����� ������� �� ����������� �� ��������� � �������
	//!< ������������� ������� �������, �� GetNextPhoto ����� ���������� � CDataWire::WaitForMessage
	//!< � ������� ������������� ������� �������. ����� �������� ��� ��������, ������
	//!< SendQuitMessage �� ������������� ������ ��� �� ��������� GUI-��������� (��. ���������� � 
	//!< CCommutator::MainLoop). 
	//!< \return
	//!<	<ul>
	//!<		<li>	0, ���� ���������� �������� �����,
	//!<		<li>	1, ���� ��������� ����� �� SendQuitMessage, 
	//!<		<li>	2, ���� ��������� ��������� �������, � ������� ������� Slave
	//!<		<li>	3, ���� CallPreset ����� ������ (obj->status=��� ������)
	//!<		<li>	4, ���� �� ����� ����������� � ������������ 
	//!<		<li>	5, ���� ������� ��� ���� ����������, ����������� ������ �������. ���������� ������ ���������� 
	//!<			�� ������ � ���������� � �����.
	//!<		<li>	6, ������� � ������ ����������
	//!<		<li>	7, ����� ���������������� �� ����������
	//!<		<li>	-1, ���� �������� � ������� DataWire
	//!<		<li>	-2, ���� ������ � ������ ����������� (�� ������ ��������� ������)
	//!<			������������� �������� ��� ������ - �����������, 
	//!<			����� ������������� ����� ���������� ������
	//!<	</ul>
	
	virtual long SendQuitMessage()=0;
	//!< ����� ���������� ������ CCommutator::SendQuitMessage

	virtual long GetNumOfInfoPages()=0;
	//!< �������� ���������� ������������ ������� ��� ������ ���������� � ������ ����������
	virtual char* GetPageTitle(long index)=0;
	//!< �������� ��������� �������������� �������� � ������� index
	virtual char* GetInfoPage(long index)=0;
	//!< �������� ����� ������� � ������� ��������� ���������� ��� ������������ �������� � ������� index

	virtual long GetStateFlags()=0;
	//!< �������� ���� ��������� (���� ��� � �������, �� ���� ����� 0)
	virtual char* GetStateString(long flags)=0;
	//!< �������� ������ � ��������� ������ �� �����, ���� �� �� ����.
	
};

//////////////////////////////////////////////////////////////////////
//! \brief ��������� CICommutator
//!
//! ��������� ����� �������� ��� ������ ������� GetCommutatorInteface
//////////////////////////////////////////////////////////////////////
class CICommutator  
{
public:
	//! \brief �������� ����������
	virtual long _Release()=0;
	
	virtual long Init(char *iniFile, CDataWare *_dw, long id)=0;//!< �������������
	//!<\param iniFile - ���� � ��/ini-����� (���� NULL, �� ���� � �� ������ �� �������)
	//!<\param _dw - ��������� �� _dw
	//!<\param id - ����� �������� � �� (��� ��������� � ���������� ����� CIDBBridge)
	//!<\return <ul> <li>0, ���� ��� � �������
	//!<			<li>-1, ���� �� ���� �������� ��������� ini-����
	//!<			<li>-2, ���� _dw=NULL
	//!<			<li>-3, ���� ����������� ������� ������ Init �������� ����� ��������� �������
	//!<			<li>-4, ���� �� ������� ������� ��������� CAlgoPTZManager
	//!<			<li>-5, ���� �� ������� ������������������� ��������� CAlgoPTZManager
	//!<		</ul>
	virtual long SendQuitMessage()=0;
	//!< ������� ��������� �� ����� �� MainLoop. ������ ����� ������ 
	//!< ��������� ��������� ������������� ������� � �������� ��� ��� ������ CDataWire "������ 
	//!< ����".
	
	virtual void MainLoop()=0;
	//!< ������ ����� ������������ ���� ��������� ��������� �� ���� ��������� 
	//!< ��������. �� �� ������ ����������, ���� �� ����� ������ ����� SendQuitMessage ���� �� ������� 
	//!< ������, ���� ��� ��������� GUI-��������� � ��� ������, ���� ����� CDataWire::WaitForMessage 
	//!< ������������ ����� ���� ��������� (�������� ����� MainLoop ����� ��������� � �������� 
	//!< ��������� � ������ CDataWire::WaitForMessage).
	virtual long SlaveCameraSwitch(long slaveID, long state)=0;	
	//!< \brief ������������ �������/��������������� ������ ������ ������
	//!< \param slaveID - ����� ������ (-1 ��� ���� �����)
	//!< \param state - <ul> <li>=0 - �������� ������ ������� CIAnalyzer::GotoXY
	//!<					<li>=1 - �������� ����� ��������� �������� �� Slave-������</ul>

	virtual long GetNumOfInfoPages()=0;
	//!< �������� ���������� ������������ ������� ��� ������ ���������� � ������ ����������
	//!< ����� �������� �� ���� ������ �����������
	virtual char* GetPageTitle(long index)=0;
	//!< �������� ��������� �������������� �������� � ������� index
	virtual char* GetInfoPage(long index)=0;
	//!< �������� ����� ������� � ������� ��������� ���������� ��� ������������ �������� � ������� index

	virtual long GetStateFlags()=0;
	//!< �������� ���� ��������� (���� ��� � �������, �� ���� ����� 0)
	virtual char* GetStateString(long flags)=0;
	//!< �������� ������ � ��������� ������ �� �����, ���� �� �� ����.
	
};

//////////////////////////////////////////////////////////////////////
//! \brief ��������� CIFilters
//!
//! ��� ������ ���������� 0 ��� ���������� ������, ������������� ����� ��� ����������� �������,
//! ������������� - ��� �������������
//////////////////////////////////////////////////////////////////////
class CIFilters {
public:
	//! \brief �������� ����������
	virtual long _Release()=0;

	virtual	long Reflection(PBITMAPINFOHEADER pbih, LPBYTE bitsIn, LPBYTE bitsOut)=0;
	//!< �������� ���������������� �������� �� bitsIn � bitsOut. �������� ��� ������� �� ������������
	//!< � ������ ��� ������� ����������.
	
	//! @name ������������
	//@{
	virtual long SetStabParams(long windowX,long windowY,long windowSize, long maxDisp)=0;
	//!< ���������� ���������� ���� � ����� ������ ����� (windowX,windowY) � �������� windowSize,
	//!< � ����� ����������� ���������� �������� maxDisp (��� ���������� �������� ��������
	//!< ������ ������������)
	virtual long Stabilize(PBITMAPINFOHEADER pbih,LPBYTE bits)=0;
	//!< ������������ ������ (�������� ��� ������� ������ �����)
	//!< ���� � ����� - � bits
	//@}

	//! @name ������ ����������� ��� � ����
	//!@{
	virtual long SetDayLightThresholds(BYTE thresh, double procent, int frameInert)=0;
	//!< ���������� ������ thresh � procent, ���������� ���������� �������� DayNightLearn,
	//!< � ����� ������� frameInert - ���������� ������ ������, ����������� ��� ������������
	//!< ��������� � ���������������.
	virtual long GetDayLightStatus(PBITMAPINFOHEADER pbih,LPBYTE bits,long * status, char* logFile=NULL)=0;
	//!< ����������, ���� ������ ��� ���� ��� �������� pbih, bits.
	//!< � status ������������ -1, ���� ��������� �������������� (�� ������ ����� SetDayLightThresholds),
	//!< 0 - ���� ����, 1 - ���� ����. ���� ��������� ������ ��� ����� logFile, �� ��� �������������
	//!< ��������� � ��� ����� ���������� ���� �� �������� ������������.
	//!@}
	
	//! ������ ����������
	virtual long TimeSmoothing(PBITMAPINFOHEADER pbih,LPBYTE bits,long coef)=0;
	//!< ���� � ����� - � bits.
	//!< coef ���������� �� 0 �� 100 � ���������� ������� ����������
	virtual long LocalBinarization(PBITMAPINFOHEADER pbih,LPBYTE bitsIn, LPBYTE bitsOut, long full=0)=0;
	//!< \brief ��������� �����������
	virtual long CheckFocus(PBITMAPINFOHEADER pbih,LPBYTE bits,long negative,long *result)=0;
	//!< \brief ���������� ���� ������
	virtual long AutoLevel(BYTE *bits, PBITMAPINFOHEADER pbih, BOOL onlyLuminocity=FALSE)=0;
	//!< \brief ����������� ��������� �������/�������������
	virtual long SmartBlur(BYTE *bitsIn, PBITMAPINFOHEADER pbih, BYTE *bitsOut, long r, long Tr)=0;
	//!< \brief "�����" �����������
	
	virtual long RemoveClouds(PBYTE bits, PBITMAPINFOHEADER pbih)=0;
	//!< \brief ������ �������

	virtual long BoundsRoberts(PBYTE bits, PBITMAPINFOHEADER pbih)=0;
	//!< \brief ��������� ������
	virtual long FocusChecker(PBYTE bits, PBITMAPINFOHEADER pbih, long flags)=0;
	//!< \brief ���������� ���� ������

};


//////////////////////////////////////////////////////////////////////
// Function
//////////////////////////////////////////////////////////////////////

//! \brief �������� ��������� ����������� �������
SRSDLL_API long __cdecl GetAnalyzerInterface(CIAnalyzer **ani);
//! \brief �������� ��������� ����������
SRSDLL_API long __cdecl GetCommutatorInterface(CICommutator **coi);
//! \brief �������� ��������� ����������� ������
SRSDLL_API long __cdecl GetSlaveManagerInterface(CISlaveManager **smi);
//! \brief �������� ��������� 
SRSDLL_API long __cdecl GetFiltersInterface(CIFilters **filti);
//! \brief ���������� � ���� ������
//! \param xmlFile - xml-���� � ����������� � ������� UNICODE
SRSDLL_API long __cdecl SetDefaultSettings(LPCSTR xmlFile);

/*! \brief �������� ������ �������� ObjectReg
\param arr - ��������� �� �������������� ������
\param nSize - ����� ��������� � ���������� �������
\return 0, ���� ��� ������
*/
SRSDLL_API long __cdecl GetObjectRegArray(ObjectReg ** arr, int nSize);

//! \brief ������� ������ �������� ObjectReg, ���������� �������� GetObjectArray
SRSDLL_API long __cdecl FreeObjectArray( ObjectReg * arr);

#define GAIE_STATUS0	1
SRSDLL_API long __cdecl GetAnalyzerInterfaceExt(CIAnalyzer **ani, DWORD options=0);
//!< \brief �������� ��������� ������ CAnalyzer, � ���. �������.
//!< \param ani - ��������� �� ����������
//!< \param options - ���� 0, �� �������� �����, ��� �
//!< GetAnalyzerInterface; ���� ������ ���� GAIE_STATUS0, �� ���� ��������� � ��������� ������� 0 (��� SuperVisor).


#endif // !defined(AFX_IANALYZER_H__B496D355_BB7D_4DB4_BD1D_618E285EE8E3__INCLUDED_)
