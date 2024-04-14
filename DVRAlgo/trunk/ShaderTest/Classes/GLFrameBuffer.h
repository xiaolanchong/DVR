#ifndef GLFRAMEBUFFER_H
#define GLFRAMEBUFFER_H

#include "GLTexture.h"

class GLFrameBuffer
{
public:
	GLFrameBuffer( );
	~GLFrameBuffer( ); 

	void Initialize( GLuint width, GLuint height, GLenum internalFormat, 
		GLenum format, GLenum elemType, void* bytes );

	void LoadBytes( void* bytes );

	GLuint Width();
	GLuint Height();
	GLuint BufferID();
	GLuint TextureID();
	GLuint InternalFormat();
	GLuint ElemType();		

	void SetAsRenderTarget();
	void ResetRenderTarget();

private:
	GLuint id;
	GLTexture myTexture;
};


#endif
