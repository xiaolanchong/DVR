//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Реализация видеограббера для сервера - адаптер для CHCSVA

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.01.2006
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ServerVideoReader.h"

//======================================================================================//
//                               class ServerVideoReader                                //
//======================================================================================//



ServerVideoReader::ServerVideoReader( boost::shared_ptr<CHCS::IStreamManager> pManager, int nCameraID )
{
	CHCS::IStream*	pStream;
	bool res = pManager->GetStreamByID( &pStream, nCameraID );
	if(!res || !pStream )
	{
		throw ServerVideoReaderException( (boost::format("No such camera #%d") % nCameraID).str() );
	}
	m_pStream = boost::shared_ptr<CHCS::IStream>( pStream, ReleaseInterface<CHCS::IStream>() );

	//! попытаемся 32 раза вытянуть кадр для взятия заголовка
	CHCS::IFrame * pFrame = 0;
	int attempt = 32;
	while ((attempt-- > 0) && !(m_pStream->GetNextFrame( &pFrame, 200 )))
	{
//		Sleep(300);
	}
	if( (attempt > 0) && (pFrame != 0) )
	{
		m_pFrame = boost::shared_ptr<CHCS::IFrame>( pFrame, ReleaseInterface<CHCS::IFrame>() );

		_ASSERT( m_pFrame->GetFormat(0) == sizeof(BITMAPINFOHEADER) );
		m_pFrame->GetFormat( reinterpret_cast<BITMAPINFO*>( &m_BmpHdr ) );
		m_pFrame.reset();
	}
	else
	{
		throw ServerVideoReaderException( (boost::format("Failed to get a frame from the camera #%d") % nCameraID).str());
	}
}

ServerVideoReader::~ServerVideoReader()
{
}

const BITMAPINFOHEADER &	ServerVideoReader::GetHeader() const
{
	return m_BmpHdr;
}

bool IsSignalDetected( CHCS::IFrame* pFrame )
{
	return	pFrame && 
			(pFrame->IsSignalLocked() > 0 );
}

bool ServerVideoReader::Read( const ubyte *& pBytes, ulong & time )
{
	pBytes = 0;
	time = 0;
	m_pFrame.reset();

	CHCS::IFrame* pFrame = 0;

	//! 1000 мсек таймаута
	bool res = m_pStream->GetNextFrame( &pFrame, 1000 );

	if( !res || !pFrame ) return false;

	m_pFrame = boost::shared_ptr<CHCS::IFrame>( pFrame, ReleaseInterface<CHCS::IFrame>() );
	if( pFrame )
	{
		pBytes	= reinterpret_cast<const boost::uint8_t*>( pFrame->GetBytes() ) ;
		time	= pFrame->GetRelativeTime();
		return pBytes != 0 && IsSignalDetected(pFrame);	
	}
	else
	{

		pBytes	= 0;
		time	= 0;
		return false;
	}
}

bool ServerVideoReader::SetUserDataToVideo( const void* pUserData, size_t UserDataSize )
{
	return m_pStream->SetCustomFrameData( pUserData, UserDataSize );
}