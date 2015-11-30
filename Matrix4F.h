// CS130 Fall 2013: Computer Graphics
// Matrix4F.h
//
// This file does not need to be modified
/////////////////////////////////////////
#ifndef __MATRIX4F_H__
#define __MATRIX4F_H__

#include "stdafx.h"
#include <vector>
#include "Vector3F.h"
#include "Vertex3F.h"

const float PI = 3.14159265f;

//Note: Everything public is bad, but I like shooting myself in the
//foot from time to time.

class Matrix4F
{
public:
	std::vector<std::vector<float> > matrix;

	//Constructors
	Matrix4F();
	Matrix4F(const Matrix4F& m);

	/* Deprecated
	Matrix4F(const float & tx, const float & ty, const float & tz, MatrixType T);
	Matrix4F(Vector3F t, MatrixType T);
	Matrix4F(const float & angle, const Axis axis);
	*/

	//Operators
	Vector3F operator*(const Vector3F & vec) const;
	Vertex3F operator*(const Vertex3F & vert) const;
	Matrix4F operator*(const Matrix4F & rhs) const;

	Matrix4F& operator=(const Matrix4F & rhs);

	std::vector<float>& operator[](int index);
	const std::vector<float>& operator[](int index) const;

	//Getters:
	Vector3F getTranslation();
	Vector3F getScale();

	//Static Methods
	static Matrix4F projection(float, float, float, float, bool);
	static Matrix4F projection();
	static Matrix4F identity();
	static Matrix4F scale(Vector3F p);
	static Matrix4F scale(float px, float py, float pz);
	static Matrix4F translate(Vector3F p);
	static Matrix4F translate(float px, float py, float pz);
	static Matrix4F rotate(float px, float py, float pz);
	static Matrix4F rotate(Vector3F p);
	static Matrix4F rotateAround(Vector3F point, Vector3F eulerAngles);
	static void useDegrees(bool useDeg);
	static void useRadians(bool useDeg) ;
	static bool isDegrees();
	static Matrix4F transform(Vector3F position, Vector3F eulerAngles, Vector3F scale);


private: 
	static bool usesDegrees;
};

#endif
