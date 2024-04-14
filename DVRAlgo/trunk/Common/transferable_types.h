///////////////////////////////////////////////////////////////////////////////////////////////////
// transferable_types.h
// ---------------------
// begin     : 2006
// modified  : 20 Mar 2006
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DVR_TRANSFERABLE_TYPES_H__
#define __DVR_TRANSFERABLE_TYPES_H__

namespace dvralgo
{

enum
{
  DATAID_QIMAGE = 1,          //!< QImage object being transferred between application modules
  DATAID_MENU_PTR,            //!< pointer to a menu used to send command messages from application to any object
  DATAID_INFO_PAGE,           //!< text string that contains info page of an object
  DATAID_FROM_HALL_TO_CAMERA, //!< data being transferred from a hall analyzer to a camera analyzer
  DATAID_FROM_CAMERA_TO_HALL, //!< data being transferred from a camera analyzer to a hall analyzer
  DATAID_BYTE_IMAGE_PTR,      //!< 
  DATAID_SET_PARAMETERS,      //!< 
  DATAID_EVENT_PTRS,          //!<
  DATAID_ALGO_SENSITIVITY,    //!<  
  DATAID_PROTECTION_DEVICE
};

// .
typedef
Elvees::TDataWraper< QImage, dvralgo::DATAID_QIMAGE >
TQImage;

// .
typedef
Elvees::TDataWraper< Elvees::AbstractMenu*, dvralgo::DATAID_MENU_PTR >
TMenuPtr;

// .
typedef
Elvees::TDataWraper< std::string, dvralgo::DATAID_INFO_PAGE >
TInfoPage;


typedef
Elvees::TDataWraper< std::pair< int, BinaryData >, dvralgo::DATAID_FROM_HALL_TO_CAMERA >
TFromHallToCamera;

typedef
Elvees::TDataWraper< std::pair< int, BinaryData >, dvralgo::DATAID_FROM_CAMERA_TO_HALL >
TFromCameraToHall;

// .
typedef
Elvees::TDataWraper< std::pair< int, const Arr2ub* >, dvralgo::DATAID_BYTE_IMAGE_PTR >
TByteImagePtr;

// .
typedef
Elvees::TDataWraper< BinaryData, dvralgo::DATAID_SET_PARAMETERS >
TSetParameters;

// .
typedef
Elvees::TDataWraper< Elvees::IEventPtrArr, dvralgo::DATAID_EVENT_PTRS >
TEventPtrs;

// .
typedef
Elvees::TDataWraper< float, dvralgo::DATAID_ALGO_SENSITIVITY >
TAlgoSensitivity;

typedef
Elvees::TDataWraper< std::string, dvralgo::DATAID_PROTECTION_DEVICE >
TProtectionDeviceName;

} // namespace dvralgo

#endif // __DVR_TRANSFERABLE_TYPES_H__

