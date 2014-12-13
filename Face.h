#ifndef __FACE_H__
#define __FACE_H__

#include "Vertex3F.h"

class Face
{
public:
	Vertex3F* vert1;
	Vertex3F* vert2;
	Vertex3F* vert3;
	float u1;
	float v1;
	float u2;
	float v2;
	float u3;
	float v3;

	Face();

	//Deleting Faces should NOT remove the vertexes they point to.
	//The mesh vertArray should handle all of this.
	~Face();
};
#endif
