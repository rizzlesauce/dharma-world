#ifndef ceiling_h
#define ceiling_h

#include "gltypes.h"


static const VERTEX_DATA_3D ceilingVertexData[] = {
    {/*v:*/{4.000000, 3.000000, 2.000000}, /*n:*/{0.000000, -1.000000, 0.000000} },
    {/*v:*/{-3.999999, 3.000000, 2.000000}, /*n:*/{0.000000, -1.000000, 0.000000} },
    {/*v:*/{-4.000000, 3.000000, -2.000000}, /*n:*/{0.000000, -1.000000, 0.000000} },
    {/*v:*/{4.000000, 3.000000, 2.000000}, /*n:*/{0.000000, -1.000000, 0.000000} },
    {/*v:*/{-4.000000, 3.000000, -2.000000}, /*n:*/{0.000000, -1.000000, 0.000000} },
    {/*v:*/{4.000000, 3.000000, -2.000000}, /*n:*/{0.000000, -1.000000, 0.000000} },
};

#define ceilingNumberOfVertices    6
// Drawing Code:
// glEnableClientState(GL_VERTEX_ARRAY);
// glEnableClientState(GL_NORMAL_ARRAY);
// glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &ceilingVertexData[0].vertex);
// glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &ceilingVertexData[0].normal);
// glDrawArrays(GL_TRIANGLES, 0, ceilingNumberOfVertices);
// glDisableClientState(GL_VERTEX_ARRAY);
// glDisableClientState(GL_NORMAL_ARRAY);


#endif
