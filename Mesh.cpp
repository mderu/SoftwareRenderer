#include "stdafx.h"
#include "Mesh.h"
#include "Util.h"
#include "Vector3F.h"
#include "Matrix4F.h"
#include <math.h>
#include <algorithm>
#include <GL/glut.h>
#include <iostream>

//Scale:
void Mesh::scale(float sc)
{
	s.x *= sc;
	s.y *= sc;
	s.z *= sc;
}
void Mesh::scale(float px, float py, float pz)
{
	s.x *= px;
	s.y *= py;
	s.z *= pz;
}
void Mesh::scale(Vector3F p)
{
	s.x *= p.x;
	s.y *= p.y;
	s.z *= p.z;
}
void Mesh::setScale(Vector3F p)
{
	s.x = p.x;
	s.y = p.y;
	s.z = p.z;
}
void Mesh::setScale(float px, float py, float pz)
{
	s.x = px;
	s.y = py;
	s.z = pz;
}

Vector3F Mesh::getScale() const
{
	return s;
}
Matrix4F Mesh::getScaleMatrix() const{ return Matrix4F(s, Matrix4F::SCALE); }

void Mesh::setRotation(Vector3F p)
{
	r = p;
}
void Mesh::setRotation(float x, float y, float z)
{
	r = Vector3F(x, y, z);
}
//Note: will return angles what they were written as.
Vector3F Mesh::getEulerAngles()
{
	return r;
}

void Mesh::calculateVerts()
{
	transform = Matrix4F::projection() * camMatrix() * Matrix4F::transform(p, r, s);
	for (unsigned i = 0; i < vertArray.size(); i++)
	{
		vertArray[i] = transform * oVertArray[i];
		vertArray[i].p.x = static_cast<int>(vertArray[i].p.x);
		vertArray[i].p.y = static_cast<int>(vertArray[i].p.y);
		vertArray[i].p.z = static_cast<int>(vertArray[i].p.z);
	}
}
//Keep in mind: v is the vertex being lit.
Vector3F Mesh::calculateLighting(const Vertex3F& v, const Face& f, const Vector3F eye) const
{
	Vector3F color;
	bool gotoNextLight = false;
	for (unsigned i = 0; i < sceneLights.size(); i++)
	{
		gotoNextLight = false;
		Vector3F rayOrigin;
		Vector3F rayDirection;
		float angle;
		if (sceneLights[i]->type == Light::LIGHT_DIRECTIONAL)
		{
			rayOrigin = Vector3F(
				sceneLights[i]->direction.x * sceneLights[i]->range - v.p.x,
				sceneLights[i]->direction.y * sceneLights[i]->range - v.p.y,
				sceneLights[i]->direction.z * sceneLights[i]->range - v.p.z
				);
			rayDirection = sceneLights[i]->direction;
		}
		else if (sceneLights[i]->type == Light::LIGHT_POINT)
		{
			rayOrigin = sceneLights[i]->origin;
			rayDirection = (v.p - rayOrigin).normalize();
		}
		else if (sceneLights[i]->type == Light::LIGHT_SPOTLIGHT)
		{
			rayOrigin = sceneLights[i]->origin;
			rayDirection = (v.p - rayOrigin).normalize();
			//If the angle between the direction the spotlight is firing and
			//The position in question is greater than the spotlight's angle,
			angle = angleBetween(rayDirection, sceneLights[i]->direction);
			if (angle > sceneLights[i]->angle)
			{
				//Skip this light, this light isn't hitting the object
				continue;
			}
		}
		float distToV = (v.p - rayOrigin).magnitude();
		//If v is out of the range of the light
		if (sceneLights[i]->type != Light::LIGHT_DIRECTIONAL && distToV > sceneLights[i]->range){continue;}
		for (unsigned j = 0; j < sceneMeshes.size(); j++)
		{
			if (false/*this != sceneMeshes[j] && rayIntersectsBoundingSphere(rayOrigin, rayDirection, sceneMeshes[j])*/)
			{
				for (unsigned k = 0; k < sceneMeshes[j]->faces.size(); k++)
				{
					if (&f == &sceneMeshes[j]->faces[k]){ continue; }
					Vector3F faceNormal = crossProduct(
						sceneMeshes[j]->faces[k].vert1->p,
						sceneMeshes[j]->faces[k].vert2->p,
						sceneMeshes[j]->faces[k].vert3->p
						).normalize();
					float divBy = dotProduct(rayDirection, faceNormal);
					//Face & Ray are parallel if divBy == 0. We want to skip to the next face then
					if (divBy == 0){ continue; }
					float rayDist =
						dotProduct(
						sceneMeshes[j]->faces[k].vert1->p - rayOrigin,
						faceNormal
						) / divBy;
					//If v is closer to the light, skip this face
					if (rayDist >= distToV || rayDist < 0){ continue; }
					Vector3F rayOntoFace = rayOrigin + rayDirection * rayDist;
					float cp2 = crossProductZ(sceneMeshes[j]->faces[k].vert2->p, sceneMeshes[j]->faces[k].vert3->p, rayOntoFace);
					if (crossProductZ(sceneMeshes[j]->faces[k].vert1->p, sceneMeshes[j]->faces[k].vert2->p, rayOntoFace) - cp2 > .001f ||
						cp2 - crossProductZ(sceneMeshes[j]->faces[k].vert3->p, sceneMeshes[j]->faces[k].vert1->p, rayOntoFace) > .001f)
					{
						gotoNextLight = true;
						break;
					}
					else
					{
						//std::cout << "Didn't hit" << std::endl;
					}
				}
			}
			if (gotoNextLight){ break; }
		}
		if (!gotoNextLight)
		{
			Vector3F refRay = reflectRay(rayDirection, v.n).normalize();
			float dotLightNorm = std::max(cosBetween(refRay, v.n), 0.0f);
			float dotRefView;
			if (backLit)
			{
				dotRefView = pow(cosBetween(refRay, eye), shineCoeff);
			}
			else
			{
				dotRefView = pow(std::max(cosBetween(rayDirection, eye), 0.0f), shineCoeff)*shineCoeff*shineCoeff;
			}
			if ((dotLightNorm + dotRefView) > 0)
			{
				if (sceneLights[i]->type == Light::LIGHT_DIRECTIONAL)
				{
					color += sceneLights[i]->color * (sceneLights[i]->intensity * (dotLightNorm + dotRefView));
				}
				else if (sceneLights[i]->type == Light::LIGHT_POINT)
				{
					color += sceneLights[i]->color * (sceneLights[i]->intensity * (1 - std::min(distToV / sceneLights[i]->range, 1.0f)) * (dotLightNorm + dotRefView));
				}
				else if (sceneLights[i]->type == Light::LIGHT_SPOTLIGHT)
				{
					color += sceneLights[i]->color * (sceneLights[i]->intensity * sin(2*(1-angle / sceneLights[i]->angle)) * (1 - std::min(distToV / sceneLights[i]->range, 1.0f)) * (dotLightNorm + dotRefView));
				}
			}
			
		}
	}
	return color;
}
void Mesh::BarryLerpPhong(const Face& face) const
{
	//Hardcode eye normal, for now
	Vector3F eye(0, 0, 1);

	Vertex3F vBottom = *face.vert1;
	Vertex3F vMiddle = *face.vert2;
	Vertex3F vTop = *face.vert3;

	//Bubble sorts the 3 elements, from lowest to highest y value.
	if (vBottom.p.y > vMiddle.p.y || (vBottom.p.y == vMiddle.p.y && vBottom.p.x > vMiddle.p.x)){ std::swap(vBottom, vMiddle); }
	if (vMiddle.p.y > vTop.p.y || (vMiddle.p.y == vTop.p.y && vMiddle.p.x > vTop.p.x)){ std::swap(vMiddle, vTop); }
	if (vBottom.p.y > vMiddle.p.y || (vBottom.p.y == vMiddle.p.y && vBottom.p.x > vMiddle.p.x)){ std::swap(vBottom, vMiddle); }

	//get the change in x,z,normal, etc over dy
	Vertex3F left_d = get_d_dy(vBottom, vMiddle);
	Vertex3F right_d = get_d_dy(vBottom, vTop);

	//Checks to see if the midpoint in the triangle is on the left or not
	bool midIsLeft = left_d.p.x < right_d.p.x;

	//start the leftMost, bottom points
	Vertex3F v1 = vBottom;
	Vertex3F v2 = vBottom;
	Vertex3F v = vBottom;

	if (vBottom.p.y == vMiddle.p.y){
		//We need to move p2 over to vMiddle, so we can have proper colors and positions
		v2 = vMiddle;
		//Needed because slopes are inaccurate for equal y values.
		midIsLeft = false;
	}

	//if they were assigned to the wrong sides, flip them
	if (!midIsLeft)
	{
		std::swap(left_d, right_d);
	}

	//Bottom of eq. guaranteed to be non-negative, so std::max with 1 is okay.
	Vertex3F dv_dx = (v2 - v1) / std::max(1.0f, v2.p.x - v1.p.x);
	//Keep the y value between the bounds of the window
	int topY = std::min((int)ceilf(vTop.p.y + 1), WINDOW_HEIGHT);
	int botY = std::max((int)vBottom.p.y, 0);
	if (botY == 0 && topY >= 0)
	{
		//If the middle point also starts below the screen, we need to flip the correct dx
		//And move the corresponding dx to the proper location.
		if (vMiddle.p.y < 0)
		{
			if (midIsLeft)
			{
				left_d = get_d_dy(vMiddle, vTop);

				v1 = vMiddle - left_d * vMiddle.p.y;
				v2 -= right_d * vBottom.p.y;
			}
			else
			{
				right_d = get_d_dy(vMiddle, vTop);

				v2 = vMiddle - right_d * vMiddle.p.y;
				v1 -= left_d * vBottom.p.y;
			}
		}
		else
		{
			v1 -= left_d * vBottom.p.y;
			v2 -= right_d * vBottom.p.y;
		}
	}

	for (int y = botY; y < topY; y++)
	{
		//std::cout << y << std::endl;
		v = v1;
		dv_dx = (v2 - v1) / std::max(1.0f, v2.p.x - v1.p.x);
		int topX = std::min((int)ceilf(v2.p.x + 1), WINDOW_WIDTH);
		int botX = std::max((int)v1.p.x, 0);
		//pushes z over if the beginning of the triangle is out of bounds
		if (botX == 0 && topX >= 0){ v -= dv_dx * v1.p.x; }
		for (int x = botX; x < topX; x++)
		{
			if (v.p.z < z_buf[x][y] && v.p.z >= 0)
			{
				v.n.normalize();
				if (lightAndShadows)
				{
					v.p = Vector3F(x, y, v.p.z);
					Vector3F barryCalc(
						crossProduct(vBottom.p, vMiddle.p, v.p).magnitude(),
						crossProduct(vBottom.p, vTop.p, v.p).magnitude(),
						crossProduct(vMiddle.p, vTop.p, v.p).magnitude()
						);
					barryCalc /= (barryCalc.x + barryCalc.y + barryCalc.z);
					v.n = (vTop.n*barryCalc.x + vMiddle.n*barryCalc.y + vBottom.n*barryCalc.z).normalize();
					v.c = vTop.c*barryCalc.x + vMiddle.c*barryCalc.y + vBottom.c*barryCalc.z;

					float ambientIntensity = (std::max(1 - cosBetween(eye, v.n), 0.0f)) * 0.25f;
					//std::cout << ambientIntensity << endl;
					Vector3F color = calculateLighting(v, face, eye);
					glColor3f(v.c.x * (color.x + ambientIntensity),
						v.c.y * (color.y + ambientIntensity),
						v.c.z * (color.z + ambientIntensity));
				}
				else
				{
					float ambientIntensity = (1 - dotProduct(eye, v.n)) / 2.0f;
					glColor3f(ambientIntensity, ambientIntensity, ambientIntensity);
				}

				//glColor3f(v.c.x * intensity, v.c.y * intensity, v.c.z * intensity);
				//glColor3f(1,1,1);
				glVertex2i(x, y);
				z_buf[x][y] = v.p.z;
			}
			v += dv_dx;
		}

		//Check if we hit the point to swap the slopes
		if (y == vMiddle.p.y)
		{
			//I just ternaried so hard.
			//Yes, this is legal code.
			(midIsLeft ? left_d : right_d) = get_d_dy(vMiddle, vTop);
		}
		else
		{
			v1 += left_d;
			v2 += right_d;
		}
	}
}
void Mesh::PhongTriangle(const Face& face) const
{
	//Hardcode eye normal, for now
	Vector3F eye(0, 0, 1);

	Vertex3F vBottom = *face.vert1;
	Vertex3F vMiddle= *face.vert2;
	Vertex3F vTop = *face.vert3;

	//Bubble sorts the 3 elements, from lowest to highest y value.
	if (vBottom.p.y > vMiddle.p.y || (vBottom.p.y == vMiddle.p.y && vBottom.p.x > vMiddle.p.x)){ std::swap(vBottom, vMiddle); }
	if (vMiddle.p.y > vTop.p.y || (vMiddle.p.y == vTop.p.y && vMiddle.p.x > vTop.p.x)){ std::swap(vMiddle, vTop); }
	if (vBottom.p.y > vMiddle.p.y || (vBottom.p.y == vMiddle.p.y && vBottom.p.x > vMiddle.p.x)){ std::swap(vBottom, vMiddle); }

	//get the change in x,z,normal, etc over dy
	Vertex3F left_d = get_d_dy(vBottom, vMiddle);
	Vertex3F right_d = get_d_dy(vBottom, vTop);

	//Checks to see if the midpoint in the triangle is on the left or not
	bool midIsLeft = left_d.p.x < right_d.p.x;

	//start the leftMost, bottom points
	Vertex3F v1 = vBottom;
	Vertex3F v2 = vBottom;
	Vertex3F v = vBottom;

	if (vBottom.p.y == vMiddle.p.y){
		//We need to move p2 over to vMiddle, so we can have proper colors and positions
		v2 = vMiddle;
		//Needed because slopes are inaccurate for equal y values.
		midIsLeft = false;
	}

	//if they were assigned to the wrong sides, flip them
	if (!midIsLeft)
	{
		std::swap(left_d, right_d);
	}

	//Bottom of eq. guaranteed to be non-negative, so std::max with 1 is okay.
	Vertex3F dv_dx = (v2 - v1) / std::max(1.0f, v2.p.x - v1.p.x);
	//Keep the y value between the bounds of the window
	int topY = std::min((int)ceilf(vTop.p.y + 1), WINDOW_HEIGHT);
	int botY = std::max((int)vBottom.p.y, 0);
	if (botY == 0 && topY >= 0)
	{
		//If the middle point also starts below the screen, we need to flip the correct dx
		//And move the corresponding dx to the proper location.
		if (vMiddle.p.y < 0)
		{
			if (midIsLeft)
			{
				left_d = get_d_dy(vMiddle, vTop);

				v1 = vMiddle - left_d * vMiddle.p.y;
				v2 -= right_d * vBottom.p.y;
			}
			else
			{
				right_d = get_d_dy(vMiddle, vTop);

				v2 = vMiddle - right_d * vMiddle.p.y;
				v1 -= left_d * vBottom.p.y;
			}
		}
		else
		{
			v1 -= left_d * vBottom.p.y;
			v2 -= right_d * vBottom.p.y;
		}
	}

	for (int y = botY; y < topY; y++)
	{
		//std::cout << y << std::endl;
		v = v1;
		dv_dx = (v2 - v1) / std::max(1.0f, v2.p.x - v1.p.x);
		int topX = std::min((int)ceilf(v2.p.x + 1), WINDOW_WIDTH);
		int botX = std::max((int)v1.p.x, 0);
		//pushes z over if the beginning of the triangle is out of bounds
		if (botX == 0 && topX >= 0){ v -= dv_dx * v1.p.x; }
		for (int x = botX; x < topX; x++)
		{
			if (v.p.z < z_buf[x][y] && v.p.z >= 0)
			{
				v.n.normalize();
				if (lightAndShadows)
				{
					v.p = Vector3F(x, y, v.p.z); v.n.normalize();
					float ambientIntensity = (std::max(1-cosBetween(eye, v.n), 0.0f)) * 0.25f;
					//std::cout << ambientIntensity << endl;
					Vector3F color = calculateLighting(v, face, eye);
					glColor3f(v.c.x * (color.x + ambientIntensity),
						v.c.y * (color.y + ambientIntensity),
						v.c.z * (color.z + ambientIntensity));
				}
				else
				{
					float ambientIntensity = (1 - dotProduct(eye, v.n))/2.0f;
					glColor3f(ambientIntensity, ambientIntensity, ambientIntensity);
				}
				
				//glColor3f(v.c.x * intensity, v.c.y * intensity, v.c.z * intensity);
				//glColor3f(1,1,1);
				glVertex2i(x, y);
				z_buf[x][y] = v.p.z;
			}
			v += dv_dx;
		}

		//Check if we hit the point to swap the slopes
		if (y == vMiddle.p.y)
		{
			//I just ternaried so hard.
			//Yes, this is legal code.
			(midIsLeft ? left_d : right_d) = get_d_dy(vMiddle, vTop);
		}
		else
		{
			v1 += left_d;
			v2 += right_d;
		}
	}
}

void Mesh::smoothTriangle(const Face& face) const
{
	Vertex3F vBottom = *face.vert1;
	Vertex3F vMiddle = *face.vert2;
	Vertex3F vTop = *face.vert3;

	//Bubble sorts the 3 elements, from lowest to highest y value.
	if (vBottom.p.y > vMiddle.p.y || (vBottom.p.y == vMiddle.p.y && vBottom.p.x > vMiddle.p.x)){ std::swap(vBottom, vMiddle); }
	if (vMiddle.p.y > vTop.p.y || (vMiddle.p.y == vTop.p.y && vMiddle.p.x > vTop.p.x)){ std::swap(vMiddle, vTop); }
	if (vBottom.p.y > vMiddle.p.y || (vBottom.p.y == vMiddle.p.y && vBottom.p.x > vMiddle.p.x)){ std::swap(vBottom, vMiddle); }

	//get the change in x,z,normal, etc over dy
	Vertex3F left_d = get_d_dy(vBottom, vMiddle);
	Vertex3F right_d = get_d_dy(vBottom, vTop);

	//Checks to see if the midpoint in the triangle is on the left or not
	bool midIsLeft = left_d.p.x < right_d.p.x;

	//start the leftMost, bottom points
	Vertex3F v1 = vBottom;
	Vertex3F v2 = vBottom;

	Vertex3F v = vBottom;

	if (vBottom.p.y == vMiddle.p.y){
		//We need to move p2 over to vMiddle, so we can have proper colors and positions
		v2 = vMiddle;
		//Needed because slopes are inaccurate for equal y values.
		midIsLeft = false;
	}

	//if they were assigned to the wrong sides, flip them
	if (!midIsLeft)
	{
		std::swap(left_d, right_d);
	}

	//Bottom of eq. guaranteed to be non-negative, so std::max with 1 is okay.
	Vertex3F dv_dx = (v2 - v1) / std::max(1.0f, v2.p.x - v1.p.x);
	//Keep the y value between the bounds of the window
	int topY = std::min((int)ceilf(vTop.p.y+1), WINDOW_HEIGHT);
	int botY = std::max((int)vBottom.p.y, 0);

	if (botY == 0 && topY >= 0)
	{
		//If the middle point also starts below the screen, we need to flip the correct dx
		//And move the corresponding dx to the proper location.
		if (vMiddle.p.y < 0)
		{
			if (midIsLeft)
			{
				left_d = get_d_dy(vMiddle, vTop);

				v1 = vMiddle - left_d * vMiddle.p.y;
				v2 -= right_d * vBottom.p.y;
			}
			else
			{
				right_d = get_d_dy(vMiddle, vTop);

				v2 = vMiddle - right_d * vMiddle.p.y;
				v1 -= left_d * vBottom.p.y;
			}
		}
		else
		{
			v1 -= left_d * vBottom.p.y;
			v2 -= right_d * vBottom.p.y;
		}
	}

	for (int y = botY; y < topY; y++)
	{
		//std::cout << y << std::endl;
		v = v1;
		dv_dx = (v2 - v1) / std::max(1.0f, v2.p.x - v1.p.x);
		int topX = std::min((int)ceilf(v2.p.x + 1), WINDOW_WIDTH);
		int botX = std::max((int)v1.p.x, 0);
		//pushes z over if the beginning of the triangle is out of bounds
		if (botX == 0 && topX >= 0){ v -= dv_dx * v1.p.x; }
		for (int x = botX; x < topX; x++)
		{
			if (v.p.z < z_buf[x][y] && v.p.z >= 0)
			{
				v.n.normalize();
					Vector3F eye(0, 0, 1);
					float ambientIntensity = 1 - ((eye.x * v.n.x) + (eye.y * v.n.y) + (eye.z * v.n.z));
					ambientIntensity *= .5f;
					glColor3f(ambientIntensity, ambientIntensity, ambientIntensity);

				//glColor3f(v.c.x * intensity, v.c.y * intensity, v.c.z * intensity);
				//glColor3f(1,1,1);
				glVertex2i(x, y);
				z_buf[x][y] = v.p.z;
			}
			v += dv_dx;
		}

		//Check if we hit the point to swap the slopes
		if (y == vMiddle.p.y)
		{
			//I just ternaried so hard.
			//Yes, this is legal code.
			(midIsLeft ? left_d : right_d) = get_d_dy(vMiddle, vTop);
		}
		else
		{
			v1 += left_d;
			v2 += right_d;
		}
	}
}

void Mesh::drawTriangle(Vertex3F vBottom, Vertex3F vMiddle, Vertex3F vTop) const
{
	//Eye dotted with the normal, giving us the color at that point.
	float shade = (1-dotProduct(crossProduct(vBottom.p, vMiddle.p, vTop.p).normalize(), Vector3F(0, 0, 1)))/2.0f;
	//Bubble sorts the 3 elements, from lowest to highest y value.
	if (vBottom.p.y > vMiddle.p.y || (vBottom.p.y == vMiddle.p.y && vBottom.p.x > vMiddle.p.x)){ std::swap(vBottom, vMiddle); }
	if (vMiddle.p.y > vTop.p.y || (vMiddle.p.y == vTop.p.y && vMiddle.p.x > vTop.p.x)){ std::swap(vMiddle, vTop); }
	if (vBottom.p.y > vMiddle.p.y || (vBottom.p.y == vMiddle.p.y && vBottom.p.x > vMiddle.p.x)){ std::swap(vBottom, vMiddle); }

	//get the change in x,z,normal, etc over dy
	Vertex3F left_d = get_d_dy(vBottom, vMiddle);
	Vertex3F right_d = get_d_dy(vBottom, vTop);

	//Checks to see if the midpoint in the triangle is on the left or not
	bool midIsLeft = left_d.p.x < right_d.p.x;

	//start the leftMost, bottom points
	float x1 = vBottom.p.x, x2 = vBottom.p.x;
	float z1 = vBottom.p.z, z2 = vBottom.p.z;
	if (vBottom.p.y == vMiddle.p.y){
		//Need to move x2 and z2 over, because the first line will
		//Only be 1 point if we don't move it over.
		x2 = vMiddle.p.x;
		z2 = vMiddle.p.z;
		//Needed because slopes are inaccurate for equal y values.
		midIsLeft = false;
	}

	//if they were assigned to the wrong sides, flip them
	if (!midIsLeft)
	{
		std::swap(left_d, right_d);
	}

	float z = z1;
	//Bottom of eq. guaranteed to be non-negative, so std::max with 1 is okay.
	float dz = (z2 - z1) / std::max(1.0f, x2 - x1);
	//Keep the y value between the bounds of the window
	int topY = std::min((int)ceilf(vTop.p.y), WINDOW_HEIGHT);
	int botY = std::max((int)vBottom.p.y, 0);

	if (botY == 0 && topY >= 0)
	{
		//If the middle point also starts below the screen, we need to flip the correct dx
		//And move the corresponding dx to the proper location.
		if (vMiddle.p.y < 0)
		{
			if (midIsLeft)
			{
				left_d = get_d_dy(vMiddle, vTop);
				x1 = vMiddle.p.x - left_d.p.x * vMiddle.p.y;
				x2 -= right_d.p.x * vBottom.p.y;

				z1 = vMiddle.p.z - left_d.p.z * vMiddle.p.y;
				z2 -= right_d.p.z * vBottom.p.y;
			}
			else
			{
				right_d = get_d_dy(vMiddle, vTop);
				x2 = vMiddle.p.x - right_d.p.x * vMiddle.p.y;
				x1 -= left_d.p.x * vBottom.p.y;

				z2 = vMiddle.p.z - right_d.p.z * vMiddle.p.y;
				z1 -= left_d.p.z * vBottom.p.y;
			}
		}
		else
		{
			x1 -= left_d.p.x * vBottom.p.y;
			x2 -= right_d.p.x * vBottom.p.y;
			z1 -= left_d.p.z * vBottom.p.y;
			z2 -= right_d.p.z * vBottom.p.y;
		}
	}

	for (int y = botY; y < topY; y++)
	{
		//std::cout << y << std::endl;
		z = z1;
		dz = (z2 - z1) / std::max(1.0f, x2 - x1);
		int topX = std::min((int)ceilf(x2), WINDOW_WIDTH);
		int botX = std::max((int)x1, 0);
		//pushes z over if the beginning of the triangle is out of bounds
		if (botX == 0 && topX >= 0){ z -= dz * (int)x1; }
		for (int x = botX; x < topX; x++)
		{
			if (z < z_buf[x][y] && z >= 0)
			{
				glColor3f(shade, shade, shade);

				//glColor3f(v.c.x * intensity, v.c.y * intensity, v.c.z * intensity);
				//glColor3f(1,1,1);
				glVertex2i(x, y);
				z_buf[x][y] = z;
				/*glColor3f(vBottom.c.x, vBottom.c.y, vBottom.c.z);
				glVertex2i(x, y);
				z_buf[x][y] = z;*/
			}
			z += dz;
		}

		//Check if we hit the point to swqp the slopes
		if (y == vMiddle.p.y)
		{
			//I just ternaried so hard
			//Yes, this is correct syntax
			(midIsLeft ? left_d : right_d) = get_d_dy(vMiddle, vTop);
		}
		else
		{
			x1 += left_d.p.x;
			x2 += right_d.p.x;
			z1 += left_d.p.z;
			z2 += right_d.p.z;
		}
	}
}

void Mesh::drawQuad(Vertex3F v1, Vertex3F v2, Vertex3F v3, Vertex3F v4)
{
	drawTriangle(v1, v2, v3);
	drawTriangle(v3, v4, v1);
}

void Mesh::drawMesh()
{
	if (renderPhong){
		std::cout << "Renderering Phong" << std::endl;
	}
	for (unsigned i = 0; i < faces.size(); i++)
	{
		//Backface culling
		if (!backfaceCulling || crossProductZ(faces[i].vert2->p, faces[i].vert1->p, faces[i].vert3->p) > 0)
		{
			if (wireFrame)
			{
				drawLine2i(faces[i].vert1->p, faces[i].vert2->p);
				drawLine2i(faces[i].vert2->p, faces[i].vert3->p);
				drawLine2i(faces[i].vert1->p, faces[i].vert3->p);
			}
			else
			{
				if (renderPhong)
				{
					PhongTriangle(faces[i]);
				}
				else if (smoothTriangles)
				{
					smoothTriangle(faces[i]);
				}
				else if (barryPhong)
				{
					BarryLerpPhong(faces[i]);
				}
				else
				{
					drawTriangle(*faces[i].vert1, *faces[i].vert2, *faces[i].vert3);
				}
			}
		}
	}
}