/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: OGLRender.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-11-22
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: OpenGL implementation of the IDVRStreamRender interface
*
*
*/

#ifndef __OGLRENDER_HPP__
#define __OGLRENDER_HPP__

struct CHCS::IFrame;
class StreamCache;
class OGLWrapper2D;


class OGLStreamRender : public IDVRStreamRender
{
	static const int Timeout = 100;
public:
	class Camera : public IDVRStreamRender::ICamera
	{
		std::vector<unsigned char>	m_FrameUserData;
	public:
		Camera( CHCS::IStream* stream, int id );
		Camera( const Camera& other );
		virtual ~Camera();
	public:
		/*
		*	Draws and stretch\shrink ( if needed ) bitmap from camera at the specified position
		*/
		bool Draw( float x, float y, float width, float height );

		/*
		*	Copies active frame data. 
		*	@width: Frame width
		*	@height: Frame height
		*	@bpp: Frame bit-per-pixel
		*	@data: Frame bitmap data pointer, if @data is null, only frame dimensions returned
		*/
		void GetFrameData( int& width, int& height, int& bpp, void** data ); 

		/*
		*	Sets camera release timeout in seconds, ( to disable this, timeout must be = 0 )
		*		when release time come, Camera frees it's internal data, and ICamera methods doesn't nothing,
		*		only IsReleased returns true; You can remove camera using delete operator
		*/
		void SetReleaseTimeout( long timeOut );

		/*
		*	Gets camera release timeout in seconds
		*/
		long GetReleaseTimeout() const;

		/*
		*	Checks if camera released or not
		*/
		bool IsReleased() const;

		/*
		*	Returns camera id
		*/
		int GetCameraID() const;

		const std::vector<unsigned char>&	GetFrameUserData() const
		{
			return m_FrameUserData;
		}

	public:
		/*
		*	Called by manager to update cameras
		*/
		void OnUpdate();

	private:
		bool mReleased;
		int mCameraID;
		long mReleaseTimeout;
		wxStopWatch mReleaseWatch;
		CHCS::IStream* mStream;
	};

public:
	/*
	*	Returns array with available camera indices 
	*/
	void GetAvailableCameras( std::vector<int>& camerasVec );

	/*
	*	Makes this render active
	*/
	void MakeActive();

	/*
	*	Checks render activity
	*/
	bool IsActive() const;

	/*
	*	Sets render refresh period in milliseconds
	*		Default value is 1000ms
	*/
	void SetRefreshPeriod( int period );

	/*
	*	Gets render refresh period in milliseconds
	*/
	int GetRefreshPeriod() const;

	/*
	*	Binds callback function
	*	Callback function will be called every time 
	*	when frame render occurs
	*/
	void BindCallback( boost::function0<void> fn );

	/*
	*	Unbind callback function
	*/
	void UnbindCallback();

public:
	/*
	*	Returns camera with the specified index
	*/
	ICamera* GetCamera( int cameraIdx );
public:
	/*
	*	Calls by render manager's timer
	*/
	void OnRenderUpdate();
public:
	void OnCameraAdded( Camera* camera );
	void OnCameraRemoved( Camera* camera );
	void OnCameraUpdate();

public:
	OGLStreamRender( boost::shared_ptr<CHCS::IStreamManager> streamManager );
	virtual ~OGLStreamRender();

private:
	boost::shared_ptr<CHCS::IStreamManager> mStreamManager;
	boost::function0<void> mCallbackFn;
private:
	std::list<Camera*> mCameras;
};


#endif //__OGLRENDER_HPP__
