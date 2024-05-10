//!
//!	DVR 
//!	Copyright(C) 2006 Elvees
//!	All rights reserved.
//!
//!	\file     Storage.hpp		
//!	\date     2006-02-15		
//!	\author   Yanakov Leonid		
//!	\version  1.0	
//!	\brief    Helper class for working with hd-storage

#include "StdAfx.hpp"
#include "Storage.hpp"

//////////////////////////////////////////////////////////////////////////
// 00\01-02-03\01-02-03.avi == 20B
static boost::regex gPartialPathExpression( "(\\d+)/(\\d{2})-(\\d{2})-(\\d{2})/(\\d{2})-(\\d{2})-(\\d{2})\\.avi$" );

//////////////////////////////////////////////////////////////////////////
Storage::FileProxy::FileProxy(const boost::filesystem::path& _path, size_t _size):
	mPath( _path ), mSize( _size ) 
{
	int year;	
	int month;	
	int day;	
	int hour;	
	int minute;
	int second;

	boost::cmatch what;
	if( 0 != boost::regex_search( mPath.string().c_str(),  what, gPartialPathExpression ) )
	{
		mCameraID	= atoi(what[1].str().c_str() );
		year		= atoi(what[2].str().c_str() );
		month		= atoi(what[3].str().c_str() );
		day		= atoi(what[4].str().c_str() );
		hour		= atoi(what[5].str().c_str() );
		minute		= atoi(what[6].str().c_str() );
		second		= atoi(what[7].str().c_str() );

		//Construct file time
		mFileTime = boost::posix_time::ptime( 
			boost::gregorian::date( year + 2000, month, day ),
			boost::posix_time::time_duration( hour, minute, second ) );
	}
	else
	{
		mCameraID	= -1;
		year		= -1;	
		month		= -1;
		day			= -1;
		hour		= -1;
		minute		= -1;
		second		= -1;
	}
}

bool Storage::FileProxy::operator<( const FileProxy& rhs )
{
	//Camera ID
	if( mCameraID < rhs.mCameraID )
		return true;
	else if( mCameraID > rhs.mCameraID )
		return false;

	if( mFileTime < rhs.mFileTime )
		return true;
	return false;
}

bool Storage::FileProxy::operator==( const FileProxy& rhs )
{
	if( ( mCameraID == rhs.mCameraID ) 
		&& ( mFileTime == rhs.mFileTime ) )
		return true;
	
	return false;
}

bool Storage::FileProxy::operator!=( const FileProxy& op )
{
	return !operator==( op );	
}

const boost::filesystem::path& Storage::FileProxy::GetPath() const
{
	return mPath;
}

std::string Storage::FileProxy::GetPathAsString() const
{
	//Format string
	return boost::str( 
		boost::format( "Cam:%d %s" ) 
			% mCameraID
			% boost::posix_time::to_simple_string( mFileTime ) );
}

int Storage::FileProxy::GetCamID() const
{
	return mCameraID;
}

size_t Storage::FileProxy::GetSize() const
{
	return mSize;
}

const boost::posix_time::ptime& Storage::FileProxy::GetDate() const
{
	return mFileTime;
}

//////////////////////////////////////////////////////////////////////////
bool Storage::FileProxy::SortByDatePred( const FileProxy& lhs, const FileProxy& rhs )
{
	if( lhs.mFileTime < rhs.mFileTime )
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Storage::Storage( const std::string& path, boost::shared_ptr<Elvees::IMessage> message ):
	mStoragePath( path ), mMessage( message ),
	m_storageCleaningCyclesCount(0)
{
	//Left blank
}

Storage::~Storage()
{
	//Left blank
}
//////////////////////////////////////////////////////////////////////////
void Storage::GetStorageSpace( long long& totalSpace, long long& freeSpace ) const
{
	wxLongLong wxFreeSpace;
	wxLongLong wxTotalSpace;

	//If GetStorageSpace failed, throw an exception
	if( !::wxGetDiskSpace( GetStoragePathForDiskSpace().c_str(), &wxTotalSpace, &wxFreeSpace) )
	{
		//TODO: Throw exception here
	}

	totalSpace = wxTotalSpace.GetValue();
	freeSpace = wxFreeSpace.GetValue();
}

double Storage::GetAvailableSpaceBalance() const
{
	long long totalSpace;
	long long freeSpace;

	//Get disk space parameters
	GetStorageSpace( totalSpace, freeSpace );

	return 100.0 * static_cast<double>(freeSpace) / static_cast<double>(totalSpace);
}

using boost::filesystem::path;
using boost::filesystem::directory_iterator;
//using boost::filesystem::path::iterator;

void	GetAllSubdirectories( const path& dir, std::list< path >& AllSubdirs )
{
	directory_iterator directoryIt		= directory_iterator(dir);		
	directory_iterator directoryEndIt;	//end iterator

	for( ; directoryIt != directoryEndIt; ++directoryIt )
	{
		if ( boost::filesystem::is_directory( *directoryIt ) ) 
		{
			AllSubdirs.push_back( *directoryIt ) ;
		}
	}
}

void	GetAllFilesInDirectory( const path& dir, std::list< path >& AllFilePath )
{
	directory_iterator directoryIt		= directory_iterator(dir);		
	directory_iterator directoryEndIt;	//end iterator

	for( ; directoryIt != directoryEndIt; ++directoryIt )
	{
		if (! boost::filesystem::is_directory( *directoryIt ) ) 
		{
			AllFilePath.push_back( *directoryIt ) ;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void Storage::GetStorageInfo( std::vector<int>& camIds, DirectoryList& directoryList, FileList& fileList, int& totalFileSize )
{
	try
	{
		int fileSize = 0;

		//Storage path
		boost::filesystem::path storagePath( GetStoragepathForFilesystem()/*, &boost::filesystem::native*/);

		boost::filesystem::directory_iterator directoryIt;		
		boost::filesystem::directory_iterator directoryEndIt;	//end iterator

		//Add path to the parent directory 
		//directoryList.push_back( boost::filesystem::directory_iterator(storagePath) );	

		std::list< path > AllDirCameraLevel;
		GetAllSubdirectories( storagePath, AllDirCameraLevel );

		for( std::list< path >::iterator it = AllDirCameraLevel.begin(); it != AllDirCameraLevel.end(); ++it )
		{
			CollectDataFromCameraDirectory( *it, directoryList, fileList, totalFileSize );
		}

	}
	catch( boost::filesystem::filesystem_error& e )
	{
		std::string errorMessage = 
			boost::str( boost::format( "FileSystem error has occured, %s" ) % e.what() );
		mMessage->Print( Elvees::IMessage::mt_error, errorMessage.c_str() );
	}
	catch( ... )
	{
		mMessage->Print( Elvees::IMessage::mt_error, "An unknown error has occured" );
	}
}

void	Storage::CollectDataFromCameraDirectory( const path& CameraDirectory, DirectoryList& directoryList, FileList& fileList, int& totalFileSize )
try
{
	std::list< path > AllDirDayLevel;
	GetAllSubdirectories( CameraDirectory, AllDirDayLevel );
	directoryList.insert( directoryList.end(), AllDirDayLevel.begin(), AllDirDayLevel.end() );

	for ( std::list< path >::iterator it = AllDirDayLevel.begin(); it != AllDirDayLevel.end(); ++it )
	{
		CollectDataFromDayDirectory( *it, directoryList, fileList, totalFileSize );
	}
}
catch( boost::filesystem::filesystem_error& e )
{
	std::string errorMessage = 
		boost::str( boost::format( "FileSystem error has occured, %s" ) % e.what() );
	mMessage->Print( Elvees::IMessage::mt_error, errorMessage.c_str() );
}
catch( ... )
{
	mMessage->Print( Elvees::IMessage::mt_error, "An unknown error has occured" );
};

void	Storage::CollectDataFromDayDirectory( 
					const boost::filesystem::path& DayDirectory, 
					DirectoryList& directoryList, 
					FileList& fileList, 
					int& totalFileSize
)
try
{
	std::list< path > AllVideoFilesForThisDay;
	GetAllFilesInDirectory( DayDirectory, AllVideoFilesForThisDay );
	std::list< path >::iterator it;
	for( it = AllVideoFilesForThisDay.begin(); it != AllVideoFilesForThisDay.end(); ++it )
	{
		path dir =  *it;
		if( !boost::filesystem::symbolic_link_exists( dir ) )
		{
			int fileSize = boost::filesystem::file_size( dir );

			FileProxy fp( dir, fileSize );
			{
				fileList.push_back( FileProxy( dir, fileSize ) );
			}
			totalFileSize += fileSize;
		}
	}
}
catch( boost::filesystem::filesystem_error& e )
{
	std::string errorMessage = 
		boost::str( boost::format( "FileSystem error has occured, %s" ) % e.what() );
	mMessage->Print( Elvees::IMessage::mt_error, errorMessage.c_str() );
}
catch( ... )
{
	mMessage->Print( Elvees::IMessage::mt_error, "An unknown error has occured" );
};

void Storage::PerformClean( 
						   std::vector<int>& camIds, 
						   double backupStart,
						   boost::posix_time::ptime& lastDate 
						   )
try
{
//	static int storageCleaningCyclesCount;

	wxStopWatch sw;

	const double storageSpaceBalance = GetAvailableSpaceBalance();
#if 0
	backupStart = 99;
#endif
	//Cleaning by free space on the storage
	if( ( m_storageCleaningCyclesCount % 50 ) == 0 )
	{
		TryRemoveFilesForDiskSpace( backupStart );
	}

	//Cleaning by date on the storage
	if( ( m_storageCleaningCyclesCount % 50 ) == 0 )
	{
		RemoveFilesForTimeExpired( lastDate );
	}

	//Each -th cycle, empty directories procedure begins
	if( ( m_storageCleaningCyclesCount % 100 ) == 0 )
	{
		RemoveEmptyDirs( /*directories*/ );
	}

	//Increment storage cleaning cycle	
	++m_storageCleaningCyclesCount;

	sw.Pause();
#if 0
	if( ( m_storageCleaningCyclesCount % 4 ) == 0 )
	{
		std::string timeElapsed =
			boost::str( boost::format( "PerformClean completed, time elapsed: %d\n" ) % sw.Time() );
		mMessage->Print( Elvees::IMessage::mt_debug_info, timeElapsed.c_str()  );
	}
#endif
}
catch( boost::filesystem::filesystem_error& e )
{
	std::string errorMessage = 
		boost::str( boost::format( "FileSystem error has occured, %s" ) % e.what() );
	mMessage->Print( Elvees::IMessage::mt_error, errorMessage.c_str() );
};

void Storage::TryRemoveFilesForDiskSpace(  double backupStartDiskSpacePercentage  )
{
	const double FreeDiskSpace = GetAvailableSpaceBalance();
	std::string InfoMessage;
	 InfoMessage = 
		boost::str( boost::format( "Minimal disk free space=%.1f%%" ) % backupStartDiskSpacePercentage );
	 mMessage->Print( Elvees::IMessage::mt_info, InfoMessage.c_str() );
	 InfoMessage = 
		boost::str( boost::format( "Current disk free space=%.1f%%" ) % FreeDiskSpace );
	 mMessage->Print( Elvees::IMessage::mt_info, InfoMessage.c_str() );

	if( FreeDiskSpace < backupStartDiskSpacePercentage )
	{
		RemoveFilesForDiskSpace( backupStartDiskSpacePercentage );
	}
}

void	Storage::RemoveFilesForDiskSpace( double backupStartDiskSpacePercentage )
try
{
	std::string InfoMessage = 
		boost::str( boost::format( "Try to remove files until minimal disk free space=%.1f%%" ) % backupStartDiskSpacePercentage );
	mMessage->Print( Elvees::IMessage::mt_info, InfoMessage.c_str() );

	std::vector<int> camIds;
	FileList files;				//Files list;
	DirectoryList directories;	//Directory list
	int totalFileSize;
	//Get storage info
	GetStorageInfo( camIds, directories, files, totalFileSize );

	//Sort files by date
	files.sort( Storage::FileProxy::SortByDatePred );

	//Remove files
	for( FileList::iterator it = files.begin(); 
		( it != files.end() )&& 
		( backupStartDiskSpacePercentage >= GetAvailableSpaceBalance() ); 
		++it )
	{
#if 1
		RemoveFile
#else
		FakeRemoveFile
#endif
			( *it );
	}
}
catch( boost::filesystem::filesystem_error& e )
{
	std::string errorMessage = 
		boost::str( boost::format( "FileSystem error has occured, %s" ) % e.what() );
	mMessage->Print( Elvees::IMessage::mt_error, errorMessage.c_str() );
};

void	Storage::RemoveFile( const FileProxy& FileInformation )
{
	//Debug output
	std::string fileStr =
		boost::str( boost::format( "Attempting to delete file: %s" ) % FileInformation.GetPathAsString() );
	mMessage->Print( Elvees::IMessage::mt_info, fileStr.c_str()  );

	//Remove file
	boost::filesystem::remove( FileInformation.GetPath() );
}

void	Storage::FakeRemoveFile( const FileProxy& FileInformation )
{
	std::string fileStr =
		boost::str( boost::format( "Fake attempt to delete file: %s" ) % FileInformation.GetPathAsString() );
	mMessage->Print( Elvees::IMessage::mt_info, fileStr.c_str()  );
}

void	Storage::RemoveFilesForTimeExpired( const boost::posix_time::ptime& lastDate)
try
{
	std::string InfoMessage = 
		boost::str( boost::format( "Storing the archive from=%s" ) % to_simple_string(lastDate) );
	mMessage->Print( Elvees::IMessage::mt_info, InfoMessage.c_str() );

	std::vector<int> camIds;
	FileList files;				//Files list;
	DirectoryList directories;	//Directory list
	int totalFileSize;
	//Get storage info
	GetStorageInfo( camIds, directories, files, totalFileSize );

	//Sort files by date
	files.sort( Storage::FileProxy::SortByDatePred );

	for( FileList::iterator it = files.begin(); 
		( it != files.end() )
		&& ( it->GetDate() <= lastDate ) ; ++it )
	{
		//Debug output
		std::string fileStr =
			boost::str( boost::format( "Attempting to delete file: %s\n" ) % it->GetPathAsString() );
		mMessage->Print( Elvees::IMessage::mt_info, fileStr.c_str()  );

		//Remove file
		boost::filesystem::remove( it->GetPath() );
	}
}
catch( boost::filesystem::filesystem_error& e )
{
	std::string errorMessage = 
		boost::str( boost::format( "FileSystem error has occured, %s" ) % e.what() );
	mMessage->Print( Elvees::IMessage::mt_error, errorMessage.c_str() );
}

void Storage::RemoveEmptyDirs( )
{
	std::string InfoMessage = 
		boost::str( boost::format( "Removing empty directories" ) );
	mMessage->Print( Elvees::IMessage::mt_info, InfoMessage.c_str() );

	std::vector<int> camIds;
	FileList files;				//Files list;
	DirectoryList directoryList;	//Directory list
	int totalFileSize;
	//Get storage info
	GetStorageInfo( camIds, directoryList, files, totalFileSize );

	boost::filesystem::directory_iterator directoryIt;		
	boost::filesystem::directory_iterator directoryEndIt;

	for( DirectoryList::iterator it = directoryList.begin(); it != directoryList.end(); ++it )
	{
		int totalFiles = 0;
	
		for( directoryIt = boost::filesystem::directory_iterator(*it); 
			  directoryIt != directoryEndIt; ++directoryIt )
		{
			if(  
				  boost::filesystem::symbolic_link_exists( *directoryIt ) || 
				! boost::filesystem::is_directory( *directoryIt ) 
				)
			{
				++totalFiles;

				//Skip symbolic links
				continue;
			}				
		}

		//If directory hasn't files, remove it
		if( totalFiles == 0 )
		{
			std::string message = 
				boost::str( boost::format( "Attempting to delete empty directory: %s" ) % (*it).string()  );
			
			mMessage->Print( Elvees::IMessage::mt_info, message.c_str()  );
			boost::filesystem::remove( *it );
		}
	}
}

std::string Storage::GetStoragePathForDiskSpace( ) const
{
	return mStoragePath;
}

std::string Storage::GetStoragepathForFilesystem() const
{
#ifdef WIN32
	
#else
#error
#endif
	std::string s(mStoragePath);
//	boost::algorithm ::replace_all( s, std::string("\\"), std::string("/") );
	return  s;
}

//////////////////////////////////////////////////////////////////////////