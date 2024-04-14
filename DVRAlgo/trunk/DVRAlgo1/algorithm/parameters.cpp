///////////////////////////////////////////////////////////////////////////////////////////////////
/// parameters.cpp
/// ---------------------
/// begin     : 2006
/// modified  : 5 Feb 2006
/// author(s) : Albert Akhriev
/// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "parameters.h"

namespace dvralgo
{

const int FIRST_CONTROL_IDENTIFIER = 1000;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
AlgorithmParams::AlgorithmParams( int & controlIdentifier )
{
  p_backgrUpdateRate.init(
    "backgrUpdateRate",
    "background update rate",
    0.02f, 0.0001f, 1.0f, this, &controlIdentifier );

  p_backgrUpdateTau.init(
    "backgrUpdateTau",
    "the time parameter of background update in seconds",
    10.0f, 0.001f, 1000.0f, this, &controlIdentifier );

  p_holeTimeout.init(
    "holeTimeout",
    "timeout for a hole existance in seconds",
    2.0f, 1.0f, 100.0f, this, &controlIdentifier );

  p_changeThreshold.init(
    "changeThreshold",
    "signal/noise threshold ration for change detection",
    4.0f, 3.0f, 7.0f, this, &controlIdentifier );

  p_minRgnPerimeter.init(
    "minRgnPerimeter",
    "the minimal perimeter of detected motion region",
    10.0f, 1.0f, 1000.0f, this, &controlIdentifier );

  p_minRgnArea.init(
    "minRgnArea",
    "the minimal area of detected motion region",
    64.0f, 1.0f, 10000.0f, this, &controlIdentifier );
 
  p_maxBoxes.init( 
    "maxBoxes", 
    "maximal number of rectangles on one frame", 
    3, 1, 16, this, &controlIdentifier );
  
  p_timeOutBeetweenEvents.init(
    "timeOutBeetweenEvents",
    "maximal time to motion pause for one event, in seconds",
    1.0f, 0.1f, 5.0f, this, &controlIdentifier ); //!< 

  p_maxEventLifeTime.init(
    "maxEventLifeTime",
    "maximal time to event to live, in seconds",
    7.0f, 0.1f, 60.0f, this, &controlIdentifier ); //!< 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
DemoParams::DemoParams( int & controlIdentifier )
{
  p_bShowCurrentState.init(
    "bShowCurrentState",
    "show the current state of surveillance process if checked",
    false, this, &controlIdentifier );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
Parameters::Parameters( int control_identifier )
: s_algorithm( control_identifier ),
  s_demo( control_identifier )
{
  s_algorithm.init( _T("Algorithm"), _T("algorithm parameters"), this );
  s_demo.init( _T("Demo"), _T("demo parameters"), this );
}

} // namespace dvralgo

