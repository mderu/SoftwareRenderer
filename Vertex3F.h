#pragma once
#ifndef __Vertex3F_H__
#define __Vertex3F_H__

#include "Vector3F.h"

class Vertex3F
{
public:
	//Point
	Vector3F p;
	//Normal
	Vector3F n;
	//Will not support alpha values, unfortunately
	Vector3F c;

	Vertex3F();
	Vertex3F(Vector3F);
	Vertex3F(const Vertex3F&);
	Vertex3F(Vector3F, Vector3F, Vector3F);
	~Vertex3F();

	Vertex3F operator-(const Vertex3F&) const;
	Vertex3F operator+(const Vertex3F&) const;
	Vertex3F operator-=(const Vertex3F&);
	Vertex3F operator+=(const Vertex3F&);
	Vertex3F operator=(const Vertex3F&);
	Vertex3F operator*(const float&) const;
	Vertex3F operator/(const float&) const;
};

#endif

