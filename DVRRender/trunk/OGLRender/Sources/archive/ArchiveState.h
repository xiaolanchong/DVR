//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Состояние архива - проигр-е, пауза, остановка

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   19.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_STATE_9797823428319564_
#define _ARCHIVE_STATE_9797823428319564_


namespace CHCS
{
	class	IStreamManager;
	struct	IStream;
	struct	IFrame;
}

typedef boost::shared_ptr<CHCS::IStream>						Stream_t;
typedef boost::shared_ptr<CHCS::IFrame>							Frame_t;
typedef std::map< int, Stream_t >								StreamMap_t;
typedef std::map< int, Frame_t >								FrameMap_t;

struct ArchiveCore
{

};

//======================================================================================//
//                                  class ArchiveState                                  //
//======================================================================================//

//! \brief Состояние архива - проигр-е, пауза, остановка
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   19.05.2006
//! \version 1.0
//! \bug 
//! \todo 

class ArchiveState
{
protected:
	std::pair<time_t, time_t>				m_WorkInterval;
	boost::shared_ptr<CHCS::IStreamManager>	m_StreamManager;
	StreamMap_t&							m_Streams;
	FrameMap_t&								m_Frames;

	std::string								m_StateName;

	ArchiveState(
		boost::shared_ptr<CHCS::IStreamManager> StreamManager, 
		StreamMap_t& Streams, 
		FrameMap_t& Frames,
		std::pair<time_t, time_t> WorkInterval,
		const char* szName);

	boost::optional<time_t> GetDefaultArchivePosition();
public:
	virtual ~ArchiveState();

	virtual void	AddCameras( const std::vector<int>& CameraIndexArr ) ;
	virtual void	RemoveCameras( const std::vector<int>& CameraIndexArr ) ;
	virtual bool	DrawFrame(  int CameraIndex, float x, float y, float width, float height 
							/*	std::vector<unsigned char>& UserFrameData*/   ) ;

	virtual void							Seek( time_t NewArchivePosition ) = 0;
	virtual void							OnUpdate() = 0;
	virtual void							PreDrawFrame(int nCameraID) = 0;

	virtual time_t GetArchivePosition() = 0;

	virtual boost::optional<time_t> GetCameraArchivePosition(int CameraID);

	virtual bool	GetFrameData( int CameraIndex, std::vector<unsigned char>& UserFrameData );

//	template <class T> std::auto_ptr<ArchiveState> CreateCopy( ) const
//	{
//		return std::auto_ptr<ArchiveState>(new T( m_StreamManager, m_Streams, m_Frames, m_WorkInterval ));
//	}
};

#endif // _ARCHIVE_STATE_9797823428319564_