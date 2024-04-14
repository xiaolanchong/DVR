#ifndef GLTEXTURE_G
#define GLTEXTURE_G

#include "GL/gl.h"
class GLTexture
{
public:
	enum TextureFormat
	{
		tf_rgb = 0,
		tf_rgba,
		tf_alpha		
	};

	enum DataFormat
	{
		df_rgb = 0,
		df_rgba,
		df_red,
		df_green,
    df_blue,
		df_alpha,
    df_luminance
	};


	GLTexture( unsigned int width, unsigned int height, TextureFormat format );
	
//	GLTexture( GLuint width, GLuint height, GLenum internalFormat, 
//		GLenum format, GLenum elemType, void* bytes );

	~GLTexture( ); 

	void WriteBytes( const void* bytes, DataFormat format );
	//void ReadBytes( void* bytes, DataFormat format );
	
	GLuint Width() const;
	GLuint Height() const;
	
	GLuint ID() const; 
	
	void Bind() const;
	void UnBind() const;

	TextureFormat Format() const
	{
		return m_textureFormat;
	}

	static GLenum ConvertToGLFormat( TextureFormat myFormat );
	static GLenum ConvertToGLFormat( DataFormat myFormat );
	static unsigned int BytesInPixel( TextureFormat myFormat );

private:
	
	TextureFormat m_textureFormat;

	GLuint width;
	GLuint height;
	GLuint id; 
	GLenum internalFormat;
	GLenum format;
	GLenum elemType;

	void Initialize( GLuint width, GLuint height, GLenum internalFormat, 
		GLenum format, GLenum elemType, void* bytes );

//////////////////////////////////////////////////////////////////////////

};

#endif