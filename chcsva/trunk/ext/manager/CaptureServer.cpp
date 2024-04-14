//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Сервер потоков захвата видео с плат захвата

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   29.06.2006
*/                                                                                      //
//======================================================================================//

#include "chcs.h"
#define NO_DSHOW_STRSAFE
#include <dshow.h>

#include "CaptureServer.h"
#include "../config/SystemSettings.h"
#include "../UuidUtility.h"

//======================================================================================//
//                                 class CaptureServer                                  //
//======================================================================================//

CaptureServer::CaptureServer()
{
}

CaptureServer::~CaptureServer()
{
}

void CaptureServer::GetAllDevicesInSystem( std::vector< std::pair<std::tstring, std::tstring> >& Devices ) const
{
	HRESULT hr;
	ULONG cFetched;

	IMalloc *pMalloc = NULL;
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pClassEnum = NULL;

	IMoniker *pMoniker = NULL;
	IPropertyBag *pBag = NULL;

	TCHAR stMoniker[MAX_PATH];
	TCHAR stFriendlyName[MAX_PATH];

	BSTR    szDisplayName;
	VARIANT varFriendlyName;

	do
	{
		hr = CoGetMalloc(1, &pMalloc);
		if(FAILED(hr))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to get alloc interface"));
			break;
		}

		// Create the system device enumerator
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
			IID_ICreateDevEnum, (void **)&pDevEnum);
		if(FAILED(hr))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create system device enumerator"));
			break;
		}

		// Create an enumerator for the video capture devices
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
		if(FAILED(hr))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create an enumerator for the video input devices"));
			break;
		}

		// If there are no enumerators for the requested type, then 
		// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
		if(pClassEnum == NULL)
		{
			Elvees::Output(Elvees::TError, TEXT("There are no enumerator for the video input devices"));
			break;
		}

		while(S_OK == pClassEnum->Next(1, &pMoniker, &cFetched))
		{
			if(!pMoniker)
				continue;

			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pBag);
			if(SUCCEEDED(hr) && pBag)
			{
				// Device display name
				//

				szDisplayName = NULL;
				hr = pMoniker->GetDisplayName(NULL, NULL, &szDisplayName);

				// Dont work with VfW devices
				if(SUCCEEDED(hr) && wcsncmp(szDisplayName, L"@device:cm:", 11) != 0)
				{
#ifdef _UNICODE
					lstrcpy(stMoniker, szDisplayName);
#else
					WideCharToMultiByte(CP_ACP, 0, szDisplayName, (int)wcslen(szDisplayName) + 1,
						stMoniker, sizeof(stMoniker), NULL, NULL);
#endif
					pMalloc->Free(szDisplayName);

					// Device friendly name
					//

					VariantInit(&varFriendlyName);
					hr = pBag->Read(L"FriendlyName", &varFriendlyName, NULL);

					if(SUCCEEDED(hr))
					{
#ifdef _UNICODE
						lstrcpy(stFriendlyName, varFriendlyName.bstrVal);
#else
						WideCharToMultiByte(CP_ACP, 0,
							varFriendlyName.bstrVal, (int)wcslen(varFriendlyName.bstrVal) + 1,
							stFriendlyName, sizeof(stFriendlyName), NULL, NULL);
#endif
					}
					else
					{
						lstrcpy(stFriendlyName, TEXT("Unknown"));
					}

					VariantClear(&varFriendlyName); 

					Devices.push_back( std::make_pair( stMoniker, stFriendlyName ) );
				}

				pBag->Release();
			}

			pMoniker->Release();
			pMoniker = NULL;
		}
	}
	while(0);

	if(pMalloc)
		pMalloc->Release();

	if(pDevEnum)
		pDevEnum->Release();

	if(pClassEnum)
		pClassEnum->Release();
}

bool	CaptureServer::CreateDevices( const std::vector<DeviceSettings>& DeviceParams)
{
	for ( size_t i = 0; i < DeviceParams.size(); ++i )
	{
		const DeviceSettings& DeviceParam = DeviceParams[i];
		// Create and init stream with desired PinNumber = 0 (real pin numbers are unknown)
		//
		CDeviceStream* pDevice = CDeviceStream::CreateDevice(
			DeviceParam.streamID, 
			DeviceParam.stMoniker.c_str(),
			DeviceParam.lWidth,	DeviceParam.lHeight, 
			DeviceParam.lFrmRate,
			0, 
			DeviceParam.lVStandard,
			DeviceParam.bBWMode);

		std::tstring stKeyName = UuidToString( DeviceParam.streamID );

		if(pDevice)
		{
			for(int i=0; i<pDevice->GetPinCount(); i++)
			{
				// GetPin not call AddRef.. refcount == 1
				boost::shared_ptr<CDevicePin> pStream = pDevice->GetPin(i);

				if(pStream)
				{
					LPCTSTR szKeyName = stKeyName.c_str();
					DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace,
						TEXT("Create stream {pin=%d} \"%s\" "), i, szKeyName));

					m_CaptureFilters.push_back( pStream );
				}
			}
			m_CaptureDevices.push_back( boost::shared_ptr<CDeviceStream>(pDevice) );
		}
	}
	return true;
}

bool	CaptureServer::IsDeviceAlreadyCreated( LPCTSTR stMoniker ) const
{
	for ( size_t i = 0; i < m_CaptureDevices.size(); ++i )
	{
		if( m_CaptureDevices[i]->GetMoniker() == stMoniker )
		{
			return true;
		}
	}
	return false;
}

IOutputFramePin*	CaptureServer::FindOuputPin( const UUID& StreamUID, long DevicePin ) const
{
	for ( size_t i = 0; i < m_CaptureFilters.size(); ++i )
	{
		if( m_CaptureFilters[i]->GetUID()		== StreamUID &&
			m_CaptureFilters[i]->GetDevicePin() == DevicePin )
		{
			return m_CaptureFilters[i].get();
		}
	}
	return NULL;
}

class FindPinPred
{
	const UUID	m_streamUID;
	const long	m_DevicePin;
public:
	bool operator()( const std::pair<UUID, long>& PinInfo ) const
	{
		return	PinInfo.first		== m_streamUID &&
				PinInfo.second		== m_DevicePin;	
	}

	FindPinPred( const UUID& streamUID, long DevicePin ):
		m_streamUID( streamUID ),
		m_DevicePin( DevicePin )
	{
	}
};

void	CaptureServer::SetUsedPins ( const std::vector< std::pair<UUID, long> >& RequiredPins  )
{
	std::vector< boost::shared_ptr<CDevicePin> >::iterator it = m_CaptureFilters.begin();

	while( it != m_CaptureFilters.end() )
	{
		if( std::find_if( RequiredPins.begin(), RequiredPins.end(), 
							FindPinPred( (*it)->GetUID(), (*it)->GetDevicePin() )  ) == 
			RequiredPins.end()
			)
		{
			UUID MutableUID = (*it)->GetUID();
			UuidString stStreamUID;
			::UuidToString( &MutableUID, &stStreamUID );
			// not found
			Elvees::OutputF(Elvees::TInfo, TEXT("Delete unused stream {pin=%ld} \"%s\""), 
								(*it)->GetDevicePin(),
								stStreamUID );
			RpcStringFree(&stStreamUID);
			(*it)->Delete();
			it = m_CaptureFilters.erase( it );
		}
		else
		{
			++it;
		}
	}
}

void	CaptureServer::GetUsedPins ( std::vector< std::pair<UUID, long> >& RequiredPins  ) const
{
	RequiredPins.clear();
	std::vector< boost::shared_ptr<CDevicePin> >::const_iterator it = m_CaptureFilters.begin();
	for ( ; it != m_CaptureFilters.end(); ++it )
	{
		RequiredPins.push_back( std::make_pair( (*it)->GetUID(), (*it)->GetDevicePin() ) );
	}
}

void CaptureServer::DeleteAllDevices()
{
	for ( size_t i =0; i < m_CaptureDevices.size() ; ++i )
	{
		m_CaptureDevices[i]->StopDevice();
	}
	m_CaptureDevices.clear();
}

void CaptureServer::DeleteAllPins()
{
	m_CaptureFilters.clear();
}