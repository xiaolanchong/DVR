//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейсы чтения/записи видеофайлов в произвольном формате

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   18.06.2006
*/                                                                                      //
//======================================================================================//
#include "chcs.h"
#include "VideoFileInterface.h"

#include "vfw/VFWFileWriter.h"
#include "vfw/VFWFileReader.h"

//////////////////////////////////////////////////////////////////////////

class VFWVideoFileFactory : public IVideoFileFactory
{
public:
	virtual std::auto_ptr<IVideoFileWriter>	
		CreateVideoFileWriter( LPCTSTR szFileName, const VideoFileSettings& FileSettings  )
	{
		return std::auto_ptr< IVideoFileWriter>( new VFWFileWriter( szFileName, FileSettings ) );
	}
	virtual std::auto_ptr<IVideoFileReader>	
		CreateVideoFileReader()
	{
		return std::auto_ptr<IVideoFileReader>();
	}
};

IVideoFileFactory*	GetVideoFileFactory()
{
	static VFWVideoFileFactory fct;
	return &fct;
}