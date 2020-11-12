#ifndef GEOMETRY_H
#define GEOMETRY_H

// Code for Vector math from Scratchapixel: https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI (3.14159265358979323846f)
#endif 

#ifndef M_PI_2
#define M_PI_2 (1.570796327f)
#endif 

template<typename T>
class Vector3
{
public:
	Vector3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vector3(T xx) : x(xx), y(xx), z(xx) {}
	Vector3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}

	// const operators
	Vector3 operator + (const Vector3 &v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	Vector3 operator - (const Vector3 &v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 operator - () const
	{
		return Vector3(-x, -y, -z);
	}
	Vector3 operator * (const T &r) const
	{
		return Vector3(x * r, y * r, z * r);
	}
	Vector3 operator * (const Vector3 &v) const
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}
	T dotProduct(const Vector3<T> &v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}
	Vector3 operator / (const T &r) const
	{
		return Vector3(x / r, y / r, z / r);
	}

	Vector3& operator /= (const T &r)
	{
		x /= r, y /= r, z /= r; return *this;
	}
	Vector3& operator *= (const T &r)
	{
		x *= r, y *= r, z *= r; return *this;
	}
	Vector3& operator += (const Vector3 &v)
	{
		x += v.x, y += v.y, z += v.z; return *this;
	}

	Vector3 crossProduct(const Vector3<T> &v) const
	{
		return Vector3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
	T norm() const
	{
		return x * x + y * y + z * z;
	}
	T length() const
	{
		return sqrt(norm());
	}


	const T& operator [] (uint8_t i) const { return (&x)[i]; }
	T& operator [] (uint8_t i) { return (&x)[i]; }
	Vector3& normalize()
	{
		T n = norm();
		if (n > 0) {
			T factor = 1 / sqrt(n);
			x *= factor, y *= factor, z *= factor;
		}

		return *this;
	}

	friend Vector3 operator * (const T &r, const Vector3 &v)
	{
		return Vector3<T>(v.x * r, v.y * r, v.z * r);
	}
	friend Vector3 operator / (const T &r, const Vector3 &v)
	{
		return Vector3<T>(r / v.x, r / v.y, r / v.z);
	}

	friend std::ostream& operator << (std::ostream &s, const Vector3<T> &v)
	{
		return s << '[' << v.x << ' ' << v.y << ' ' << v.z << ']';
	}

	T x, y, z;
};

typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

#endif
