// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <cmath>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

#include "Windows.h"


#include <Cg/cg.h> //Cg runtime API
#include <Cg/cgGL.h> // OpenGL-specific Cg runtime API

#include <GL/glut.h>
#include <GL/glext.h> 

//const GLenum targetTexture = GL_TEXTURE_2D;
//const int maxWidth = 1;
//const int maxHeight = 1;

const GLenum targetTexture = GL_TEXTURE_RECTANGLE_NV;
const int maxWidth = 352;
const int maxHeight = 288;


// TODO: reference additional headers your program requires here
