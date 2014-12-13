#include "stdafx.h"
#include <math.h>
#include "Vector3F.h"

Vector3F::Vector3F() : x(0.0f), y(0.0f), z(0.0f) {}
Vector3F::Vector3F(const float & nx, const float & ny, const float & nz) : x(nx), y(ny), z(nz) {}

Vector3F Vector3F::operator+(const Vector3F & rhs) const { return Vector3F(x + rhs.x, y + rhs.y, z + rhs.z); }
Vector3F Vector3F::operator-(const Vector3F & rhs) const { return Vector3F(x - rhs.x, y - rhs.y, z - rhs.z); }
Vector3F Vector3F::operator-() const { return Vector3F(-x, -y, -z); }
Vector3F Vector3F::operator*(float val) const { return Vector3F(x * val, y * val, z * val); }
Vector3F Vector3F::operator/(float val) const { return Vector3F(x / val, y / val, z / val); }
Vector3F Vector3F::operator+=(const Vector3F & rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
Vector3F Vector3F::operator-=(const Vector3F & rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
Vector3F Vector3F::operator*=(float val) { x *= val; y *= val; z *= val; return *this; }
Vector3F Vector3F::operator/=(float val) { x /= val; y /= val; z /= val; return *this; }
Vector3F Vector3F::operator=(const Vector3F & rhs) { x = rhs.x; y = rhs.y; z = rhs.z; return *this; }

Vector3F Vector3F::normalize(){ float m = magnitude(); x /= m; y /= m; z /= m; return *this; }
float Vector3F::magnitude() const{ return sqrt(x*x + y*y + z*z); }
float Vector3F::sqrMagnitude() const{ return x*x + y*y + z*z; }

const Vector3F Vector3F::up = Vector3F(0, 1, 0);
const Vector3F Vector3F::down = Vector3F(0, -1, 0);
const Vector3F Vector3F::left = Vector3F(-1, 0, 0);
const Vector3F Vector3F::right = Vector3F(1, 0, 0);
const Vector3F Vector3F::forward = Vector3F(0, 0, 1);
const Vector3F Vector3F::back = Vector3F(0, 0, -1);