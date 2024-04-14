//*********************************************************
//GLFONT.CPP -- glFont routines
//Copyright (c) 1998 Brad Fish
//See glFont.txt for terms of use
//November 10, 1998
//*********************************************************

#include "StdAfx.h"
#include "OGLFont.hpp"

//*********************************************************
//Variables
//*********************************************************

//Current font
//const GLFONT *glFont;

//*********************************************************
//Functions
//*********************************************************
GLFont::GLFont( const char *FileName)
{
/*	FILE *Input;
	char *TexBytes;
	size_t Num;

	//Open font file
	if ((Input = fopen(FileName, "rb")) == NULL)
	{
		return false;
	}

	fseek( Input, SEEK_END , 0 );
	long nFileSize = ftell( Input );
	fseek( Input, SEEK_SET, 0 );
	
	unsigned char * pFileContent = malloc( nFileSize );

	if( !pFileContent )
	{
		return FALSE;
	}

	//Clean up
	free(TexBytes);
	fclose(Input);

	//Return pointer to new font
	return TRUE;*/
}

GLFont::GLFont( const void* pData, size_t nDataSize)
{
	glGenTextures( 1, &m_OpenGLTextureName );

	size_t				 nCurBufferSize = nDataSize;
	const unsigned char* pCurBuffer		= (const unsigned char*)pData;
	if( nDataSize <= sizeof(GLFONT) )
	{
		throw std::runtime_error("Input data size is too small");
	}
	memcpy( &m_FontHdr, pCurBuffer, sizeof(GLFONT) );
	pCurBuffer		+= sizeof(GLFONT);
	nCurBufferSize	-= sizeof(GLFONT);

	//Get number of characters
	size_t CharNumber = m_FontHdr.IntEnd - m_FontHdr.IntStart + 1;

	//Allocate memory for characters
	m_CharDataArr.resize( CharNumber );

	if( CharNumber * sizeof(GLFONTCHAR) > nDataSize  )
	{
		throw std::runtime_error("Input data size is too small");
	}
	//Read glFont characters
	memcpy( &m_CharDataArr[0], pCurBuffer, sizeof(GLFONTCHAR) * CharNumber);

	pCurBuffer		+= sizeof(GLFONTCHAR) * CharNumber;
	nCurBufferSize	-= sizeof(GLFONTCHAR) * CharNumber;

	//Get texture size
	const size_t TextDataSize = m_FontHdr.TexWidth * m_FontHdr.TexHeight * 2;

	//Allocate memory for texture data
	if (nCurBufferSize < TextDataSize )
	{
		throw std::runtime_error("Not enough char data");
	}

	glBindTexture( GL_TEXTURE_2D, m_OpenGLTextureName );
	//Create texture
	glTexImage2D( 
		GL_TEXTURE_2D, 0, 2, 
		m_FontHdr.TexWidth, m_FontHdr.TexHeight, 
		0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
		(void*)pCurBuffer);

}

GLFont::~GLFont()
{
	glDeleteTextures( 1, &m_OpenGLTextureName );
}

//*********************************************************

void GLFont::PrepareForTextOut( ) const
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Bind to font texture
	glBindTexture(GL_TEXTURE_2D, m_OpenGLTextureName);	
 
	//! установить параметры текстурирования - фильтрация, наложения с цветом
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);  
}

void GLFont::ClearAfterTextOut() const
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

//*********************************************************
void GLFont::TextOut (const char *String, float x, float y, const GLFont::Color& TextColor) const
{
	//Get length of string
	const size_t Length = strlen(String);
	const float z = 0.0f;
	
	//! set text color
	glColor4f( TextColor.r, TextColor.g, TextColor.b, TextColor.a );
	//Begin rendering quads
	glBegin(GL_QUADS);
	
	//Loop through characters
	for (size_t i = 0; i < Length; i++)
	{
		unsigned char TextSymbol = String[i];
		if (TextSymbol < m_FontHdr.IntStart || TextSymbol > m_FontHdr.IntEnd )
		{
			break;
		}
		//Get pointer to glFont character
		const GLFONTCHAR* Char = &m_CharDataArr[ TextSymbol - (unsigned)m_FontHdr.IntStart];
	
		//Specify vertices and texture coordinates
		glTexCoord2f(Char->tx1, Char->ty1); glVertex3f(x, y, z);
		glTexCoord2f(Char->tx1, Char->ty2); glVertex3f(x, y - Char->dy, z);
		glTexCoord2f(Char->tx2, Char->ty2);	glVertex3f(x + Char->dx, y - Char->dy, z);
		glTexCoord2f(Char->tx2, Char->ty1);	glVertex3f(x + Char->dx, y, z);
	
		//Move to next character
		x += Char->dx;
	}

	//Stop rendering quads
	glEnd();
}

std::pair<int, int> GLFont::GetCharSize( unsigned char cSym) const
{
	//Make sure in range
	if (cSym < m_FontHdr.IntStart || cSym >  m_FontHdr.IntEnd )
	{
		return std::make_pair(0, 0);
	}
	else
	{
		const GLFONTCHAR *pCharData = &m_CharDataArr[ cSym - m_FontHdr.IntStart ];
		int x = (pCharData->tx2 - pCharData->tx1) * m_FontHdr.TexWidth ;
		int y = (pCharData->ty2 - pCharData->ty1) * m_FontHdr.TexHeight;
		return std::make_pair( x, y );
	}
}
