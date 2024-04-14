#pragma once

#include <boost\thread\mutex.hpp>
#include <glh/glh_extensions.h>

#define GLH_EXT_SINGLE_FILE

class GLExtensionInitializer
{
public:
	GLExtensionInitializer()
		: isInitialized( false ), initCount( 0 )
	{
	}
	void Initialize( const std::string& requredExtensions )
	{
		boost::mutex::scoped_lock lock( _mutex );
		initCount++;
		if( !isInitialized )
		{
			if( !glh_init_extensions( requredExtensions.c_str() ) )
			{
				throw std::exception("Necessary extensions were not supported" );
			}	
			isInitialized = true;
		}
	}

	void Uninitialize()
	{
		boost::mutex::scoped_lock lock( _mutex );
		if( initCount == 0 )
			throw std::runtime_error( "before uninitialize you need to initialize" );
		initCount--;
		if( isInitialized && initCount == 0 )
		{
			OutputDebugStringA( "Unitialized GL extensions\n" );
			glh_shutdown_extensions();
			isInitialized = false;
		}
	}

private:
	bool isInitialized;
	unsigned int initCount;
	boost::mutex _mutex;
};

GLExtensionInitializer& GetGLExtensionsInitializer();
