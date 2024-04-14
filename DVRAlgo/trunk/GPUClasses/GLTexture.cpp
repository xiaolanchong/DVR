#include "stdafx.h"
#include "GLTexture.h"

GLTexture::GLTexture( unsigned int width_, unsigned int height_, TextureFormat format )
:m_textureFormat( format )
{
	Initialize( width_, height_, BytesInPixel( format) , ConvertToGLFormat( format ), GL_UNSIGNED_BYTE, 0 );
}

GLTexture::~GLTexture( )
{
	glDeleteTextures( 1, &id );
}

void GLTexture::Initialize( GLuint width_, GLuint height_, GLenum internalFormat_, 
 GLenum format_, GLenum elemType_, void* bytes )
{
	width = width_;
	height = height_;
	internalFormat = internalFormat_;
	format = format_;
	elemType = elemType_;

	glGenTextures( 1, &id ); 
	glBindTexture( GL_TEXTURE_RECTANGLE_NV, id );
	glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, internalFormat, width, height, 0, format, elemType, bytes );
	glTexParameteri( GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glBindTexture( GL_TEXTURE_RECTANGLE_NV, 0 );
}


void GLTexture::WriteBytes( const void* bytes, DataFormat format )
{
	glBindTexture( GL_TEXTURE_RECTANGLE_NV, id );
	glTexSubImage2D( GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, width, height, ConvertToGLFormat(format), elemType, bytes );
	glBindTexture( GL_TEXTURE_RECTANGLE_NV, 0 );
}

GLuint GLTexture::Width() const
{
	return width;
}

GLuint GLTexture::Height() const
{
	return height;
}

GLuint GLTexture::ID() const
{
	return id;
}

GLenum GLTexture::ConvertToGLFormat( TextureFormat myFormat )
{
	switch( myFormat ) 
	{
	case tf_rgb:
		return GL_BGR_EXT;
	case tf_rgba:
		return GL_BGRA_EXT;
	case tf_alpha:
		return GL_ALPHA;
	default:
		throw std::runtime_error( "unknown data format" );
	}
}

GLenum GLTexture::ConvertToGLFormat( DataFormat myFormat )
{
	switch( myFormat ) 
	{
	case df_rgb:
		return GL_BGR_EXT; 
	case df_rgba:
		return GL_BGRA_EXT;
	case df_red:
		return GL_RED;
	case df_green:
		return GL_GREEN;
	case df_blue:
		return GL_BLUE;
	case df_alpha:
		return GL_ALPHA;
	case df_luminance:
		return GL_LUMINANCE;
	default:
		throw std::runtime_error( "unknown data format" );
	}
}

unsigned int GLTexture::BytesInPixel( TextureFormat myFormat )
{
	switch( myFormat ) 
	{
	case tf_rgb:
		return 3;
	case tf_rgba:
		return 4;
	case tf_alpha:
		return 1;
	default:
		throw std::runtime_error( "unknown data format" );
	}
}

void GLTexture::Bind() const
{
	glBindTexture( GL_TEXTURE_RECTANGLE_NV, id );
	glEnable( GL_TEXTURE_RECTANGLE_NV );
}

void GLTexture::UnBind() const
{
	glDisable( GL_TEXTURE_RECTANGLE_NV );
	glBindTexture( GL_TEXTURE_RECTANGLE_NV, 0 );
}