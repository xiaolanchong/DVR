#ifndef _ARE_IMAGES_EQUAL_
#define _ARE_IMAGES_EQUAL_


template <class Image >
bool AreImagesEqual( const Image & img1, const Image & img2 )
{
	if ( (img1.width() != img2.width()) || (img1.height() != img2.height() ) )
		return false;
	int w = img1.width();
	int h = img1.height();
 
	for ( int y = 0; y < h; y++)
		for ( int x = 0; x < w; x++)
			if ( ! ( img1(x,y) == img2(x,y) ) )
			{	
				img1(x,y) == img2(x,y);// for dubug purpose
				return false;	
			}
	return true;
}

#endif