#include "stdafx.h"
#include "GLTexture.h"

GLTexture::GLTexture()
{
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
	glBindTexture( targetTexture, id );
	glTexImage2D( targetTexture, 0, internalFormat, width, height, 0, format, elemType, bytes );
	glTexParameteri( targetTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( targetTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glBindTexture( targetTexture, 0 );

}


void GLTexture::LoadBytes( void* bytes )
{
	glBindTexture( targetTexture, id );
	glTexSubImage2D( targetTexture, 0, 0, 0, width, height, format, elemType, bytes );
	glBindTexture( targetTexture, 0 );
}


GLuint GLTexture::Width()
{
	return width;
}

GLuint GLTexture::Height()
{
	return height;
}

GLuint GLTexture::ID()
{
	return id;
}
GLuint GLTexture::InternalFormat()
{
	return internalFormat;
}

GLuint GLTexture::ElemType()
{
	return elemType;
}

