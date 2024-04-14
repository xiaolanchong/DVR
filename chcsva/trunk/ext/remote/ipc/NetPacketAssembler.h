//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: —борка/отправка TCP пакетов

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   06.07.2006
*/                                                                                      //
//======================================================================================//
#ifndef _NET_PACKET_ASSEMBLER_2314224074565335_
#define _NET_PACKET_ASSEMBLER_2314224074565335_

//======================================================================================//
//                               class NetPacketAssembler                               //
//======================================================================================//

//! \brief —борка/отправка TCP пакетов
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   06.07.2006
//! \version 1.0
//! \bug 
//! \todo 

class NetPacketAssembler
{
	struct Marker
	{
		int nID;
		int Length;
	};

	struct StreamData
	{
		//	InternetAddress		NetAddress;
		int					TotalBytes;
		bool				TotalValid;
		std::vector<BYTE>	Data;

		StreamData(/*const InternetAddress& addr*/):
		//NetAddress(addr),
		TotalBytes(0),
			TotalValid(false)
		{
		}
	};
	StreamData m_StreamData;
	virtual void	OnReceivePacket( const std::vector<BYTE>& Data) = 0; 
protected:
	void		ReceiveData(const void* pBytes, size_t nSize );
	typedef boost::function2<void, const void*, size_t> SenderFunc_t;
	void		SendData( SenderFunc_t, const void* pBytes, size_t nSize );

	void		ClearDataCache();

	virtual ~NetPacketAssembler(){};
};
#endif // _NET_PACKET_ASSEMBLER_2314224074565335_