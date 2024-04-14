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

#ifndef __STORAGE_HPP__
#define __STORAGE_HPP__

//#define USE_BOOST_POOL

class DirectoryList;
class FileList;

class Storage
{
	//! \class FileProxy
	//!	\brief
	class FileProxy
	{
	public:
		//! \function Constructor
		//! \brief FileProxy constructor
		FileProxy( const boost::filesystem::path& _path, size_t _size );
	
	public:
		//! \function operator<
		//! \brief Overloaded < operator, compares two files
		bool operator<( const FileProxy& rhs );

		//! \function operator==
		//! \brief Overloaded == operator, compares two files
		bool operator==( const FileProxy& rhs );

		//! \function operator!=
		//! \brief Overloaded != operator, compares two files
		bool operator!=( const FileProxy& rhs );
	public:
		//! \function SortByDatePred
		//! \brief SortByDatePred, sorting predicate, sorts by date ( exclude camera number )
		static bool SortByDatePred( const FileProxy& lhs, const FileProxy& rhs );

	public:
		//! \function GetPath
		//! \brief Returns file path
		const boost::filesystem::path& GetPath() const;

		//! \function GetDate
		//! \brief Returns file date 
		const boost::posix_time::ptime& GetDate() const;

		//! \function GetPathAsString
		//! \brief Returns file path as string
		std::string GetPathAsString() const;

		//! \function GetSize
		//! \brief Gets file size
		size_t GetSize() const;

		//! \function GetCamID
		//! \brief Returns camera ID
		int GetCamID() const;

	private:
		boost::filesystem::path mPath;
		boost::posix_time::ptime mFileTime;
		size_t mSize;
		int mCameraID;
	};


#ifdef USE_BOOST_POOL
	typedef std::list< boost::filesystem::directory_iterator, boost::pool_allocator<boost::filesystem::directory_iterator> > DirectoryList;	
	typedef std::list< FileProxy, boost::pool_allocator<FileProxy> > FileList;
#else
	typedef std::list< boost::filesystem::path> DirectoryList;	
	typedef std::list< FileProxy> FileList;
#endif

public:
	Storage( const std::string& path, boost::shared_ptr<Elvees::IMessage> message );
	virtual ~Storage();
public:
//	void SetStopEvent()


	//! function PerformClean
	//! brief Perfroms a cleaning operation, if storage free space balance or lastTime conditions were achieved
	void PerformClean( std::vector<int>& camIds, double backupStart/*, double backupEnd*/, boost::posix_time::ptime& lastDate  );
private:
	//!	function GetInfo
	//!	brief Scans storage for files and folder
	void GetStorageInfo( std::vector<int>& camIds, DirectoryList& directoryList, FileList& fileList, int& totalFileSize );

	//! function RemoveEmptyDirs
	//! brief Remove empty directories
	void RemoveEmptyDirs(  );

	//! function GetStorageSpace
	//! brief Returns the storage total space and free space 
	void GetStorageSpace( long long& totalSpace, long long& freeSpace ) const;

	//! function GetAvailableSpaceBalance
	//! brief Returns storage free space to total space ratio
	double GetAvailableSpaceBalance() const;


	void	CollectDataFromCameraDirectory( 
		const boost::filesystem::path& CameraDirectory, 
		DirectoryList& directoryList, 
		FileList& fileList, 
		int& totalFileSize 
	);
	void	CollectDataFromDayDirectory( 
		const boost::filesystem::path& DayDirectory, 
		DirectoryList& directoryList, 
		FileList& fileList, 
		int& totalFileSize 
	);

	void    TryRemoveFilesForDiskSpace(  double backupStartDiskSpacePercentage  );
	void	RemoveFilesForDiskSpace( double backupStartDiskSpacePercentage );
	void	RemoveFilesForTimeExpired( const boost::posix_time::ptime& lastDate);
	void	RemoveFile( const FileProxy& FileInformation );
	void	FakeRemoveFile( const FileProxy& FileInformation );
private:
	boost::shared_ptr<Elvees::IMessage> mMessage;
	std::string							mStoragePath;
	size_t								m_storageCleaningCyclesCount;

	std::string GetStoragePathForDiskSpace( ) const;
	std::string GetStoragepathForFilesystem() const;
};


#endif //__STORAGE_HPP__
