#ifndef __UTIL_H__
#define __UTIL_H__

#include "Matrix4F.h"
#include "Vector3F.h"
#include "Vertex3F.h"
#include "Mesh.h"
#include "Light.h"
#include <vector>
#include <iostream>

using std::cin;
using std::endl;

Vector3F moveAlong(Vector3F angles, float dirX, float dirY, float dirZ);
Vector3F moveAlong(float rotX, float rotY, float rotZ, Vector3F direction);
Vector3F moveAlong(float rotX, float rotY, float rotZ, float dirX, float dirY, float dirZ);
Vector3F moveAlong(Vector3F angles, Vector3F direction);
Vector3F moveAlong(Matrix4F rotation, Vector3F direction);
Vector3F moveAlong(Matrix4F rotation, Matrix4F direction);
Vector3F crossProduct(const Vector3F &a, const Vector3F &b, const Vector3F &c);

Vertex3F get_d_dy(const Vertex3F &a, const Vertex3F &b);
float get_dx(const Vector3F &a, const Vector3F &b);
float get_dz(const Vector3F & a, const Vector3F & b);
float crossProductZ(const Vector3F &a, const Vector3F &b, const Vector3F &c);
float dotProduct(const Vector3F &a, const Vector3F &b);
float angleBetween(const Vector3F &a, const Vector3F &b);
float cosBetween(const Vector3F &a, const Vector3F &b);
Vector3F reflectRay(const Vector3F&, const Vector3F&);


void drawLine2i(int, int, int, int);
void drawLine2i(Vector3F a, Vector3F b);
void drawLine2i(Vertex3F a, Vertex3F b);


extern Vector3F eyePos;
extern Vector3F eyeRot;
Matrix4F camMatrix();

const float float_max = std::numeric_limits<float>::max();
const float toDeg = 180.0f / PI;
const float toRad = PI / 180.0f;

const int WINDOW_WIDTH = 801;
const int WINDOW_HEIGHT = 801;
extern float z_buf[WINDOW_WIDTH][WINDOW_HEIGHT];

extern bool backfaceCulling, wireFrame, renderPhong, lightAndShadows, backLit, smoothTriangles, barryPhong;

bool iShouldCheckThisMesh(const Vector3F& rayOrigin, const Vector3F& rayDirection, const Mesh* mesh);
float rayIntersectsBoundingSphere(const Vector3F& rayOrigin, const Vector3F& rayDirection, const Mesh* mesh);

extern std::vector<Mesh*> sceneMeshes;
extern std::vector<Light*> sceneLights;

#endif