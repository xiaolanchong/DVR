#ifndef CGPROGRAM_H
#define CGPROGRAM_H

#include <Cg/cg.h> //Cg runtime API
#include <Cg/cgGL.h> // OpenGL-specific Cg runtime API

#include <vector>
#include <map>
#include <string>

#include "GLTexture.h"
#include "GLFrameBuffer.h"

class CgProgram
{
public:
	CgProgram( const std::string& fileName, 
		const std::string& mainName, const std::vector<std::string>& textureParams );
	~CgProgram( );
	
	
	void SetTextureParameter( const std::string& name, GLTexture& texture );
	void SetTextureParameter( const std::string& name, GLFrameBuffer& buffer );
	void SetOutputBuffer( GLFrameBuffer&  buffer );

	void Bind();
	void RunProgram();
	void UnBind();

private:
	CGcontext myContext;
	CGprogram myProgram;

	std::vector<std::string> textureParamNames;
	std::map<std::string, CGparameter> textureParamsToCgParams;
	std::map<std::string, GLuint> textureParamsToTextures;

	void getTextureParamsFromRunTime( const std::vector<std::string>& textureParams );

	void Init( const std::string& fileName, 
		const std::string& mainName, const std::vector<std::string>& textureParams );

	GLFrameBuffer* currentOutput;
}; 

#endif