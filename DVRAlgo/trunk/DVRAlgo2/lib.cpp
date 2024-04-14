///////////////////////////////////////////////////////////////////////////////////////////////////
// lib.cpp
// ---------------------
// begin     : 2006
// modified  : 23 Jan 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
// Note!
//
// If this DLL is dynamically linked against the MFC DLLs, any functions exported from this DLL which
// call into MFC must have the AFX_MANAGE_STATE macro added at the very beginning of the function.
//
// For example:
// extern "C" BOOL PASCAL EXPORT ExportedFunction()
// {
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
//   // normal function body here
// }
//
// It is very important that this macro appear in each function, prior to any calls into MFC.  This means that
// it must appear as the first statement within the function, even before any object variable declarations
// as their constructors may generate calls into the MFC DLL. Please see MFC Technical Notes 33 and 58 for
// additional details.
//

BEGIN_MESSAGE_MAP(CDVRAlgo2Dll, CWinApp)
END_MESSAGE_MAP()

CDVRAlgo2Dll theDll;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Contructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
CDVRAlgo2Dll::CDVRAlgo2Dll()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Initialization.
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDVRAlgo2Dll::InitInstance()
{
  CWinApp::InitInstance();
  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Parameter functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function loads parameters from a file.
///
/// \param  fileName    the name of source file.
/// \param  pBinParams  pointer to destination binary storage of parameters.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LoadParameters( const char * fileName, Elvees::IMessage * pOutput )
{
  fileName;pOutput;
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function saves parameters to a file.
///
/// \param  fileName    the name of destination file.
/// \param  pBinParams  pointer to source binary storage of parameters.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool SaveParameters( const char * fileName, Elvees::IMessage * pOutput )
{
  fileName;pOutput;
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function shows dialog window used to edit parameters.
///
/// \param  pBinParams  in: pointer to binary storage of parameters, out: modified parameters.
/// \param  pOutput     pointer to the message handler or 0.
/// \param  hParentWnd  the handle of a parent window or 0.
/// \param  dlgWidth    desired width of dialog window or 0 (default width).
/// \param  dlgHeight   desired height of dialog window ot 0 (default height).
/// \return             Ok = +1, Cancel = -1, error = 0.
///////////////////////////////////////////////////////////////////////////////////////////////////
sint ShowParameterDialog( BinaryData       * pBinParams,
                          Elvees::IMessage * pOutput,
                          ulong              hParentWnd,
                          uint               dlgWidth,
                          uint               dlgHeight )
{
  pBinParams;pOutput;hParentWnd;dlgWidth;dlgHeight;
  return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function sets parameters stored in DLL by default values.
///
/// \param  pBinParams  pointer to destination binary storage of parameters or 0.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool SetDefaultParameters( BinaryData       * pBinParams,
                           Elvees::IMessage * pOutput )
{ 
  pBinParams;pOutput;
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function fills a binary storage by the current parameters settings stored in DLL.
///
/// \param  pBinParams  pointer to destination binary storage of parameters or 0.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GetParameters( BinaryData * pBinParams, Elvees::IMessage * pOutput )
{
  pBinParams;pOutput;
  return false;
}

