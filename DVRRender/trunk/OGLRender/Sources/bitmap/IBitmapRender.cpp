//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Интерфейс отрисовки 2D-изображений

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   14.06.2006
*/                                                                                      //
//======================================================================================//

#include "stdafx.h"
#include "IBitmapRender.h"

#include "RasterRender.h"
#include "TextureRender.h"

//======================================================================================//
//                                 class IBitmapRender                                  //
//======================================================================================//
/*
IBitmapRender::IBitmapRender()
{
}

IBitmapRender::~IBitmapRender()
{
}*/

//! поддерживает ли карта расширение OpenGL, из http://www.gamedev.ru/articles/?id=20002
//! \param szExtensionName имя расширения
//! \return да/нет
bool IsExtensionSupported(const char * szExtensionName)
{
	// Получить строку со списком доступных расширений
	const char * ch = (const char *)glGetString(GL_EXTENSIONS);

	size_t size = strlen(szExtensionName);
	while(ch)
	{
		if(!strncmp(ch,szExtensionName,size) && (*(ch+size)==0 || *(ch+size)==' '))
			return true;
		ch=strchr(ch,' ');
		if(ch)
			ch++;
	}

	return false;
}

//#define FORCE_TO_REFERENCE


boost::shared_ptr<IBitmapRender>	IBitmapRender::CreateBitmapRender()
{
	bool bNPOTTextureSupported = IsExtensionSupported( "GL_ARB_texture_rectangle" ) ||
								 IsExtensionSupported( "GL_NV_texture_rectangle" );
#ifdef FORCE_TO_REFERENCE
	bNPOTTextureSupported = false;
#endif
	return boost::shared_ptr<IBitmapRender>( 
				bNPOTTextureSupported ? static_cast<IBitmapRender*>( new TextureRender )  
									  : static_cast<IBitmapRender*>( new RasterRender  ) 
									  );
}