#ifndef FRAMEEXTRACTOR_H
#define FRAMEEXTRACTOR_H

struct Frame
{
	unsigned int xLeft;
	unsigned int yUp;
	unsigned int xRight;
	unsigned int yDown;

	Frame( unsigned int x, unsigned int y )
		: xLeft( x ), xRight( x ), yUp( y ), yDown( y )
	{
	}

	Frame( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2 )
		: xLeft( x1 ), xRight( x2 ), yUp( y1 ), yDown( y2 )
	{
	}
};

class FrameExtractor
{
public:
	FrameExtractor( unsigned int width_, unsigned int height_, unsigned int threshold_,
unsigned int minimalArea_, unsigned int downSale_ )
	:width( width_ ), height( height_ ), threshold( threshold_ ), minimalArea( minimalArea_),
downScale( downSale_ ) 
	{
		isProcessed.resize( width * height );
	}
	
	void GetFrames( const unsigned char * bytes, std::vector<Frame>& output )
	{
		std::fill( isProcessed.begin(), isProcessed.end(), 0 );
		currentImage = bytes;
		
		for( unsigned int y = 0; y < height; ++y )
		{
			for( unsigned int x = 0; x < width; ++x )
			{
				if( !ProcessedAt( x, y ) && IsMotionAt( x, y ) )
				{
					Frame frame( x, y );
					InflateFrame( x, y, frame );

					frame.xLeft = frame.xLeft;// * downScale;
					frame.xRight = frame.xRight;// * downScale;
					frame.yDown = frame.yDown;// * downScale;
					frame.yUp = frame.yUp;// * downScale;
					if( ( frame.yDown - frame.yUp ) * ( frame.xRight - frame.xLeft ) >= minimalArea )
						output.push_back( frame );
					MarkProcessed( x, y );
				}
			}
		}
	}

private:
	
	unsigned int width;
	unsigned int height;
	unsigned int threshold;
	unsigned int downScale;

	std::vector<unsigned char> isProcessed;

	unsigned int minimalArea;

	const unsigned char * currentImage;

	void InflateFrame( unsigned int x, unsigned int y, Frame& frame )
	{
		MarkProcessed( x, y );

		unsigned int left = x;
		unsigned int right = x;

		if( x < width - 1 )
		{
			for( unsigned int pos = x + 1; ( pos < width ) && !ProcessedAt( pos, y ) && IsMotionAt( pos, y ) ; ++pos )
			{
				++right;
				MarkProcessed( pos, y );
			}
		}
		if( x > 0 )
		{
			for( unsigned int pos = x - 1; ( pos >= 0 ) && !ProcessedAt( pos, y ) && IsMotionAt( pos, y ); --pos )
			{
				--left;
				MarkProcessed( pos, y );
			}
		}
	//	++left;

		// inflate frame
		frame.xLeft = std::min< unsigned int >( frame.xLeft, left );
		frame.xRight = std::max< unsigned int >( frame.xRight, right );
		frame.yUp = std::min< unsigned int >( frame.yUp, y );
		frame.yDown = std::max< unsigned int >( frame.yDown, y );

		//check out down line
		if( y < height - 1 )
		{
			for( unsigned int pos = left; pos < right; ++pos )
			{
				if( !ProcessedAt( pos, y + 1 ) && IsMotionAt( pos, y + 1 ) )
					InflateFrame( pos, y + 1, frame );
			}
		}

		//check out up line
		if( y > 0 )
		{
			for( unsigned int pos = left; pos < right; ++pos )
			{
				if( !ProcessedAt( pos, y - 1 ) && IsMotionAt( pos, y - 1 ) )
					InflateFrame( pos, y - 1, frame );
			}
		}
	}

	bool IsMotionAt( unsigned int x, unsigned int y )
	{
		return *(currentImage + x + y * width) > threshold;
	}

	bool ProcessedAt( unsigned int x, unsigned int y )
	{
		return isProcessed[ x + y * width ] == 1;
	}

	void MarkProcessed( unsigned int x, unsigned int y )
	{
		isProcessed[ x + y * width ] = 1;
	}
};

#endif