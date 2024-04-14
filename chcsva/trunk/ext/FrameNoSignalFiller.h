//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright� ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: ����������� ������ ���������, ���� ��� ������� �� ����� ����� �������

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   11.09.2006
*/                                                                                      //
//======================================================================================//
#ifndef _FRAME_NO_SIGNAL_FILLER_6342464761593017_
#define _FRAME_NO_SIGNAL_FILLER_6342464761593017_

//======================================================================================//
//                              class FrameNoSignalFiller                               //
//======================================================================================//

//! \brief ����������� ������ ���������, ���� ��� ������� �� ����� ����� �������
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   11.09.2006
//! \version 1.0
//! \bug 
//! \todo 

class FrameNoSignalFiller
{
	//! �������� ��������� ��������
	HDC			m_CacheContext;
	//! ������-�� ����������� RGB24
	HBITMAP		m_CacheBmp;
	
	//! ������� ��������� ��� ����������� RGB24
	//! \param bih		�������� ���������
	//! \param Width	������ �����������
	//! \param Height	������ �����������
	static void CreateHeader( BITMAPINFOHEADER& bih, unsigned int Width, unsigned int Height );
	//! �������� ����������� (��������� ������)
	//! \param hDC		�������� ��������
	//! \param Width	������
	//! \param Height	������
	static void	ClearImage( HDC hDC, unsigned int Width, unsigned int Height );
	//! ������������� RGB24 � YUY2, ��� ����� ��������������, ���������� ��� ��������
	//! \param SrcImage		�������� ������
	//! \param DstImage		�������� ������
	//! \param ImageWidth	������ �����������
	//! \param ImageHeight	������ �����������
	static void ConvertRGBtoYUY2( 
		const std::vector<BYTE>& SrcImage, std::vector<BYTE>& DstImage,
		unsigned int ImageWidth, unsigned int ImageHeight
		);
	//! ���������� �� ����������� ������ ���������� �������
	//! \param hDC			�������� �����������
	//! \param ImageWidth	������
	//! \param ImageHeight	������
	static void DrawNoSignalIcon( HDC hDC, unsigned int ImageWidth, unsigned int ImageHeight);
	//! ������� �������� ��� �����������
	//! \param Width	������
	//! \param Height	������
	void		CreateContext(unsigned int Width, unsigned int Height);
	//! ���������� ������ �������� �� m_CacheContext � ���������������� ������
	//! \param ImageData �������� ������
	void		CopyBitmapToMemory(std::vector<BYTE>& ImageData) const;

	//! ������ �����������
	unsigned int m_ImageWidth;
	//! ������ �����������
	unsigned int m_ImageHeight;

	//! �������� �����������
	std::vector<BYTE>		m_ImageData;
public:
	//! ������� ����������� ��������� ������� � ������� YUY2
	//! \param ImageWidth	������
	//! \param ImageHeight	������
	FrameNoSignalFiller( unsigned int ImageWidth, unsigned int ImageHeight );
	virtual ~FrameNoSignalFiller();

	//! �������� ������ �������� � ������� YUY2
	//! \return 
	const std::vector<BYTE>&	GetImage( ) const 
	{
		return m_ImageData;
	}
};

#endif // _FRAME_NO_SIGNAL_FILLER_6342464761593017_