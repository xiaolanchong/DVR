#pragma once


class USBKey
{
public:
	USBKey(): oldRes( 0 )
	{
	}
	unsigned char GetResponseFromKey( int argument )
	{
		unsigned char * bytes = (unsigned char * )(&argument);
		int res = 170 ^ bytes[0] ^ bytes[1] ^ bytes[2] ^ bytes[3] ^ oldRes;
		oldRes = res;
		return res;
	}
private:
	unsigned char oldRes;
};

class DeltaTimeProtection
{
public:
	DeltaTimeProtection():cnt(0), oldRes(0), dif(0) {}
	virtual ~DeltaTimeProtection(){}

	//returns delta (difference between current and previous times)
	int operator()()
	{
		const int keyTime = 7127;
		const int difTime = 13673;

		cnt = (++cnt) % (keyTime + difTime);

		if ( cnt == keyTime )
		{
			r = rand();
			res = key.GetResponseFromKey( r );
			return dif;
		} 
		else if ( cnt == difTime)
		{
			dif = res - HashFun( r, oldRes );
			oldRes = res;
			return dif;
		}		
		return dif;
	}

	unsigned char HashFun( int r, unsigned char oldRes )
	{
		unsigned char * bytes = (unsigned char * )(&r);
		int res = 170 ^ bytes[0] ^ bytes[1] ^ bytes[2] ^ bytes[3] ^ oldRes;		
		return res;
	}
private:
	
	USBKey key;
	unsigned char oldRes;
	int dif;
	int res;
	int r;
	int cnt;
};