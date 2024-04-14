#ifndef GLTEXTURE_G
#define GLTEXTURE_G

#include "GL/gl.h"
class GLTexture
{
public:
	GLTexture( );
	~GLTexture( ); 

	void Initialize( GLuint width, GLuint height, GLenum internalFormat, 
		GLenum format, GLenum elemType, void* bytes );

	void LoadBytes( void* bytes );
	
	GLuint Width();
	GLuint Height();
	GLuint ID();
	GLuint InternalFormat();
	GLuint ElemType();		

private:
	GLuint width;
	GLuint height;
	GLuint id; 
	GLenum internalFormat;
	GLenum format;
	GLenum elemType;

//////////////////////////////////////////////////////////////////////////

};

#endif