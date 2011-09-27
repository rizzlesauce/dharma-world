#ifndef wall4_h
#define wall4_h

#include "gltypes.h"


static const VERTEX_DATA_3D wall4VertexData[] = {
    {/*v:*/{-4.000000, 0.000000, -2.000000}, /*n:*/{0.000000, 0.000000, 1.000000}},
    {/*v:*/{3.999999, -0.000000, -2.000000}, /*n:*/{0.000000, 0.000000, 1.000000}},
    {/*v:*/{-4.000000, 3.000000, -2.000000}, /*n:*/{0.000000, 0.000000, 1.000000}},
    {/*v:*/{3.999999, -0.000000, -2.000000}, /*n:*/{0.000000, 0.000000, 1.000000}},
    {/*v:*/{4.000000, 3.000000, -2.000000}, /*n:*/{0.000000, 0.000000, 1.000000}},
    {/*v:*/{-4.000000, 3.000000, -2.000000}, /*n:*/{0.000000, 0.000000, 1.000000}}
};

#define wall4NumberOfVertices    6
// Drawing Code:
// glEnableClientState(GL_VERTEX_ARRAY);
// glEnableClientState(GL_COLOR_ARRAY);
// glEnable(GL_COLOR_MATERIAL)
// glEnableClientState(GL_NORMAL_ARRAY);
// glVertexPointer(3, GL_FLOAT, sizeof(COLORED_VERTEX_DATA_3D), &wall4VertexData[0].vertex);
// glNormalPointer(GL_FLOAT, sizeof(COLORED_VERTEX_DATA_3D), &wall4VertexData[0].normal);
// glColorPointer(4, GL_FLOAT, sizeof(COLORED_VERTEX_DATA_3D), &wall4VertexData[0].color);
// glDrawArrays(GL_TRIANGLES, 0, wall4NumberOfVertices);
// glDisableClientState(GL_VERTEX_ARRAY);
// glDisableClientState(GL_NORMAL_ARRAY);
// glDisable(GL_COLOR_MATERIAL);
// glDisableClientState(GL_NORMAL_ARRAY);


#endif
