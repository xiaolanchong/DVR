#include "stdafx.h"
#include "CgProgram.h"

CgProgram::CgProgram()
{
}

void CgProgram::Init( const std::string& fileName, 
const std::string& mainName, const std::vector<std::string>& textureParams )
{
	// Create the context, this is a container for Cg programs
	myContext = cgCreateContext();
	if( myContext == 0 )
	{
		throw std::exception( "cannot create Cg context" );
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		myProgram = cgCreateProgramFromFile( myContext,  // The context where we want it added
			CG_SOURCE,    
			fileName.c_str(),    // The sourcefile
			CG_PROFILE_FP30,// The profile 
			mainName.c_str(),           // The entry function 
			NULL              // Compiler options
			);

		if( myProgram==NULL )
		{
			throw std::exception("cannot create program" );
		}
		else
		{
			// Now load the program
			cgGLLoadProgram( myProgram );
			
			getTextureParamsFromRunTime( textureParams );
		}
	}
}

CgProgram::~CgProgram()
{
}


void CgProgram::getTextureParamsFromRunTime( const std::vector<std::string>& textureParams )
{
	textureParamNames = textureParams;
	for( size_t i = 0; i < textureParams.size(); i++ )
	{
		CGparameter param = cgGetNamedParameter( myProgram, textureParams[i].c_str() );
		if( param != 0 )
			textureParamsToCgParams.insert( make_pair( textureParams[i], param ) );
		else
			throw( std::exception( " there is no such texture parameter: " ) );			
	}
}

void CgProgram::SetTextureParameter( const std::string& name, GLTexture& texture )
{
	textureParamsToTextures[ name ] = texture.ID();
}

void CgProgram::SetTextureParameter( const std::string& name, GLFrameBuffer& buffer )
{
	textureParamsToTextures[ name ] = buffer.TextureID();
}

void CgProgram::Bind()
{
	currentOutput.SetAsRenderTarget();
	glEnable( targetTexture );

	cgGLEnableProfile( CG_PROFILE_FP30 );
	
	//Set texture parameters
	for( int i = 0; i < textureParamNames.size(); ++i )
	{
		cgGLSetTextureParameter( textureParamsToCgParams[textureParamNames[i]], 
														 textureParamsToTextures[textureParamNames[i]] );
	}	

	// Bind the program
	cgGLBindProgram( myProgram );

	// enable texture parameters 
	for( int i = 0; i < textureParamNames.size(); ++i )
	{
		cgGLEnableTextureParameter( textureParamsToCgParams[textureParamNames[i]] ); 
	}	
}

void CgProgram::UnBind()
{
	// enable texture parameters 
	for( int i = 0; i < textureParamNames.size(); ++i )
	{
		cgGLEnableTextureParameter( textureParamsToCgParams[textureParamNames[i]] ); 
	}	

	// unbind the program
	cgGLBindProgram( 0 );
	
	glDisable( targetTexture );
	cgGLDisableProfile( CG_PROFILE_FP30 );

	currentOutput.ResetRenderTarget();
}

void CgProgram::SetOutputBuffer( const GLFrameBuffer&  buffer )
{
	currentOutput = buffer;
}

void CgProgram::RunProgram()
{
	Bind();
	glLoadIdentity();
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0); 
		glVertex2f(-1.0,  -1.0);
		glTexCoord2f(0, maxHeight); 
		glVertex2f( -1.0, 1.0);
		glTexCoord2f(maxWidth, maxHeight); 
		glVertex2f( 1.0, 1.0);
		glTexCoord2f(maxWidth, 0); 
		glVertex2f( 1.0, -1.0);
	}
	glEnd();
	UnBind();
}
