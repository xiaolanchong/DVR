//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: XML document IVideoSystemConfigEx implementation

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   16.09.2006
*/                                                                                      //
//======================================================================================//
#ifndef _XML_VIDEO_SYSTEM_CONFIG_3905705851014638_
#define _XML_VIDEO_SYSTEM_CONFIG_3905705851014638_

//using namespace CHCS;
#include "../../IVideoSystemConfig.h"
//======================================================================================//
//                              class XmlVideoSystemConfig                              //
//======================================================================================//

//! \brief XML document IVideoSystemConfigEx implementation
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   16.09.2006
//! \version 1.0
//! \bug 
//! \todo 

class XmlVideoSystemConfig : public CHCS::IVideoSystemConfigEx
{
	class CoInit 
	{
		HRESULT hr;
	public:
		CoInit(): hr( CoInitialize(0) )
		{
		}
		~CoInit()
		{
			if( S_OK == hr )
			{
				CoUninitialize();
			}
		}
	} _coinit;

	CComPtr<IXMLDOMDocument> m_pDocument;
	std::tstring			 m_sFileName;
public:
	XmlVideoSystemConfig( LPCTSTR szFileName );
	virtual ~XmlVideoSystemConfig();

private:
	virtual void		 GetLocalDeviceSettings(std::vector<IVideoSystemConfig::DeviceSettings>& params );
	virtual void		 GetCameraSettings(std::vector<IVideoSystemConfig::CameraSettings>& params);
	virtual std::tstring GetArchivePath();
	virtual void		 GetCodecSettings(IVideoSystemConfig::CodecSettings& params);
private:
	virtual void	DeleteAllDevicesAndCameras() ;
	virtual void	CreateDevice( const IVideoSystemConfig::DeviceSettings& ds ); 
	virtual void	DeleteCamera( LONG CameraID );
	virtual LONG	CreateCamera( const UUID& DeviceUID, unsigned int PinNumber );

	virtual void		 SetArchivePath(const std::tstring& sArchivePath)		;
	virtual void		 SetCodecSettings(const IVideoSystemConfig::CodecSettings& params) ;

private:
	LONG				GetNewCameraID();
	void			SaveXmlFile();
//	void			RenumberCameras();

	LONG			m_NewCameraID;
};

#endif // _XML_VIDEO_SYSTEM_CONFIG_3905705851014638_