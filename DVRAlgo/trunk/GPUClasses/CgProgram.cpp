#include "stdafx.h"
#include "CgProgram.h"

const CGprofile currentProfile = CG_PROFILE_FP40;

CgProgram::CgProgram( const std::string& fileName, 
const std::string& mainName, const std::vector<std::string>& textureParams )
{
	Init( fileName, mainName, textureParams );
}

void CgProgram::Init( const std::string& fileName, 
const std::string& mainName, const std::vector<std::string>& textureParams )
{
	// Create the context, this is a container for Cg programs
	myContext = cgCreateContext();
	if( myContext == 0 )
	{
		throw std::runtime_error ( "cannot create Cg context" );
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		myProgram = cgCreateProgramFromFile( myContext,  // The context where we want it added
			CG_SOURCE,    
			fileName.c_str(),    // The sourcefile
			currentProfile,// The profile 
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
	currentOutput->SetAsRenderTarget();
	glEnable( GL_TEXTURE_RECTANGLE_NV );

	cgGLEnableProfile( currentProfile );
	
	//Set texture parameters
	for( size_t i = 0; i < textureParamNames.size(); ++i )
	{
		cgGLSetTextureParameter( textureParamsToCgParams[textureParamNames[i]], 
														 textureParamsToTextures[textureParamNames[i]] );
	}	

	// Bind the program
	cgGLBindProgram( myProgram );

	// enable texture parameters 
	for( size_t i = 0; i < textureParamNames.size(); ++i )
	{
		cgGLEnableTextureParameter( textureParamsToCgParams[textureParamNames[i]] ); 
	}	
}

void CgProgram::UnBind()
{
	// enable texture parameters 
	for( size_t i = 0; i < textureParamNames.size(); ++i )
	{
		cgGLEnableTextureParameter( textureParamsToCgParams[textureParamNames[i]] ); 
	}	

	// unbind the program
	cgGLUnbindProgram( currentProfile );
	
	glDisable( GL_TEXTURE_RECTANGLE_NV );
	cgGLDisableProfile( currentProfile );

	currentOutput->ResetRenderTarget();
}

void CgProgram::SetOutputBuffer( GLFrameBuffer&  buffer )
{
	currentOutput = &buffer;
}

void CgProgram::RunProgram()
{
	Bind();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	GLint oldViewPort[4];
	glGetIntegerv( GL_VIEWPORT, oldViewPort );

	glViewport( 0, 0, currentOutput->Width(), currentOutput->Height() );
	glBegin(GL_QUADS);
	{
		glTexCoord2i(0, 0); 
		glVertex2f(-1.0,  -1.0);
		glTexCoord2i(0, currentOutput->Height() ); 
		glVertex2f( -1.0, 1.0);
		glTexCoord2i( currentOutput->Width(),  currentOutput->Height() ); 
		glVertex2f( 1.0, 1.0);
		glTexCoord2i( currentOutput->Width(), 0); 
		glVertex2f( 1.0, -1.0);
	}
	glEnd();

	glViewport( oldViewPort[0], oldViewPort[1], oldViewPort[2], oldViewPort[3] );
	glPopMatrix();

	UnBind();
}
