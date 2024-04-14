#include "stdafx.h"
#include "GLframeBuffer.h"

#define GLH_EXT_SINGLE_FILE
#include <glh/glh_extensions.h>

GLFrameBuffer::GLFrameBuffer( GLuint width, GLuint height, GLTexture::TextureFormat format )
: myTexture( width, height, format )
{
	glGenFramebuffersEXT ( 1, &id );	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id ); 

	//may be bind the texture needed
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_NV, myTexture.ID(), 0); 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0 ); 
}
GLFrameBuffer::~GLFrameBuffer( )
{
/*	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id ); 

	//may be bind the texture needed
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_NV, 0, 0); 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0 ); 
*/
	glDeleteFramebuffersEXT( 1, &id );
}

/*void GLFrameBuffer::Initialize( GLuint width, GLuint height, GLenum internalFormat, 
		GLenum format, GLenum elemType, void* bytes )
{
	glGenFramebuffersEXT ( 1, &id );	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id ); 

	//may be bind the texture needed
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_NV, myTexture.ID(), 0); 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0 ); 
}*/

void GLFrameBuffer::SetAsRenderTarget() const
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, id );
}

void GLFrameBuffer::ResetRenderTarget() const
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
	
void GLFrameBuffer::WriteBytes( const void* bytes, GLTexture::DataFormat format )
{
	myTexture.WriteBytes( bytes, format );
}


void GLFrameBuffer::ReadBytes( void* bytes, GLTexture::DataFormat format ) const
{
//	void* _pPixels = glMapBuffer( 
//		GL_PIXEL_PACK_BUFFER_EXT, GL_READ_ONLY
//		); 

	// copy the pixels from the pbo into memory here 

	//glUnmapBufferARB(GL_FRAMEBUFFER_EXT); 

// OLD

	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, id );
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( 0, 0, Width(), Height(), GLTexture::ConvertToGLFormat( format), GL_UNSIGNED_BYTE, bytes );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	
}


void GLFrameBuffer::CopyFrom( const GLFrameBuffer& buffer )
{
	SetAsRenderTarget();//bind frame buffer
	buffer.BindBufferTexture();
/*
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
*/
//	glEnable( targetTexture );
//	glBindTexture( targetTexture, textureID );

	GLint oldViewPort[4];
	glGetIntegerv( GL_VIEWPORT, oldViewPort );
	glViewport( 0, 0, Width(), Height() );

	glColor3f(1.0, 1, 1);

	const float fraction = 1.0f;
	glBegin(GL_QUADS);
	{
		glTexCoord2i( 0, 0 ); 
		glVertex2f( -fraction,  -fraction );
		glTexCoord2i( 0, buffer.Height() ); 
		glVertex2f( -fraction, fraction );
		glTexCoord2i( buffer.Width(), buffer.Height() ); 
		glVertex2f( fraction, fraction );
		glTexCoord2i( buffer.Width(), 0 ); 
		glVertex2f( fraction, -fraction );
	}
	glEnd();

	glViewport( oldViewPort[0], oldViewPort[1], oldViewPort[2], oldViewPort[3] );


	buffer.UnBindBufferTexture();	
	ResetRenderTarget();
}

GLuint GLFrameBuffer::Width() const
{
	return myTexture.Width();
}

GLuint GLFrameBuffer::Height() const
{
	return myTexture.Height();
}

GLuint GLFrameBuffer::TextureID() const
{
	return myTexture.ID();
}


void GLFrameBuffer::BindBufferTexture() const
{
	myTexture.Bind();
}

void GLFrameBuffer::UnBindBufferTexture() const
{
	myTexture.UnBind();
}

