#ifndef __Vector3F_H__
#define __Vector3F_H__

class Vector3F
{
public:
  float x;
  float y;
  float z;

  Vector3F();
  Vector3F(const float & nx, const float & ny, const float & nz);
  
  Vector3F operator+(const Vector3F & rhs) const;
  Vector3F operator-(const Vector3F & rhs) const;
  Vector3F operator-() const;
  Vector3F operator*(float val) const;
  Vector3F operator/(float val) const;
  Vector3F operator+=(const Vector3F & rhs);
  Vector3F operator-=(const Vector3F & rhs);
  Vector3F operator*=(float val);
  Vector3F operator/=(float val);
  Vector3F operator=(const Vector3F & rhs);

  //Vector3F operator=(const Matrix4F & rhs);
  //float crossProduct(Vector3F a, Vector3F b, Vector3F c){ return (a.  * b.y - a.y * b.x;}
  Vector3F normalize();
  float magnitude() const;
  float sqrMagnitude() const;

  static const Vector3F up;
  static const Vector3F down;
  static const Vector3F left;
  static const Vector3F right;
  static const Vector3F forward;
  static const Vector3F back;
};

#endif
