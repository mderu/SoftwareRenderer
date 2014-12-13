#include "stdafx.h"
#include "Util.h"
#include "math.h"
#include "GL/glut.h"
#include <iostream>

Vector3F moveAlong(Vector3F angles, float dirX, float dirY, float dirZ)
{
	return moveAlong(angles.x, angles.y, angles.z, dirX, dirY, dirZ);
}
Vector3F moveAlong(float angX, float angY, float angZ, Vector3F direction)
{
	return moveAlong(angX, angY, angZ, direction.x, direction.y, direction.z);
}
Vector3F moveAlong(float rotX, float rotY, float rotZ, float dirX, float dirY, float dirZ)
{
	Matrix4F retMe;
	//c = cos, s = sin
	float isDeg = (Matrix4F::isDegrees() ? PI / 180.0f : 1.0f);
	float cx = cos(rotX * isDeg), cy = cos(rotY * isDeg), cz = cos(rotZ * isDeg);
	float sx = sin(rotX * isDeg), sy = sin(rotY * isDeg), sz = sin(rotZ * isDeg);

	return Vector3F(
		-dirX*(cy*cz) - dirY*(sx*sy*cz + cx*sz) - dirZ*(-cx*sy*cz + sx*sz),
		-dirX*(-cy*sz) - dirY*(-sx*sy*sz + cx*cz) - dirZ*(cx*sy*sz + sx*cz),
		-dirX*(sy) - dirY*(-sx*cy) - dirZ*(cx*cy)
		);
}
Vector3F moveAlong(Vector3F angles, Vector3F direction)
{
	return moveAlong(angles.x, angles.y, angles.z, direction.x, direction.y, direction.z);
}
Vector3F moveAlong(Matrix4F rotation, Vector3F direction)
{
	return Vector3F
		(
		-direction.x*rotation[0][0] - direction.y*rotation[1][0] - direction.z*rotation[2][0],
		-direction.x*rotation[0][1] - direction.y*rotation[1][1] - direction.z*rotation[2][1],
		-direction.x*rotation[0][2] - direction.y*rotation[1][2] - direction.z*rotation[2][2]
		);
}
Vector3F moveAlong(Matrix4F rotation, Matrix4F direction)
{
	return Vector3F
		(
		-direction[0][0]*rotation[0][0] - direction[0][1]*rotation[1][0] - direction[0][2]*rotation[2][0],
		-direction[0][0]*rotation[0][1] - direction[0][1]*rotation[1][1] - direction[0][2]*rotation[2][1],
		-direction[0][0]*rotation[0][2] - direction[0][1]*rotation[1][2] - direction[0][2]*rotation[2][2]
		);
}
Vector3F crossProduct(const Vector3F &a, const Vector3F &b, const Vector3F &c)
{
	return Vector3F((a.y - b.y)*(b.z - c.z) - (a.z - b.z)*(b.y - c.y),
		(a.z - b.z)*(b.x - c.x) - (a.x - b.x)*(b.z - c.z),
		(a.x - b.x)*(b.y - c.y) - (a.y - b.y)*(b.x - c.x));
}
float get_dx(const Vector3F &a, const Vector3F &b)
{
	return (a.y == b.y) ? (a.x - b.x) : ((a.x - b.x) / (a.y - b.y));
	/*//Effectively this code:
	if(a.y - b.y == 0)
	{
	//avoid division by zero. because returning NaN would destroy our algorithm.
	//We are moving in y steps of 1, so this is an okay value to use.
	return a.x - b.x;
	}
	else
	{
	return (a.x - b.x)/(a.y - b.y);
	}
	*/
}


Vertex3F get_d_dy(const Vertex3F &a, const Vertex3F &b)
{
	Vertex3F retMe;

	retMe = a - b;
	if (a.p.y != b.p.y)
	{
		retMe.p /= a.p.y - b.p.y;
		retMe.n /= a.p.y - b.p.y;
		retMe.c /= a.p.y - b.p.y;
	}
	return retMe;
}

float get_dz(const Vector3F & a, const Vector3F & b)
{
	//See get_dx for details
	return (a.y == b.y) ? (a.z - b.z) : ((a.z - b.z) / (a.y - b.y));
}

float crossProductZ(const Vector3F &a, const Vector3F &b, const Vector3F &c)
{
	return (a.x - b.x)*(b.y - c.y) - (a.y - b.y)*(b.x - c.x);
}

float dotProduct(const Vector3F &a, const Vector3F &b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
float angleBetween(const Vector3F &a, const Vector3F &b)
{
	if (Matrix4F::isDegrees())
	{
		return acos(dotProduct(a, b) / (a.magnitude() * b.magnitude())) * toDeg;
	}
	else
	{
		return acos(dotProduct(a, b) / (a.magnitude() * b.magnitude()));
	}
}
float cosBetween(const Vector3F &a, const Vector3F &b)
{
	return dotProduct(a, b) / (a.magnitude() + b.magnitude());
}

Vector3F reflectRay(const Vector3F& rayDirection, const Vector3F& normal)
{
	return (rayDirection - normal * (2*dotProduct(rayDirection, normal)));
}
bool iShouldCheckThisMesh(const Vector3F& rayOrigin, const Vector3F& rayDirection, const Mesh* mesh)
{
	Vector3F fakeNorm = crossProduct(rayOrigin, rayOrigin + rayDirection, mesh->p).normalize();
	std::cout << mesh->p.x << ", " << mesh->p.y << ", " << mesh->p.z << std::endl;
	bool retMe =  pow(fakeNorm.x*mesh->p.x + fakeNorm.y*mesh->p.y + fakeNorm.z, 2) / (fakeNorm.x*fakeNorm.x + fakeNorm.y*fakeNorm.y) < 100;
	if (retMe){ std::cout << "You should check" << std::endl; std::cin >> fakeNorm.x; }
	return retMe;
}

//TODO::Fix Spherical Raycast

//Deprecated: Do not use until it gets fixed.
float rayIntersectsBoundingSphere(const Vector3F& rayOrigin, const Vector3F& rayDirection, const Mesh* mesh)
{
	//Ray can be written origin + direction * t where t >= 0
	//Sphere can be written as (x - center.x)^2 + (y - center.y)^2 + (z - center.z)^2 = radius^2
	//Need to determine where these equations coincide.  If

	float a = rayDirection.sqrMagnitude();
	float b = 2 * dotProduct(rayDirection, (rayOrigin - mesh->p));
	float c = (rayOrigin - mesh->p).sqrMagnitude() - 2*mesh->s.sqrMagnitude();
	
	float determinant = b*b - 4 * a*c;

	//prevents imaginary numbers (if det < 0, 0 solutions)
	//If negative, just return -1.
	if (determinant < b*b){ return -1.0f; }
	float t_small = (-b - sqrt(determinant)) / (2 * a);
	return t_small > 0 ? t_small : (-b + sqrt(determinant)) / (2 * a);
	//std::cout << *t_small << ", " << *t_large << std::endl;
	return true;
}

Matrix4F camMatrix()
{
	Matrix4F retMe = Matrix4F::rotate(eyeRot);
	retMe[0][3] = eyePos.x;
	retMe[1][3] = eyePos.y;
	retMe[2][3] = eyePos.z;
	return retMe;
}

void drawLine2i(int x1, int y1, int x2, int y2)
{
	int i = x1;
	int j = y1;
	if (x2 < x1 && y2 <= y1)
	{
		j = y2;
		y2 = y1;
		y1 = j;

		i = x2;
		x2 = x1;
		x1 = i;
	}
	if (x1 < x2 && y1 <= y2){
		glVertex2i(i, j);
		int dx = x2 - x1;
		int dy = y2 - y1;

		if (abs(dy) >= abs(dx))
		{
			int F = 2 * dy*(i)+2 * (x2*y1 - x1*y2) - 2 * dx*(j);
			while (j < y2)
			{
				j++;
				if (F > 0)
				{
					F -= 2 * dx;
				}
				else
				{
					F -= 2 * dx - 2 * dy;
					i++;
				}
				glVertex2i(i, j);
			}
		}
		else
		{
			int F = 2 * dy*(i)+2 * (x2*y1 - x1*y2) - 2 * dx*(j);
			while (i < x2)
			{
				//std::cout << "F = " << F << std::endl;
				i++;
				if (F >= 0)
				{
					F -= 2 * dy;
				}
				else
				{
					F -= 2 * dy - 2 * dx;
					j++;
				}
				glVertex2i(i, j);
			}
		}
	}
	else{
		if (x1 >= x2 && y1 < y2){
			i = x2;
			x2 = x1;
			x1 = i;

			j = y2;
			y2 = y1;
			y1 = j;
		}
		//std::cout << y2 << " - " << y1 << ". " << x2 << " - " << x1 << std::endl;
		int dx = x2 - x1;
		int dy = y2 - y1;

		glVertex2i(i, j);
		if (abs(dy) > abs(dx))
		{
			//std::cout << "Here" << std::endl;
			int F = 2 * dy*(i)+2 * (x2*y1 - x1*y2) - 2 * dx*(j);
			while (j > y2)
			{
				//std::cout << F << std::endl;
				j--;
				if (F <= 0)
				{
					F += 2 * dx;
				}
				else
				{
					F += 2 * dx + 2 * dy;
					i++;
				}
				glVertex2i(i, j);
			}
		}
		else
		{
			int F = 2 * dy*(i + 1) + 2 * (x2*y1 - x1*y2) - 2 * dx*(j);
			while (i < x2)
			{
				i++;
				if (F >= 0)
				{
					F += 2 * dy;
				}
				else
				{
					F += 2 * dy + 2 * dx;
					j--;
				}
				glVertex2i(i, j);
			}
		}
	}
}


void drawLine2i(Vector3F a, Vector3F b) { drawLine2i((int)a.x, (int)a.y, (int)b.x, (int)b.y); }

void drawLine2i(Vertex3F a, Vertex3F b){ drawLine2i((int)a.p.x, (int)a.p.y, (int)b.p.x, (int)b.p.y); }

Vector3F eyePos(0, 0, 1000);
Vector3F eyeRot(0, 0, 0);
bool backfaceCulling = true;
bool renderPhong = false;
bool barryPhong = false;
bool smoothTriangles = false;
bool wireFrame = false;
bool lightAndShadows = true;
//This one is entirely for fun. It creates some really cool effects, giving models a backlit effect.
bool backLit = false;
float z_buf[WINDOW_WIDTH][WINDOW_HEIGHT];

std::vector<Mesh*> sceneMeshes;
std::vector<Light*> sceneLights;
