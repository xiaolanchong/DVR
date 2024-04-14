///////////////////////////////////////////////////////////////////////////////////////////////////
// lib.cpp
// ---------------------
// begin     : 2006
// modified  : 5 Feb 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lib.h"
#include "../Common/parameter_functions.inl"

CDVRAlgo1Dll theLib;

///------------------------------------------------------------------------------------------------
/// \brief Contructor.
///------------------------------------------------------------------------------------------------
CDVRAlgo1Dll::CDVRAlgo1Dll() : m_parameters( dvralgo::FIRST_CONTROL_IDENTIFIER )
{
  m_parameters.set_default();
}

