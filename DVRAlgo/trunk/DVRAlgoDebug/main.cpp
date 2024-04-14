#include "stdafx.h"
#include "camera_thread.h"
#include "console_imessage.h"
#include "main.h"
#include "../Common/dvr_common.h"
#include "../Common/milli_timer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp            theApp;
Elvees::MilliTimer g_timer;

dvralgo::PCreateCameraAnalyzer funcCreateCameraAnalyzer = 0;
dvralgo::PCreateHallAnalyzer   funcCreateHallAnalyzer = 0;
dvralgo::PSetDefaultParameters funcSetDefaultParameters = 0;
dvralgo::PGetParameters        funcGetParameters = 0;
dvralgo::PLoadParameters       funcLoadParameters = 0;
dvralgo::PSaveParameters       funcSaveParameters = 0;

const char PRESS_ANY_KEY[] = "Press any key to continue";

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Main function.
///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
	argc;argv;
	if (!AfxWinInit( ::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0 ))
		return 1;

	int                        cameraNum = dvralgo::MAX_CAMERA_NUMBER;
	dvralgo::CameraThreadMap   cameras;
	Elvees::ConsoleMessage     output;
	Elvees::IHallAnalyzerPtr   pHall;
	dvralgo::TFromHallToCamera hall2camera;
	dvralgo::TFromCameraToHall camera2hall;
	dvralgo::ApplicationData   appData;
	HINSTANCE                  hAlgoLib = 0;
	dvralgo::TSetParameters    binParams;

	try
	{
		// Parse ini-file and initialize some application parameters.
		if (argc == 2)
		{
			Elvees::IniFileParser iniParser;
			if (iniParser.Initialize( argv[1] ))
			{
				std::string text;
				while (iniParser.Proceed())
				{
					if (iniParser.IsText( "algorithm" ))
					{
						iniParser.File() >> appData.algorithmNo;
						ELVEES_VERIFY( appData.algorithmNo > 0, "Wrong algorithm index" );
					}
					else if (iniParser.IsText( "algo1_params" ))
					{
						if (appData.algorithmNo == 1)
							iniParser.File() >> appData.algoFileName;
					}
					else if (iniParser.IsText( "algo2_params" ))
					{
						if (appData.algorithmNo == 2)
							iniParser.File() >> appData.algoFileName;
					}
				}
			}
		}
		else
		{
			std::cout << "Please, specify INI-file name as a single application parameter" << std::endl;
			std::cout << PRESS_ANY_KEY << std::endl;
			_getch();
			return 0;
		}

		// Load algorithm library.
		{
			char libName[256];
			sprintf( libName, "dvralgo%d.dll", appData.algorithmNo );
			hAlgoLib = AfxLoadLibrary( libName );
			ELVEES_VERIFY( hAlgoLib != 0, "!AfxLoadLibrary()" );

			funcCreateCameraAnalyzer = MY_GET_PROC_ADDR( hAlgoLib, CreateCameraAnalyzer );
			funcCreateHallAnalyzer   = MY_GET_PROC_ADDR( hAlgoLib, CreateHallAnalyzer );
			funcSetDefaultParameters = MY_GET_PROC_ADDR( hAlgoLib, SetDefaultParameters );
			funcGetParameters        = MY_GET_PROC_ADDR( hAlgoLib, GetParameters );
			funcLoadParameters       = MY_GET_PROC_ADDR( hAlgoLib, LoadParameters );
			funcSaveParameters       = MY_GET_PROC_ADDR( hAlgoLib, SaveParameters );

			ELVEES_VERIFY( funcCreateCameraAnalyzer != 0, "Function CreateCameraAnalyzer() was not found" );
			ELVEES_VERIFY( funcCreateHallAnalyzer   != 0, "Function CreateHallAnalyzer() was not found" );
			//ELVEES_VERIFY( funcShowParameterDialog  != 0, "Function ShowParameterDialog () was not found" );
			ELVEES_VERIFY( funcSetDefaultParameters != 0, "Function SetDefaultParameters() was not found" );
			ELVEES_VERIFY( funcGetParameters        != 0, "Function GetParameters() was not found" );
			ELVEES_VERIFY( funcLoadParameters       != 0, "Function LoadParameters() was not found" );
			ELVEES_VERIFY( funcSaveParameters       != 0, "Function SaveParameters() was not found" );
		}

		// Modify algorithm's settings.
		if (appData.algorithmNo == 2) // orwell
		{
			ASSERT( sizeof(BinaryData::value_type) == sizeof(std::string::value_type) );
			ELVEES_ASSERT( !(appData.algoFileName.empty()) );
			appData.algoParams.resize( appData.algoFileName.size()+1, 0 );
			memcpy( &(appData.algoParams[0]), appData.algoFileName.c_str(), appData.algoFileName.size() );
		}
		else // non-orwell
		{
			// Try to load parameter file.
			if ( !(appData.algoFileName.empty()) )
			{
				if (!funcLoadParameters( appData.algoFileName.c_str(), &output ))
				{
					ELVEES_ASSERT( funcSetDefaultParameters( &(appData.algoParams), &output ) );
					ELVEES_ASSERT( funcSaveParameters( appData.algoFileName.c_str(), &output ) );
				}				
			}

		}

		// Create and launch camera threads.
		for (int i = 0; i < cameraNum; i++)
		{
			Elvees::ICameraGrabber * pGrab = Elvees::CreateChcsvaCameraGrabber( i, &output );
			if (pGrab == 0)
				break;
			cameraNum = Elvees::GetNumberOfAvailablesCameras();
			(cameras[i]).Initialize( i, &appData, &output, pGrab );
			//>>>>>
			//break;
			//>>>>>
		}
		ELVEES_VERIFY( !(cameras.empty()), "No one camera thread was created" );
		std::cout << "The number of available cameras: " << cameraNum << std::endl;
		std::cout << "The number of launched camera threads: " << (int)(cameras.size()) << std::endl;

		// Create a hall analyzer.
		pHall.reset( funcCreateHallAnalyzer( &(appData.algoParams), &output ) );
		ELVEES_ASSERT( pHall.get() != 0 );

		///////////////////////
		// Run surveillance. //
		///////////////////////
		{
			dvralgo::TAlgoSensitivity sensitivity;
			sensitivity.data = 0.5f;
			dvralgo::TProtectionDeviceName protectionDevice;
			protectionDevice.data ="U3nb40Ls2CfPO561J350d8cvbcvMFGJdJged8gdfg";// "COM4";

			dvralgo::CameraThreadMap::iterator camIt;
			// Collect information from all camera analyzers.
			for (camIt = cameras.begin(); camIt != cameras.end(); ++camIt)
			{
				(camIt->second.GetAnalyzer())->SetData( &sensitivity );
				(camIt->second.GetAnalyzer())->SetData( &protectionDevice );
			}
		}

		{
			while (_kbhit()) _getch(); // clear message queue
			for (int ch = 0; !_kbhit() || ((ch = _getch()) != 27); ch = 0)
			{
				dvralgo::CameraThreadMap::iterator camIt;

				while (_kbhit()) _getch(); // clear message queue
				//Sleep(100);

				// Change parameters if necessary.
				//   if (ch == 'p')
				//   {
				//     if (funcShowParameterDialog( &(binParams.data), &output, 0, 0, 0 ) > 0)
				//     {
				//       ELVEES_ASSERT( funcSaveParameters( appData.algoFileName.c_str(), &output ) );
				//       for (camIt = cameras.begin(); camIt != cameras.end(); ++camIt)
				//         ELVEES_ASSERT( (camIt->second.GetAnalyzer())->SetData( &binParams ) );
				//     }
				//   }

				Sleep( 1000 );

				// Collect information from all camera analyzers.
				for (camIt = cameras.begin(); camIt != cameras.end(); ++camIt)
				{
					ELVEES_ASSERT( (camIt->second.GetAnalyzer())->GetData( &camera2hall ) );
					ELVEES_ASSERT( pHall->SetData( &camera2hall ) );		  
				}
				
				Sleep(100);
				// Analyze the overall statistics.
				ulong prTime = g_timer.Time();
				pHall->Process( prTime );

				//GetData from hall analyzer
				dvralgo::TEventPtrs events;
				pHall->GetData( &events );

				for (size_t k = 0; k < events.data.size(); k++)
				{
#if 0
					if ( ((Elvees::ICameraMotionEvent *)events.data[k].get())->IsClosed() ) 
					{
						std::cout << "cl ev, cam " <<  ((Elvees::ICameraMotionEvent *)events.data[k].get())->SensorId() << " "
							<< " tb " << ((Elvees::ICameraMotionEvent *)events.data[k].get())->BeginTime() 
							<< " te "  << ((Elvees::ICameraMotionEvent *)events.data[k].get())->EndTime()
							<< " call time " << prTime; 
						//((Elvees::ICameraMotionEvent *)events.data[k].get())
						const Elvees::ICameraMotionEvent::MotionRectArr & arr  = ((Elvees::ICameraMotionEvent *)events.data[k].get())->GetMotionRectangles();
						std::cout << " size " << arr.size() <<  std::endl;
						
						std::cout << " id " << ((dvralgo::CameraMotionEvent *)events.data[k].get())->GetEventNum() <<  std::endl;

						std::cout << "------------------------------------------------------------------" << std::endl;
						((dvralgo::CameraMotionEvent *)events.data[k].get())->GetLastSecondMotionRectangles(); 
					}
					else
					{
						std::cout << "op ev, cam " <<   ((Elvees::ICameraMotionEvent *)events.data[k].get())->SensorId() << " "
							<< " tb " << ((Elvees::ICameraMotionEvent *)events.data[k].get())->BeginTime()
							<< " te "  << ((Elvees::ICameraMotionEvent *)events.data[k].get())->EndTime()
							<< " call time " << prTime;
						const Elvees::ICameraMotionEvent::MotionRectArr & arr  = ((Elvees::ICameraMotionEvent *)events.data[k].get())->GetMotionRectangles();
						std::cout << " size " << arr.size() <<  std::endl;
						std::cout << " id " << ((dvralgo::CameraMotionEvent *)events.data[k].get())->GetEventNum() <<  std::endl;

					}
#endif
				}


				// Transfer feedback information to all camera analyzers.
				for (camIt = cameras.begin(); camIt != cameras.end(); ++camIt)
				{
					hall2camera.data.first = camIt->first;
					ELVEES_ASSERT( pHall->GetData( &hall2camera ) );
					ELVEES_ASSERT( (camIt->second.GetAnalyzer())->SetData( &hall2camera ) );
				}
			}
			dvralgo::g_bContinueRunning = false;
		}

		// Finalize camera threads.
		cameras.clear();
	}
	catch (std::runtime_error & e)
	{
		std::cout << e.what() << std::endl << std::endl << PRESS_ANY_KEY << std::endl;
	}
	catch (...)
	{
		std::cout << "unhandled exception" << std::endl;
	}

	// Release algorithm library.
	pHall.reset();
	if ((hAlgoLib != 0) && !AfxFreeLibrary( hAlgoLib ))
		std::cout << "!AfxFreeLibrary()" << std::endl;

	std::cout << PRESS_ANY_KEY << std::endl;
	_getch();
	return 0;
}

