#include "stdafx.h"
#include <vector>
#include "Vector3F.h"
#include "Vertex3F.h"
#include "Matrix4F.h"
#include <iostream>

//Private Static Member stdRot
Matrix4F::MatrixType Matrix4F::stdRot = MatrixType::ROTATE_DEG;
void Matrix4F::useDegrees(bool useDeg = true){ stdRot = useDeg ? MatrixType::ROTATE_DEG : MatrixType::ROTATE_RAD; }
void Matrix4F::useRadians(bool useDeg = true){ stdRot = useDeg ? MatrixType::ROTATE_RAD : MatrixType::ROTATE_DEG; }
Matrix4F::MatrixType Matrix4F::getRotType(){ return stdRot; }

//Constructors

Matrix4F::Matrix4F() : matrix(4, std::vector<float>(4, 0)){}
Matrix4F::Matrix4F(Matrix4F& m) { matrix = m.matrix; }
Matrix4F::Matrix4F(const float & tx, const float & ty, const float & tz, MatrixType T) : matrix(4, std::vector<float>(4, 0))
{
	if (T == SCALE)
	{
		matrix[0][0] = tx;
		matrix[1][1] = ty;
		matrix[2][2] = tz;
		matrix[3][3] = 1;
	}
	else if (T == TRANSLATE)
	{
		for (int i = 0; i < 4; i++){ matrix[i][i] = 1; }
		matrix[0][3] = tx;
		matrix[1][3] = ty;
		matrix[2][3] = tz;
	}
	else if (T == ROTATE_RAD || T == ROTATE_DEG)
	{
		//c = cos, s = sin
		float isDeg = (T == ROTATE_DEG ? PI / 180.0f : 1.0f);
		float cx = cos(tx * isDeg), cy = cos(ty * isDeg), cz = cos(tz * isDeg);
		float sx = sin(tx * isDeg), sy = sin(ty * isDeg), sz = sin(tz * isDeg);

		//Note: Result is the multiplication of Rx * Ry *Rz, in that order
		matrix[0][0] = cy*cz;
		matrix[1][0] = sx*sy*cz + cx*sz;
		matrix[2][0] = -cx*sy*cz + sx*sz;
		//[3][0] already set to 0

		matrix[0][1] = -cy*sz;
		matrix[1][1] = -sx*sy*sz + cx*cz;
		matrix[2][1] = cx*sy*sz + sx*cz;
		//[3][1] also already 0

		matrix[0][2] = sy;
		matrix[1][2] = -sx*cy;
		matrix[2][2] = cx*cy;
		//[3][2] = 0

		matrix[3][3] = 1.0f;
	}
}
Matrix4F::Matrix4F(const float & angle, const Axis axis) : matrix(4, std::vector<float>(4, 0))
{
	if (axis == AXIS_X)
	{
		matrix[0][0] = 1;
		matrix[2][1] = sin(angle * PI / 180.0f);
		matrix[1][1] = cos(angle * PI / 180.0f);;
		matrix[1][2] = -sin(angle * PI / 180.0f);
		matrix[2][2] = cos(angle * PI / 180.0f);
	}
	else if (axis == AXIS_Y)
	{
		matrix[0][0] = cos(angle * PI / 180.0f);
		matrix[0][2] = sin(angle * PI / 180.0f);
		matrix[1][1] = 1;
		matrix[2][0] = -sin(angle * PI / 180.0f);
		matrix[2][2] = cos(angle * PI / 180.0f);
	}
	else //if(axis == AXIS_Z)
	{
		matrix[0][0] = cos(angle * PI / 180.0f);
		matrix[1][0] = sin(angle * PI / 180.0f);
		matrix[1][1] = cos(angle * PI / 180.0f);
		matrix[0][1] = -sin(angle * PI / 180.0f);
		matrix[2][2] = 1;
	}
	matrix[3][3] = 1;
}
Matrix4F::Matrix4F(Vector3F t, MatrixType T) : Matrix4F(t.x, t.y, t.z, T){}

//Operators

Vector3F Matrix4F::operator*(const Vector3F & vec) const
{
	Vector3F retMe;
	retMe.x = matrix[0][0] * vec.x + matrix[0][1] * vec.y + matrix[0][2] * vec.z + matrix[0][3];
	retMe.y = matrix[1][0] * vec.x + matrix[1][1] * vec.y + matrix[1][2] * vec.z + matrix[1][3];
	retMe.z = matrix[2][0] * vec.x + matrix[2][1] * vec.y + matrix[2][2] * vec.z + matrix[2][3];
	return retMe;
}
Vertex3F Matrix4F::operator*(const Vertex3F & vert) const
{
	Vertex3F retMe = vert;
	retMe.p.x = matrix[0][0] * vert.p.x + matrix[0][1] * vert.p.y + matrix[0][2] * vert.p.z + matrix[0][3];
	retMe.p.y = matrix[1][0] * vert.p.x + matrix[1][1] * vert.p.y + matrix[1][2] * vert.p.z + matrix[1][3];
	retMe.p.z = matrix[2][0] * vert.p.x + matrix[2][1] * vert.p.y + matrix[2][2] * vert.p.z + matrix[2][3];
	
	retMe.n.x = matrix[0][0] * vert.n.x + matrix[0][1] * vert.n.y + matrix[0][2] * vert.n.z;
	retMe.n.y = matrix[1][0] * vert.n.x + matrix[1][1] * vert.n.y + matrix[1][2] * vert.n.z;
	retMe.n.z = matrix[2][0] * vert.n.x + matrix[2][1] * vert.n.y + matrix[2][2] * vert.n.z;

	retMe.n.normalize();

	return retMe;
}
Matrix4F Matrix4F::operator*(const Matrix4F & rhs) const
{
	Matrix4F retMe;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				if (matrix[i][k] != 0 && rhs[k][j] != 0)
				{
					retMe[i][j] += matrix[i][k] * rhs[k][j];
				}
			}
		}
	}
	return retMe;
}

Matrix4F& Matrix4F::operator=(const Matrix4F & rhs) {
	matrix = rhs.matrix;
	return *this;
}

std::vector<float>& Matrix4F::operator[](int index) { return matrix[index]; }
const std::vector<float>& Matrix4F::operator[](int index) const { return matrix[index]; }


//Getters:

Vector3F Matrix4F::getTranslation()
{
	return Vector3F(matrix[0][3], matrix[1][3], matrix[2][3]);
}
Vector3F Matrix4F::getScale()
{
	return Vector3F(matrix[0][0], matrix[1][1], matrix[2][2]);
}


//Static Methods

Matrix4F Matrix4F::identity() {
	Matrix4F retMe;
	retMe[0][0] = retMe[1][1] = retMe[2][2] = retMe[3][3] = 1;
	return retMe;
}
Matrix4F Matrix4F::scale(Vector3F p) {
	Matrix4F retMe;
	retMe[0][0] = p.x;
	retMe[1][1] = p.y;
	retMe[2][2] = p.z;
	retMe[3][3] = 1.0f;
	return retMe;
}
Matrix4F Matrix4F::scale(float px, float py, float pz) {
	Matrix4F retMe;
	retMe[0][0] = px;
	retMe[1][1] = py;
	retMe[2][2] = pz;
	retMe[3][3] = 1.0f;
	return retMe;
}
Matrix4F Matrix4F::translate(Vector3F p) {
	Matrix4F retMe;
	retMe[0][3] = p.x;
	retMe[1][3] = p.y;
	retMe[2][3] = p.z;
	retMe[3][3] = 1.0f;
	return retMe;
}
Matrix4F Matrix4F::translate(float px, float py, float pz) {
	Matrix4F retMe;
	retMe[0][3] = px;
	retMe[1][3] = py;
	retMe[2][3] = pz;
	retMe[3][3] = 1.0f;
	return retMe;
}
Matrix4F Matrix4F::rotate(float px, float py, float pz)
{
	Matrix4F retMe;
	//c = cos, s = sin
	float isDeg = (stdRot == ROTATE_DEG ? PI / 180.0f : 1.0f);
	float cx = cos(px * isDeg), cy = cos(py * isDeg), cz = cos(pz * isDeg);
	float sx = sin(px * isDeg), sy = sin(py * isDeg), sz = sin(pz * isDeg);

	//Note: Result is the multiplication of Rx * Ry *Rz, in that order
	retMe[0][0] = cy*cz;
	retMe[1][0] = sx*sy*cz + cx*sz;
	retMe[2][0] = -cx*sy*cz + sx*sz;
	//[3][0] already set to 0

	retMe[0][1] = -cy*sz;
	retMe[1][1] = -sx*sy*sz + cx*cz;
	retMe[2][1] = cx*sy*sz + sx*cz;
	//[3][1] also already 0

	retMe[0][2] = sy;
	retMe[1][2] = -sx*cy;
	retMe[2][2] = cx*cy;
	//[3][2] = 0

	retMe[3][3] = 1.0f;
	return retMe;
}
Matrix4F Matrix4F::rotate(Vector3F p)
{
	return rotate(p.x, p.y, p.z);
}

//UNTESTED. May need to be moved into Util
Matrix4F Matrix4F::rotateAround(Vector3F point, Vector3F eulerAngles)
{
	Matrix4F retMe(eulerAngles, stdRot);
	//I did the math by hand here, so it might need some looking over.
	retMe[0][3] = -point.x*retMe[0][0] - point.y*retMe[1][0] - point.z*retMe[2][0];
	retMe[1][3] = -point.x*retMe[0][1] - point.y*retMe[1][1] - point.z*retMe[2][1];
	retMe[2][3] = -point.x*retMe[0][2] - point.y*retMe[1][2] - point.z*retMe[2][2];
	return retMe;
}

Matrix4F Matrix4F::transform(Vector3F position = Vector3F(0, 0, 0), Vector3F eulerAngles = Vector3F(0, 0, 0), Vector3F scale = Vector3F(0, 0, 0))
{
	Matrix4F retMe(eulerAngles, stdRot);
	for (unsigned i = 0; i < 4; i++)
	{
		retMe[0][i] *= scale.x;
		retMe[1][i] *= scale.y;
		retMe[2][i] *= scale.z;
	}

	retMe[0][3] = position.x;
	retMe[1][3] = position.y;
	retMe[2][3] = position.z;

	return retMe;
}
Matrix4F Matrix4F::projection()
{
	return projection(60.0f, 1.0f, 0.0f, 800.0f, true);
}
Matrix4F Matrix4F::projection(float fov = 45.0f, float aspect = 1.0f, float nearDist = 0.0f, float farDist = 800.0f, bool leftHanded  = true )
{
	/*Matrix4F result;
	//
	// General form of the Projection Matrix
	//
	// uh = Cot( fov/2 ) == 1/Tan(fov/2)
	// uw / uh = 1/aspect
	// 
	//   uw         0       0       0
	//    0        uh       0       0
	//    0         0      f/(f-n)  1
	//    0         0    -fn/(f-n)  0
	//
	// Make result to be identity first

	// check for bad parameters to avoid divide by zero:
	// if found, assert and return an identity matrix.
	if (fov <= 0 || aspect == 0)
	{
		exit(1);
	}
	float frustumDepth = 9000;
	float oneOverDepth = 1 / frustumDepth;

	result[1][1] = 1 / tan(0.5f * fov);
	result[0][0] = (leftHanded ? 1 : -1) * result[1][1] / aspect;
	result[2][2] = farDist * oneOverDepth;

	result[3][2] = (-farDist * nearDist) * oneOverDepth;

	result[2][3] = 1;
	result[3][3] = 0;
	return result;*/
	return Matrix4F::identity();
}