#include "stdafx.h"
#include "ExtensionsInitializer.h"

GLExtensionInitializer& GetGLExtensionsInitializer()
{
	static GLExtensionInitializer g_ExtInitializer;
	return g_ExtInitializer;	
}
