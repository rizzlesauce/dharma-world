#ifndef wall1_h
#define wall1_h

#include "gltypes.h"


static const VERTEX_DATA_3D wall1VertexData[] = {
	{/*v:*/{4.000000, -2.000000, 3.000000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, 2.000000, 3.000000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, 2.000000, 1.977874}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, -2.000000, 3.000000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, 2.000000, 1.977874}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, -2.000000, 1.977874}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, -2.000000, 0.000000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, -2.000000, 1.978000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, -0.600000, 0.000000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, -2.000000, 1.978000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, -0.600000, 1.978000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, -0.600000, 0.000000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, 0.600000, 0.000000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, 0.600000, 1.978000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, 2.000000, 1.978000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, 0.600000, 0.000000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, 2.000000, 1.978000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
	{/*v:*/{4.000000, 2.000000, 0.000000}, /*n:*/{-1.000000, 0.000000, 0.000000} },
};

#define wall1NumberOfVertices	18
// Drawing Code:
// glEnableClientState(GL_VERTEX_ARRAY);
// glEnableClientState(GL_NORMAL_ARRAY);
// glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall1VertexData[0].vertex);
// glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall1VertexData[0].normal);
// glDrawArrays(GL_TRIANGLES, 0, wall1NumberOfVertices);
// glDisableClientState(GL_VERTEX_ARRAY);
// glDisableClientState(GL_NORMAL_ARRAY);


#endif
