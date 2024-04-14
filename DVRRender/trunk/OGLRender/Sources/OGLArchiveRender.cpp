/*
*	DVR 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: OGLRender.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2006-01-24
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/

#include "StdAfx.h"
#include "chcsva.h"
#include "DllMain.hpp"
#include "../../DVRRender.hpp"

#include "OGLWrapper.hpp"
#include "OGLRenderManager.hpp"
#include "OGLArchiveRender.hpp"

//////////////////////////////////////////////////////////////////////////
void OGLArchiveRender::Camera::Start()
{
	switch( mState )
	{
	case CS_Stop:
		{
			
		}
		break;
	case CS_Paused:
		{

		}
		break;
	}
}

void OGLArchiveRender::Camera::Stop()
{
	//If camera isn't stopped
	if( mState != CS_Stop )
	{
		mStream->Stop();
		mState = CS_Stop;	
	}
}


void OGLArchiveRender::Camera::Pause()
{
	switch( mState )
	{
	case CS_Paused:
		{

		}
		break;
	case CS_Start:
		{
			if( mStream ){
				CHCS::IFrame* frame;
				if( mStream->GetNextFrame( &frame, Timeout ) ){
					mFrame = boost::shared_ptr<CHCS::IFrame>( frame, boost::mem_fn( &CHCS::IFrame::Release ) );
				}
				else{
					mState = CS_Error;
				}
			}
		}
		break;
	case CS_Stop:
		{

		}
		break;
	}

	mState = CS_Paused;	
}


bool OGLArchiveRender::Camera::IsPlaying() const
{
	if( mState == CS_Start ){
		return true;
	}
	return false;
}


bool OGLArchiveRender::Camera::IsPaused() const
{
	if( mState == CS_Paused ){
		return true;
	}
	return false;
}


void OGLArchiveRender::Camera::Seek( INT64 position )
{
	assert( position );
	if( mStream->SeekTo( position ) )
	{
		CHCS::IFrame* frame;
		if( mStream->GetNextFrame( &frame, Timeout ) ){
			mFrame = boost::shared_ptr<CHCS::IFrame>( frame, boost::mem_fn( &CHCS::IFrame::Release ) );
		}
		else{
			mState = CS_Error;
		}
	}
}


INT64 OGLArchiveRender::Camera::GetPosition() const
{
	if( mReleased ){
		return -1;
	}

	if( mFrame ){
		return mFrame->GetTime();
	}

	return -1;
}


void OGLArchiveRender::Camera::Draw( float x, float y, float width, float height )
{
	if( mReleased ){
		//TODO Add special drawing function here to indicate that camera is released
	}

	if( mFrame )
	{
		BITMAPINFO bi;
		mFrame->GetFormat( &bi );

		OGLGraphicsWrapper* g = dllGetGraphicsWrapper();
		g->DrawBitmap( IDVRGraphicsWrapper::Rect( x, y, x + width, y + height ),
			bi.bmiHeader.biWidth,
			bi.bmiHeader.biHeight,
			bi.bmiHeader.biBitCount,
			mFrame->GetBytes() );
	}
}

void OGLArchiveRender::Camera::SetReleaseTimeout( long timeOut )
{
	if( mReleased ){
		return;
	}

	mReleaseWatch.Start();
	mReleaseTimeout = timeOut;
}

long OGLArchiveRender::Camera::GetReleaseTimeout() const
{
	if( mReleased ){
		return 0;
	}

	return mReleaseTimeout;
}

bool OGLArchiveRender::Camera::IsReleased() const
{
	return mReleased;
}

int OGLArchiveRender::Camera::GetCameraID() const
{
	return mCameraID;
}

void OGLArchiveRender::Camera::OnUpdate()
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
OGLArchiveRender::OGLArchiveRender( boost::shared_ptr<CHCS::IStreamManager> streamManager ):
	mStreamManager( streamManager )
{
}

OGLArchiveRender::~OGLArchiveRender()
{
}
//////////////////////////////////////////////////////////////////////////
void OGLArchiveRender::GetAvailableCameras( std::vector<int>& camerasVec )
{
	CAUUID streamsUIDs;
	if( mStreamManager->GetAvailableStreams( &streamsUIDs ) )
	{
		boost::shared_ptr<void>( streamsUIDs.pElems, CoTaskMemFree );

		for( ULONG i = 0; i < streamsUIDs.cElems; ++i )
		{
			//Detect stream id
			CHCS::IStream* stream;
			if( mStreamManager->GetStreamByUID( &stream, &streamsUIDs.pElems[i] ) )
			{
				CHCS::IFrame* frame;
				if( stream->GetNextFrame( &frame, 100 ) ){
					int id = frame->GetSourceID();
					if( id != -1 ){
						camerasVec.push_back( id );
					}
				}
			}
		}
	}
}


void OGLArchiveRender::MakeActive()
{
	OGLRenderManager* rm = dllGetRenderManager();
	rm->SetActiveRender( OGLRenderManager::RM_ARCHIVE_RENDER );
}


bool OGLArchiveRender::IsActive() const
{
	OGLRenderManager* rm = dllGetRenderManager();
	if ( OGLRenderManager::RM_ARCHIVE_RENDER == rm->GetActiveRender() ){
		return true;
	}
	return false;
}


void OGLArchiveRender::SetRefreshPeriod( int period )
{
	OGLRenderManager* rm = dllGetRenderManager();
	rm->SetRefreshPeriod( OGLRenderManager::RM_ARCHIVE_RENDER, period );
}


int OGLArchiveRender::GetRefreshPeriod() const
{
	OGLRenderManager* rm = dllGetRenderManager();
	return rm->GetRefreshPeriod( OGLRenderManager::RM_ARCHIVE_RENDER );
}


void OGLArchiveRender::SeekArchive( INT64 position )
{
	double cameraNum = (double)mCameras.size();
	double averagePos = 0;

	//Calculate average position
	for( std::list<Camera*>::iterator it = mCameras.begin(); it != mCameras.end(); ++it ){
		averagePos += (double)((*it)->GetPosition()) / cameraNum;
	}

	INT64 newArchivePos = (INT64)averagePos;

	//Seek to new position
	for( std::list<Camera*>::iterator it = mCameras.begin(); it != mCameras.end(); ++it ){
		(*it)->Seek( newArchivePos );
	}
}

INT64 OGLArchiveRender::GetArchivePosition() const
{
	//Return average position of streams
	return 0;
}


IDVRArchiveRender::ICamera* OGLArchiveRender::GetCamera( int cameraIdx )
{
	return 0;

}


void OGLArchiveRender::SetArchiveTimeInterval( INT64 startTime, INT64 endTime )
{
	mArchiveStartTime = startTime;
	mArchiveEndTime = endTime;
}

void OGLArchiveRender::GetArchiveTimeInterval( INT64& startTime, INT64& endTime )
{
	startTime = mArchiveStartTime;
	endTime = mArchiveEndTime;
}

void OGLArchiveRender::BindCallback( boost::function0<void> fn )
{
	mCallbackFn = fn;
}

void OGLArchiveRender::UnbindCallback()
{
	mCallbackFn = 0;
}

//////////////////////////////////////////////////////////////////////////
void OGLArchiveRender::OnRenderUpdate()
{
	if( mCallbackFn ){
		mCallbackFn();
	}	
}

//////////////////////////////////////////////////////////////////////////
void OGLArchiveRender::OnCameraAdded( Camera* camera )
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

void OGLArchiveRender::OnCameraRemoved( Camera* camera )
{
	for( std::list<Camera*>::iterator it = mCameras.begin(); it != mCameras.end(); ++it )
	{
		//If camera already present
		if( *it == camera ){
			mCameras.erase( it );
		}
	}
}


void OGLArchiveRender::OnCameraUpdate()
{
	for( std::list<Camera*>::iterator it = mCameras.begin(); it != mCameras.end(); ++it )
	{
		(*it)->OnUpdate();
	}
}



//////////////////////////////////////////////////////////////////////////