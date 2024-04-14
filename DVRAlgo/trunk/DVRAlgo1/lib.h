///////////////////////////////////////////////////////////////////////////////////////////////////
// lib.h
// ---------------------
// begin     : 2006
// modified  : 5 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// A T T E N T I O N !                                            //
// Do NOT export this file to another libraries and applications. //
// Use "DVRAlgo/Common/common.h" instead.                         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "algorithm/parameters.h"

///================================================================================================
/// \class CDVRAlgo1Dll.
/// \brief Main DLL class of the first DVR algorithm.
///================================================================================================
class CDVRAlgo1Dll 
{
public:
  boost::mutex        m_dataLocker; //!< object locks/unlocks the data being transferred between threads
  dvralgo::Parameters m_parameters; //!< algorithm parameters

public:
  CDVRAlgo1Dll();
};

extern CDVRAlgo1Dll theLib;

const int DEFAULT_PARAM_DIALOG_WIDTH = 320;
const int DEFAULT_PARAM_DIALOG_HEIGHT = 200;

