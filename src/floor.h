#ifndef floor_h
#define floor_h

#include "gltypes.h"


static const VERTEX_DATA_3D floorVertexData[] = {
	{/*v:*/{4.000000, 0.000000, 2.000000}, /*n:*/{0.000000, 1.000000, 0.000000} },
	{/*v:*/{4.000000, -0.000000, -2.000000}, /*n:*/{0.000000, 1.000000, 0.000000} },
	{/*v:*/{-3.999999, -0.000000, -2.000000}, /*n:*/{0.000000, 1.000000, 0.000000} },
	{/*v:*/{4.000000, 0.000000, 2.000000}, /*n:*/{0.000000, 1.000000, 0.000000} },
	{/*v:*/{-3.999999, -0.000000, -2.000000}, /*n:*/{0.000000, 1.000000, 0.000000} },
	{/*v:*/{-4.000000, 0.000000, 2.000000}, /*n:*/{0.000000, 1.000000, 0.000000} },
};

#define floorNumberOfVertices	6
// Drawing Code:
// glEnableClientState(GL_VERTEX_ARRAY);
// glEnableClientState(GL_NORMAL_ARRAY);
// glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &floorVertexData[0].vertex);
// glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &floorVertexData[0].normal);
// glDrawArrays(GL_TRIANGLES, 0, floorNumberOfVertices);
// glDisableClientState(GL_VERTEX_ARRAY);
// glDisableClientState(GL_NORMAL_ARRAY);


#endif
