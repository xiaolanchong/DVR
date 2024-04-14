/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: OGLRender.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-11-22
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "StdAfx.h"
#include "DllMain.hpp"
#include "chcsva.h"
#include "../../DVRRender.hpp"
#include "../../IDVRGraphicsWrapper.h"

#include "OGLWrapper.hpp"
#include "OGLRenderManager.hpp"
//#include "OGLArchiveRender.hpp"
#include "OGLStreamRender.hpp"

//////////////////////////////////////////////////////////////////////////
OGLStreamRender::OGLStreamRender( boost::shared_ptr<CHCS::IStreamManager> streamManager ):
	mStreamManager( streamManager )
{
	_ASSERT( mStreamManager );
}

OGLStreamRender::~OGLStreamRender()
{
}
//////////////////////////////////////////////////////////////////////////
void OGLStreamRender::GetAvailableCameras( std::vector<int>& camerasVec )
{
#if 0
	CAUUID streamsUIDs;
	if( mStreamManager->GetAvailableStreams( &streamsUIDs ) )
	{
		boost::shared_ptr<void>( streamsUIDs.pElems, CoTaskMemFree );

		for( ULONG i = 0; i < streamsUIDs.cElems; ++i )
		{
			//Detect stream id
			CHCS::IStream* stream = 0;
			if( mStreamManager->GetStreamByUID( &stream, &streamsUIDs.pElems[i] ) )
			{
				CHCS::IFrame* frame;
				if( stream->GetNextFrame( &frame, 100 ) ){
					int id = frame->GetSourceID();
					if( id != -1 ){
						camerasVec.push_back( id );
					}
					frame->Release();
				}
			}
		}
	}
#endif
}

void OGLStreamRender::MakeActive()
{
	OGLRenderManager* rm = dllGetRenderManager();
	rm->SetActiveRender( OGLRenderManager::RM_STREAM_RENDER );
}


bool OGLStreamRender::IsActive() const
{
	OGLRenderManager* rm = dllGetRenderManager();
	if ( OGLRenderManager::RM_STREAM_RENDER == rm->GetActiveRender() ){
		return true;
	}
	return false;
}


void OGLStreamRender::SetRefreshPeriod( int period )
{
	OGLRenderManager* rm = dllGetRenderManager();
	rm->SetRefreshPeriod( OGLRenderManager::RM_STREAM_RENDER, period );
}


int OGLStreamRender::GetRefreshPeriod() const
{
	OGLRenderManager* rm = dllGetRenderManager();
	return rm->GetRefreshPeriod( OGLRenderManager::RM_STREAM_RENDER );
}

IDVRStreamRender::ICamera* OGLStreamRender::GetCamera( int cameraIdx )
{
	if( cameraIdx > 0 )
	{
		CHCS::IStream* stream;
		if( mStreamManager->GetStreamByID( &stream, cameraIdx ) ){
			return new Camera( stream, cameraIdx );			
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
OGLStreamRender::Camera::Camera( CHCS::IStream* stream, int id ):
	mStream(stream),
	mCameraID(id),
	mReleased( false )
{
	//Check stream and id;
	assert( stream );
	assert( id >= 0);

	//Inform render class
	OGLStreamRender* sr = dllGetStreamRender();
	sr->OnCameraAdded( this );
}

OGLStreamRender::Camera::Camera( const Camera& other )
{
	mCameraID = other.mCameraID;
	mStream = other.mStream;
	mStream->AddRef();
	
	//Inform render class
	OGLStreamRender* sr = dllGetStreamRender();
	sr->OnCameraAdded( this );
}

OGLStreamRender::Camera::~Camera()
{
	if( mStream ){
		mStream->Release();
	}

	//Inform render class
//	OGLStreamRender* sr = dllGetStreamRender();
//	sr->OnCameraRemoved( this );
}
//////////////////////////////////////////////////////////////////////////
bool OGLStreamRender::Camera::Draw( float x, float y, float width, float height )
{
	if( mReleased ){
		m_FrameUserData.clear();
		return false;
	}

	CHCS::IFrame* frame;
	if( mStream->GetNextFrame( &frame, Timeout ) )
	{
		if( !frame )
		{
			m_FrameUserData.clear();
			return false;
		}
		boost::shared_ptr<CHCS::IFrame> _mFrame( frame, boost::mem_fn( &CHCS::IFrame::Release ) );
		BITMAPINFO bi;
		frame->GetFormat( &bi );

		if( !bi.bmiHeader.biWidth || !bi.bmiHeader.biWidth  )
		{
			m_FrameUserData.clear();
			return false;
		}

		size_t DataSize;
		bool res = frame->GetCustomData( 0, DataSize );
		if( res )
		{
			m_FrameUserData.resize( DataSize );
			frame->GetCustomData( m_FrameUserData.size() ?  &m_FrameUserData[0] : 0, DataSize );
		}
		else
		{
			m_FrameUserData.clear();
		}

		OGLGraphicsWrapper* g = dllGetGraphicsWrapper();
		g->DrawBitmap( 
			IDVRGraphicsWrapper::Rect( x, y, x + width, y + height ),
			bi.bmiHeader.biWidth,
			bi.bmiHeader.biHeight,
			bi.bmiHeader.biBitCount,
			frame->GetBytes() );
		return true;
	}
	else
	{
		return false;
	}
}

void OGLStreamRender::Camera::GetFrameData( int& width, int& height, int& bpp, void** data )
{
	if( mReleased ){
		return;
	}

	CHCS::IFrame* frame;
	if( mStream->GetNextFrame( &frame, Timeout ) )
	{
		// auto release
		boost::shared_ptr<CHCS::IFrame> _mFrame( frame, boost::mem_fn( &CHCS::IFrame::Release ) );
		BITMAPINFO bi;
		frame->GetFormat( &bi );
		float width = bi.bmiHeader.biWidth;
		float height = bi.bmiHeader.biHeight;
		float bpp = bi.bmiHeader.biBitCount;

		//If copy needed
		if( data ){
			memcpy( *data, frame->GetBytes(), bi.bmiHeader.biSizeImage );
		}
	}
}

void OGLStreamRender::Camera::SetReleaseTimeout( long timeOut )
{
	if( mReleased ){
		return;
	}

	mReleaseWatch.Start();
	mReleaseTimeout = timeOut;
}

long OGLStreamRender::Camera::GetReleaseTimeout() const
{
	if( mReleased ){
		return 0;
	}

	return mReleaseTimeout;
}

bool OGLStreamRender::Camera::IsReleased() const
{
	return mReleased;
}

int OGLStreamRender::Camera::GetCameraID() const
{
	return mCameraID;
}

void OGLStreamRender::Camera::OnUpdate()
{
	//If release time come...
	if( mReleaseWatch.Time() >= mReleaseTimeout )
	{
		//Release stream
		if( mStream )
		{
			mStream->Release();
			mStream = 0;
		}
		mReleased = true;
		mReleaseWatch.Pause();
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void OGLStreamRender::BindCallback( boost::function0<void> fn )
{
	mCallbackFn = fn;
}


void OGLStreamRender::UnbindCallback()
{
	mCallbackFn = 0;
}

//////////////////////////////////////////////////////////////////////////
void OGLStreamRender::OnRenderUpdate()
{
	if( mCallbackFn ){
		mCallbackFn();
	}
}
//////////////////////////////////////////////////////////////////////////
void OGLStreamRender::OnCameraAdded( Camera* camera )
{
	for( std::list<Camera*>::iterator it = mCameras.begin(); it != mCameras.end(); ++it )
	{
		//If camera already present
		if( *it == camera ){
			return;
		}
	}

	mCameras.push_back( camera );
}

void OGLStreamRender::OnCameraRemoved( Camera* camera )
{
	for( std::list<Camera*>::iterator it = mCameras.begin(); it != mCameras.end(); ++it )
	{
		//If camera already present
		if( *it == camera ){
			mCameras.erase( it );
		}
	}
}


void OGLStreamRender::OnCameraUpdate()
{
	for( std::list<Camera*>::iterator it = mCameras.begin(); it != mCameras.end(); ++it )
	{
		(*it)->OnUpdate();
	}
}