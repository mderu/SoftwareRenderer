//Mesh Class
#pragma once
#ifndef __MESH_H__
#define __MESH_H__

#include "Vector3F.h"
#include "Matrix4F.h"
#include "Vertex3F.h"
#include "Face.h"

class Mesh{
public:
	//Position
	Vector3F p;
	//Rotation
	Vector3F r;
	//Scale
	Vector3F s;

	//Product sum of previous matricies.
	Matrix4F transform;

	std::vector<Vertex3F> oVertArray;
	std::vector<Vertex3F> vertArray;
	std::vector< Face > faces;

	//Temporary coloring until a materials are created.
	std::vector<Vector3F> triColors;

	float shineCoeff;

public:
	//Constructors
	Mesh() : p(Vector3F(0, 0, 0)), r(Vector3F(0, 0, 0)), s(Vector3F(1, 1, 1)), shineCoeff(7){}

	//Scale:
	void scale(float);
	void scale(float, float, float);
	void scale(Vector3F);
	void setScale(Vector3F);
	void setScale(float, float, float);

	Vector3F getScale() const;
	Matrix4F getScaleMatrix() const;

	//Rotation:
	void setRotation(Vector3F);
	void setRotation(float, float, float);

	Vector3F getEulerAngles();

	void calculateVerts();

	void drawQuad(Vertex3F v1, Vertex3F v2, Vertex3F v3, Vertex3F v4);

	void drawTriangle(Vertex3F, Vertex3F, Vertex3F) const;
	void smoothTriangle(const Face&) const;
	void PhongTriangle(const Face&) const;
	void BarryLerpPhong(const Face&) const;
	Vector3F calculateLighting(const Vertex3F& v, const Face& f, const Vector3F eye) const;

	void drawMesh();
};

#endif
