#ifndef _MERGE_BOXES_H
#define _MERGE_BOXES_H

#include "dvr_common.h"

void MergeBoxes(dvralgo::MotionRegionArr & boxes, size_t maxBoxes = 16 )
{
    if ( boxes.size() < maxBoxes )
	{
		return;
	}
	
	std::sort( boxes.begin(), boxes.end(), std::greater<dvralgo::MotionRegion>() );

	for ( size_t i = 0; i < maxBoxes; i++)
	{
		//unite every big box with all small boxes intersecting it
		dvralgo::MotionRegionArr::iterator it = boxes.begin() + maxBoxes;
		while ( it != boxes.end() )
		{
			ARect tmp = it->rect & boxes[i].rect;
			if ( !tmp.is_null() )
			{
				boxes[i].rect |= it->rect;
				it = boxes.erase( it ); 
			}
			else
			{
				it++;
			}
		}
	}
	boxes.erase( boxes.begin() + maxBoxes, boxes.end() );
}


#endif