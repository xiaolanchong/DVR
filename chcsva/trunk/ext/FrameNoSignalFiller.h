//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: «аполнитель кадров картинкой, если нет сигнала на входе платы захвата

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   11.09.2006
*/                                                                                      //
//======================================================================================//
#ifndef _FRAME_NO_SIGNAL_FILLER_6342464761593017_
#define _FRAME_NO_SIGNAL_FILLER_6342464761593017_

//======================================================================================//
//                              class FrameNoSignalFiller                               //
//======================================================================================//

//! \brief «аполнитель кадров картинкой, если нет сигнала на входе платы захвата
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   11.09.2006
//! \version 1.0
//! \bug 
//! \todo 

class FrameNoSignalFiller
{
	//! контекст рисовани€ картинки
	HDC			m_CacheContext;
	//! собств-но изображение RGB24
	HBITMAP		m_CacheBmp;
	
	//! создать заголовок дл€ изображени€ RGB24
	//! \param bih		выходной заголовок
	//! \param Width	ширина изображени€
	//! \param Height	высота изображени€
	static void CreateHeader( BITMAPINFOHEADER& bih, unsigned int Width, unsigned int Height );
	//! очистить изображение (заполнить цветом)
	//! \param hDC		контекст картинки
	//! \param Width	ширина
	//! \param Height	высота
	static void	ClearImage( HDC hDC, unsigned int Width, unsigned int Height );
	//! преобразовать RGB24 в YUY2, нет нужды оптимизировать, вызываетс€ при создании
	//! \param SrcImage		исходные данные
	//! \param DstImage		выходные данные
	//! \param ImageWidth	ширина изображени€
	//! \param ImageHeight	высота изображени€
	static void ConvertRGBtoYUY2( 
		const std::vector<BYTE>& SrcImage, std::vector<BYTE>& DstImage,
		unsigned int ImageWidth, unsigned int ImageHeight
		);
	//! нарисовать на изображении иконку отсутстви€ сигнала
	//! \param hDC			контекст изображени€
	//! \param ImageWidth	ширина
	//! \param ImageHeight	высота
	static void DrawNoSignalIcon( HDC hDC, unsigned int ImageWidth, unsigned int ImageHeight);
	//! создать контекст дл€ изображени€
	//! \param Width	ширина
	//! \param Height	высота
	void		CreateContext(unsigned int Width, unsigned int Height);
	//! копировать данные картинки из m_CacheContext в пользовательскую пам€ть
	//! \param ImageData вызодные данные
	void		CopyBitmapToMemory(std::vector<BYTE>& ImageData) const;

	//! ширина изображени€
	unsigned int m_ImageWidth;
	//! высота изображени€
	unsigned int m_ImageHeight;

	//! выходное изображение
	std::vector<BYTE>		m_ImageData;
public:
	//! создать изображение отсутсви€ сигнала в формате YUY2
	//! \param ImageWidth	ширина
	//! \param ImageHeight	высота
	FrameNoSignalFiller( unsigned int ImageWidth, unsigned int ImageHeight );
	virtual ~FrameNoSignalFiller();

	//! получить данные картинки в формате YUY2
	//! \return 
	const std::vector<BYTE>&	GetImage( ) const 
	{
		return m_ImageData;
	}
};

#endif // _FRAME_NO_SIGNAL_FILLER_6342464761593017_