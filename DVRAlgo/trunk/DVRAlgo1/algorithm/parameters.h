///////////////////////////////////////////////////////////////////////////////////////////////////
/// parameters.h
/// ---------------------
/// begin     : 2006
/// modified  : 5 Feb 2006
/// author(s) : Albert Akhriev
/// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_DVR_ALGO_1_PARAMETERS_H__
#define __DEFINE_DVR_ALGO_1_PARAMETERS_H__

#include "../../Common/dvr_common.h"

namespace dvralgo
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct AlgorithmParams.
/// \brief  AlgorithmParams.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct AlgorithmParams : public alib::BaseParamSection
{
  alib::FltParam p_backgrUpdateRate;      //!<
  alib::FltParam p_backgrUpdateTau;       //!< 
  alib::FltParam p_holeTimeout;           //!< 
  alib::FltParam p_changeThreshold;       //!< 
  alib::FltParam p_minRgnPerimeter;       //!<
  alib::FltParam p_minRgnArea;            //!< 
  alib::IntParam p_maxBoxes;              //!< 
  alib::FltParam p_timeOutBeetweenEvents; //!< 
  alib::FltParam p_maxEventLifeTime;      //!< 

  explicit AlgorithmParams( int & controlIdentifier );
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct DemoParams.
/// \brief  DemoParams.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DemoParams : public alib::BaseParamSection
{
  alib::BlnParam p_bShowCurrentState; //!<

  explicit DemoParams( int & controlIdentifier );
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class Parameters.
/// \brief Parameters.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Parameters : public alib::BaseParamSection
{
  dvralgo::AlgorithmParams s_algorithm;      //!< algorithm section
  dvralgo::DemoParams      s_demo;           //!< demo section

  Parameters( int control_identifier );
};

extern const int FIRST_CONTROL_IDENTIFIER;

} // namespace dvralgo

#endif // __DEFINE_DVR_ALGO_1_PARAMETERS_H__

