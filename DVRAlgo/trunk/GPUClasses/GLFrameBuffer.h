#ifndef GLFRAMEBUFFER_H
#define GLFRAMEBUFFER_H

#include "GLTexture.h"

class GLFrameBuffer
{
public:
	GLFrameBuffer( GLuint width, GLuint height, GLTexture::TextureFormat format );

	~GLFrameBuffer( ); 

	void Initialize( GLuint width, GLuint height, GLenum internalFormat, 
		GLenum format, GLenum elemType, void* bytes );

	void WriteBytes( const void* bytes, GLTexture::DataFormat format );
	void ReadBytes( void* bytes, GLTexture::DataFormat format ) const;
	void CopyFrom( const GLFrameBuffer& buffer );

	unsigned int Width() const;
	unsigned int Height() const;
	
	GLuint TextureID() const; 

	void BindBufferTexture() const;
	void UnBindBufferTexture() const;

	void SetAsRenderTarget() const;
	void ResetRenderTarget() const;

private:
	GLuint id;
	GLTexture myTexture;
};


#endif
