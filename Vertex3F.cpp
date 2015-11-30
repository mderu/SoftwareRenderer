#include "stdafx.h"
#include "Vector3F.h"
#include "Vertex3F.h"
#include "Matrix4F.h"


Vertex3F::Vertex3F(){}
Vertex3F::Vertex3F(Vector3F p) : p(p) {}
Vertex3F::Vertex3F(const Vertex3F& rhs):p(rhs.p), n(rhs.n), c(rhs.c){}
Vertex3F::~Vertex3F(){}
Vertex3F::Vertex3F(Vector3F p, Vector3F n, Vector3F c) : p(p), n(n), c(c){}

Vertex3F Vertex3F::operator-(const Vertex3F & rhs) const { return Vertex3F(p - rhs.p, n - rhs.n, c - rhs.c); }
Vertex3F Vertex3F::operator+(const Vertex3F & rhs) const { return Vertex3F(p + rhs.p, n + rhs.n, c + rhs.c); }
Vertex3F Vertex3F::operator-=(const Vertex3F & rhs) { p -= rhs.p; n -= rhs.n; c -= rhs.c; return *this; }
Vertex3F Vertex3F::operator+=(const Vertex3F & rhs) { p += rhs.p; n += rhs.n; c += rhs.c; return *this; }
Vertex3F Vertex3F::operator=(const Vertex3F & rhs) { p = rhs.p; n = rhs.n; c = rhs.c; return *this; }
Vertex3F Vertex3F::operator*(const float& f) const { return Vertex3F(p * f, n * f, c * f); }
Vertex3F Vertex3F::operator/(const float& f) const { return Vertex3F(p / f, n / f, c / f); }
