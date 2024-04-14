#include "stdafx.h"
#include "GLframeBuffer.h"
#include <glh/glh_extensions.h>

GLFrameBuffer::GLFrameBuffer( )
{
}
GLFrameBuffer::~GLFrameBuffer( )
{
	glDeleteFramebuffersEXT( 1, &id );
}

void GLFrameBuffer::Initialize( GLuint width, GLuint height, GLenum internalFormat, 
		GLenum format, GLenum elemType, void* bytes )
{
	myTexture.Initialize( width, height, internalFormat, format, elemType, bytes );
	glGenFramebuffersEXT ( 1, &id );	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id ); 

	//may be bind the texture needed
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, targetTexture, myTexture.ID(), 0); 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0 ); 
}

void GLFrameBuffer::SetAsRenderTarget()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, id );
}

void GLFrameBuffer::ResetRenderTarget()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
	
void GLFrameBuffer::LoadBytes( void* bytes )
{
	myTexture.LoadBytes( bytes );
}

GLuint GLFrameBuffer::Width()
{
	return myTexture.Width();
}

GLuint GLFrameBuffer::Height()
{
	return myTexture.Height();
}

GLuint GLFrameBuffer::BufferID()
{	
	return id;
}


GLuint GLFrameBuffer::TextureID()
{
	return myTexture.ID();
}

GLuint GLFrameBuffer::InternalFormat()
{
	return myTexture.InternalFormat();
}

GLuint GLFrameBuffer::ElemType()
{
	return myTexture.ElemType();
}
