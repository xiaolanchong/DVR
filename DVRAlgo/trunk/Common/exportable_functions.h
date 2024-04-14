///////////////////////////////////////////////////////////////////////////////////////////////////
// exportable_functions.h
// ---------------------
// begin     : 2006
// modified  : 20 Mar 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DVR_EXPORTABLE_FUNCTIONS_H__
#define __DVR_EXPORTABLE_FUNCTIONS_H__

namespace dvralgo
{

extern "C"
{

///------------------------------------------------------------------------------------------------
/// \brief Function creates an instance of camera analyzer.
///
/// \param  cameraNo     the index of a camera.
/// \param  pParameters  pointer to a binary storage of parameter settings.
/// \param  pMask        pointer to a mask of visible (nonzero) and non-visible (zero) points.
/// \param  pOutput      pointer to the message handler.
/// \return              Ok = pointer to the instance of created camera analyzer, otherwise 0.
///------------------------------------------------------------------------------------------------
Elvees::ICameraAnalyzer * CreateCameraAnalyzer( sint               cameraNo,
                                                const BinaryData * pParameters,
                                                const Arr2ub     * pMask,
                                                Elvees::IMessage * pOutput/*, HDC & */);
                                                
typedef Elvees::ICameraAnalyzer * (*PCreateCameraAnalyzer)( sint               cameraNo,
                                                            const BinaryData * pParameters,
                                                            const Arr2ub     * pMask,
                                                            Elvees::IMessage * pOutput/*, HDC &*/ );


///------------------------------------------------------------------------------------------------
/// \brief Function creates an instance of hall analyzer.
///
/// \param  pOutput   pointer to the message handler.
/// \return           Ok = pointer to the instance of created hall analyzer, otherwise 0.
///------------------------------------------------------------------------------------------------
Elvees::IHallAnalyzer * CreateHallAnalyzer( const BinaryData * pParameters,
                                            Elvees::IMessage * pOutput );
                                            
typedef Elvees::IHallAnalyzer * (*PCreateHallAnalyzer)( const BinaryData * pParameters,
                                                        Elvees::IMessage * pOutput );


///------------------------------------------------------------------------------------------------
/// \brief Function shows dialog window used to edit parameters stored in DLL class.
///
/// \param  pBinParams  in: pointer to binary storage of parameters, out: modified parameters.
/// \param  pOutput     pointer to the message handler or 0.
/// \param  hParentWnd  the handle of a parent window or 0.
/// \param  dlgWidth    desired width of dialog window or 0 (default width).
/// \param  dlgHeight   desired height of dialog window ot 0 (default height).
/// \return             Ok = +1, Cancel = -1, error = 0.
///------------------------------------------------------------------------------------------------
sint ShowParameterDialog( BinaryData       * pBinParams,
                          Elvees::IMessage * pOutput,
                          ulong              hParentWnd,
                          uint               dlgWidth,
                          uint               dlgHeight );

typedef sint (*PShowParameterDialog)( BinaryData       * pBinParams,
                                      Elvees::IMessage * pOutput,
                                      ulong              hParentWnd,
                                      uint               dlgWidth,
                                      uint               dlgHeight );


///------------------------------------------------------------------------------------------------
/// \brief Function sets default parameters.
///
/// \param  pBinParams  pointer to destination binary storage of parameters or 0.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///------------------------------------------------------------------------------------------------
bool SetDefaultParameters( BinaryData * pBinParams, Elvees::IMessage * pOutput );

typedef bool (*PSetDefaultParameters)( BinaryData * pBinParams, Elvees::IMessage * pOutput );


///------------------------------------------------------------------------------------------------
/// \brief Function fills a binary storage by the current parameters settings stored in DLL.
///
/// \param  pBinParams  pointer to destination binary storage of parameters or 0.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///------------------------------------------------------------------------------------------------
bool GetParameters( BinaryData * pBinParams, Elvees::IMessage * pOutput );

typedef bool (*PGetParameters)( BinaryData * pBinParams, Elvees::IMessage * pOutput );


///------------------------------------------------------------------------------------------------
/// \brief Function loads parameters from a file.
///
/// \param  fileName    the name of source file.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///------------------------------------------------------------------------------------------------
bool LoadParameters( const char * filename, Elvees::IMessage * pOutput );

typedef bool (*PLoadParameters)( const char * filename, Elvees::IMessage * pOutput );


///------------------------------------------------------------------------------------------------
/// \brief Function saves parameters to a file.
///
/// \param  fileName    the name of destination file.
/// \param  pOutput     pointer to the message handler or 0.
/// \return             Ok = true.
///------------------------------------------------------------------------------------------------
bool SaveParameters( const char * filename, Elvees::IMessage * pOutput );

typedef bool (*PSaveParameters)( const char * filename, Elvees::IMessage * pOutput );

} // extern "C"

} // namespace dvralgo

#endif // __DVR_EXPORTABLE_FUNCTIONS_H__

