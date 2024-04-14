/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: DVRRender.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-11-22
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: DVR Render main interfaces
*
*
*/
#ifndef __DVRRENDER_HPP__
#define __DVRRENDER_HPP__

class wxWindow;
class IDVRRender;
class IDVRStreamRender;
class IDVRArchiveRender;
class IDVRGraphicsWrapper;

//////////////////////////////////////////////////////////////////////////
class IDVRRenderManager
{
public:
	/*
	*	Checks video adapter capabilities, and print report 
	*		to the errors out if any error found. Return true if 
	*		adapter match capabilities
	*	
	*/
//	virtual bool CheckVideoCapabilities( std::string* errorsOut ) const;
public:
	/*
	*	Returns stream render instance
	*/
	virtual IDVRStreamRender* GetStreamRender() const = 0;

	/*
	*	Returns archive render instance	
	*/
	virtual IDVRArchiveRender* GetArchiveRender() const = 0;

	/*
	*	Returns graphics wrapper used for rendering
	*/
	virtual IDVRGraphicsWrapper* GetWrapper() = 0;

	/*
	*	Get parent wxWindow
	*/
	virtual wxWindow* GetWindow() = 0;

	/*
	*	Virtual destructor
	*/
	virtual ~IDVRRenderManager() = 0 {}	
};

//////////////////////////////////////////////////////////////////////////
class IDVRRender
{
public:
	/*
	*	Returns array with available camera indices 
	*/
	virtual void GetAvailableCameras( std::vector<int>& camerasVec ) = 0;

	/*
	*	Makes this render active
	*/
	virtual void MakeActive() = 0;

	/*
	*	Checks render activity
	*/

	virtual void BindCallback( boost::function0<void> fn ) = 0;

	/*
	*	Unbind callback function
	*/
	virtual void UnbindCallback() = 0;

protected:
	virtual ~IDVRRender() = 0 {}	
};

//////////////////////////////////////////////////////////////////////////
class IDVRStreamRender : public IDVRRender
{
public:
	class ICamera
	{
	public:
		virtual ~ICamera() = 0 {}
	public:
		/*
		*	Draws and stretch\shrink ( if needed ) bitmap from camera at the specified position
		*/
		virtual bool Draw( float x, float y, float width, float height ) = 0;

		/*
		*	Copies active frame data. 
		*	@width: Frame width
		*	@height: Frame height
		*	@bpp: Frame bit-per-pixel
		*	@data: Frame bitmap data pointer, if @data is null, only frame dimensions returned
		*/
		virtual void GetFrameData( int& width, int& height, int& bpp, void** data ) = 0; 

		/*
		*	Sets camera release timeout in seconds, ( to disable this, timeout must be = 0 )
		*		when release time come, Camera frees it's internal data, and ICamera methods doesn't nothing,
		*		only IsReleased returns true; You can remove camera using delete operator
		*/

		virtual void SetReleaseTimeout( long timeOut ) = 0;
		/*
		*	Gets camera release timeout in seconds
		*/
		virtual long GetReleaseTimeout() const = 0;

		/*
		*	Checks if camera released or not
		*/
		virtual bool IsReleased() const = 0;

		/*
		*	Returns camera id
		*/
		virtual int GetCameraID() const = 0;

		virtual const std::vector<unsigned char>&	GetFrameUserData() const = 0;
	};

public:
	/*
	*	Returns camera with the specified index
	*/
	virtual ICamera* GetCamera( int cameraIdx ) = 0;

protected:
    /*
	*	Virtual destructor
	*/
	virtual ~IDVRStreamRender() = 0 {};
};

class IDVRArchiveRender : public IDVRRender
{
public:
	virtual ~IDVRArchiveRender() {}

	virtual void	AddCameras( const std::vector<int>& CameraIndexArr ) = 0;
	virtual void	RemoveCameras( const std::vector<int>& CameraIndexArr ) = 0;
	virtual bool	DrawFrame(  int CameraIndex, float x, float y, float width, float height ) = 0;

	virtual void	AcquireArchive( time_t nStartTime, time_t nEndTime, const std::vector<int>& CameraIndexArr) = 0 ;
	virtual void	ReleaseArchive( ) = 0;

	virtual void	Play() = 0;
	virtual void	Pause() = 0;
	virtual void	Stop() = 0;
	virtual void	Seek( time_t NewArchivePosition ) = 0;

	virtual time_t GetArchivePosition() = 0;
	virtual boost::optional<time_t>	GetCameraArchivePosition(int CameraID) = 0;

	virtual bool	GetFrameUserData(int CameraID, std::vector<unsigned char>& UserFrameData ) = 0;
};

#endif //__DVRRENDER_HPP__
