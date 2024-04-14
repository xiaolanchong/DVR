///////////////////////////////////////////////////////////////////////////////////////////////////
// main.h
// ---------------------
// begin     : 2006
// modified  : 8 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_MAIN_H__
#define __DEFINE_MAIN_H__

#include "../Common/dvr_common.h"

namespace dvralgo
{

const int IMAGE_NUMBER_TO_SHOW = 4;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct ApplicationData.
/// \brief  ApplicationData.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ApplicationData
{
  int         algorithmNo;   //!< index of an algorithm to be used
  std::string algoFileName;  //!< the name of file of algorithm parameters
  BinaryData  algoParams;    //!< the binary storage of algorithm parameters
  
  ApplicationData()
  {
    algorithmNo = 0;
    algoFileName.clear();
    algoParams.clear();
  }
};

} // namespace dvralgo

extern dvralgo::PCreateCameraAnalyzer funcCreateCameraAnalyzer;
extern dvralgo::PCreateHallAnalyzer   funcCreateHallAnalyzer;
extern dvralgo::PShowParameterDialog  funcShowParameterDialog;
extern dvralgo::PSetDefaultParameters funcSetDefaultParameters;
extern dvralgo::PGetParameters        funcGetParameters;
extern dvralgo::PLoadParameters       funcLoadParameters;
extern dvralgo::PSaveParameters       funcSaveParameters;
extern Elvees::MilliTimer             g_timer;

#define MY_GET_PROC_ADDR(hLib,name) (dvralgo::P##name)(::GetProcAddress(hLib,#name))

#endif // __DEFINE_MAIN_H__

