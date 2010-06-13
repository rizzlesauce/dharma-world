#ifndef dharmaCube_h
#define dharmaCube_h

#include "gltypes.h"


static const TEXTURED_VERTEX_DATA_3D dharmaCubeVertexData[] = {
	{/*v:*/{1.000000, 1.000000, -1.000000}, /*n:*/{0.666646, 0.333323, -0.666646}, /*t:*/{0.000000, 0.500000}},
	{/*v:*/{1.000000, -1.000000, -1.000000}, /*n:*/{0.408246, -0.816492, -0.408246}, /*t:*/{0.250000, 0.500000}},
	{/*v:*/{-1.000000, -1.000000, -1.000000}, /*n:*/{-0.408246, -0.408246, -0.816492}, /*t:*/{0.250000, 0.749999}},
	{/*v:*/{1.000000, 1.000000, -1.000000}, /*n:*/{0.666646, 0.333323, -0.666646}, /*t:*/{0.000000, 0.500000}},
	{/*v:*/{-1.000000, -1.000000, -1.000000}, /*n:*/{-0.408246, -0.408246, -0.816492}, /*t:*/{0.250000, 0.749999}},
	{/*v:*/{-1.000000, 1.000000, -1.000000}, /*n:*/{-0.666646, 0.666646, -0.333323}, /*t:*/{0.000000, 0.749999}},
	{/*v:*/{-1.000000, -1.000000, 1.000000}, /*n:*/{-0.577349, -0.577349, 0.577349}, /*t:*/{0.500000, 0.250001}},
	{/*v:*/{-1.000000, 1.000000, 1.000000}, /*n:*/{-0.577349, 0.577349, 0.577349}, /*t:*/{0.750000, 0.250001}},
	{/*v:*/{-1.000000, 1.000000, -1.000000}, /*n:*/{-0.666646, 0.666646, -0.333323}, /*t:*/{0.749999, 0.500001}},
	{/*v:*/{-1.000000, -1.000000, 1.000000}, /*n:*/{-0.577349, -0.577349, 0.577349}, /*t:*/{0.500000, 0.250001}},
	{/*v:*/{-1.000000, 1.000000, -1.000000}, /*n:*/{-0.666646, 0.666646, -0.333323}, /*t:*/{0.749999, 0.500001}},
	{/*v:*/{-1.000000, -1.000000, -1.000000}, /*n:*/{-0.408246, -0.408246, -0.816492}, /*t:*/{0.499999, 0.500000}},
	{/*v:*/{1.000000, -1.000000, 1.000000}, /*n:*/{0.816492, -0.408246, 0.408246}, /*t:*/{0.250000, 0.250000}},
	{/*v:*/{0.999999, 1.000000, 1.000001}, /*n:*/{0.333323, 0.666646, 0.666646}, /*t:*/{0.000000, 0.250000}},
	{/*v:*/{-1.000000, -1.000000, 1.000000}, /*n:*/{-0.577349, -0.577349, 0.577349}, /*t:*/{0.250000, 0.000000}},
	{/*v:*/{0.999999, 1.000000, 1.000001}, /*n:*/{0.333323, 0.666646, 0.666646}, /*t:*/{0.000000, 0.250000}},
	{/*v:*/{-1.000000, 1.000000, 1.000000}, /*n:*/{-0.577349, 0.577349, 0.577349}, /*t:*/{0.000000, 0.000000}},
	{/*v:*/{-1.000000, -1.000000, 1.000000}, /*n:*/{-0.577349, -0.577349, 0.577349}, /*t:*/{0.250000, 0.000000}},
	{/*v:*/{1.000000, -1.000000, -1.000000}, /*n:*/{0.408246, -0.816492, -0.408246}, /*t:*/{0.250000, 0.500000}},
	{/*v:*/{1.000000, 1.000000, -1.000000}, /*n:*/{0.666646, 0.333323, -0.666646}, /*t:*/{0.000000, 0.500000}},
	{/*v:*/{1.000000, -1.000000, 1.000000}, /*n:*/{0.816492, -0.408246, 0.408246}, /*t:*/{0.250000, 0.250000}},
	{/*v:*/{1.000000, 1.000000, -1.000000}, /*n:*/{0.666646, 0.333323, -0.666646}, /*t:*/{0.000000, 0.500000}},
	{/*v:*/{0.999999, 1.000000, 1.000001}, /*n:*/{0.333323, 0.666646, 0.666646}, /*t:*/{0.000000, 0.250000}},
	{/*v:*/{1.000000, -1.000000, 1.000000}, /*n:*/{0.816492, -0.408246, 0.408246}, /*t:*/{0.250000, 0.250000}},
	{/*v:*/{1.000000, 1.000000, -1.000000}, /*n:*/{0.666646, 0.333323, -0.666646}, /*t:*/{0.999999, 0.500002}},
	{/*v:*/{-1.000000, 1.000000, -1.000000}, /*n:*/{-0.666646, 0.666646, -0.333323}, /*t:*/{0.749999, 0.500001}},
	{/*v:*/{0.999999, 1.000000, 1.000001}, /*n:*/{0.333323, 0.666646, 0.666646}, /*t:*/{1.000000, 0.250002}},
	{/*v:*/{-1.000000, 1.000000, -1.000000}, /*n:*/{-0.666646, 0.666646, -0.333323}, /*t:*/{0.749999, 0.500001}},
	{/*v:*/{-1.000000, 1.000000, 1.000000}, /*n:*/{-0.577349, 0.577349, 0.577349}, /*t:*/{0.750000, 0.250001}},
	{/*v:*/{0.999999, 1.000000, 1.000001}, /*n:*/{0.333323, 0.666646, 0.666646}, /*t:*/{1.000000, 0.250002}},
	{/*v:*/{1.000000, -1.000000, -1.000000}, /*n:*/{0.408246, -0.816492, -0.408246}, /*t:*/{0.250000, 0.500000}},
	{/*v:*/{1.000000, -1.000000, 1.000000}, /*n:*/{0.816492, -0.408246, 0.408246}, /*t:*/{0.250000, 0.250000}},
	{/*v:*/{-1.000000, -1.000000, 1.000000}, /*n:*/{-0.577349, -0.577349, 0.577349}, /*t:*/{0.500000, 0.250001}},
	{/*v:*/{1.000000, -1.000000, -1.000000}, /*n:*/{0.408246, -0.816492, -0.408246}, /*t:*/{0.250000, 0.500000}},
	{/*v:*/{-1.000000, -1.000000, 1.000000}, /*n:*/{-0.577349, -0.577349, 0.577349}, /*t:*/{0.500000, 0.250001}},
	{/*v:*/{-1.000000, -1.000000, -1.000000}, /*n:*/{-0.408246, -0.408246, -0.816492}, /*t:*/{0.499999, 0.500000}},
};

#define dharmaCubeNumberOfVertices	36
// Drawing Code:
// glEnableClientState(GL_VERTEX_ARRAY);
// glEnableClientState(GL_TEXTURE_COORD_ARRAY);
// glEnableClientState(GL_NORMAL_ARRAY);
// glVertexPointer(3, GL_FLOAT, sizeof(TEXTURED_VERTEX_DATA_3D), &dharmaCubeVertexData[0].vertex);
// glNormalPointer(GL_FLOAT, sizeof(TEXTURED_VERTEX_DATA_3D), &dharmaCubeVertexData[0].normal);
// glTexCoordPointer(2, GL_FLOAT, sizeof(TEXTURED_VERTEX_DATA_3D), &dharmaCubeVertexData[0].texCoord);
// glDrawArrays(GL_TRIANGLES, 0, dharmaCubeNumberOfVertices);
// glDisableClientState(GL_VERTEX_ARRAY);
// glDisableClientState(GL_TEXTURE_COORD_ARRAY);
// glDisableClientState(GL_NORMAL_ARRAY);


#endif
