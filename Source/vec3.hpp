#ifndef VEC3_HPP
#define VEC3_HPP

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3 {
public:

	// Fields

	union {

		double data[3];
		struct {
			double x;
			double y;
			double z;
		};
		struct {
			double r;
			double g;
			double b;
		};

	};

	// Constructors

	vec3() : data{ 0,0,0 } {}
	vec3(double x, double y, double z) : data{ x,y,z } {}


	// Operators

	vec3 operator-() const { return vec3(-x, -y, -z); }
	double operator[](int i) const { return data[i]; }
	double& operator[](int i) { return data[i]; }

	vec3& operator+=(const vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3& operator*=(const double& t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	vec3 operator/=(const double& t)
	{
		return *this *= 1 / t;
	}

	// Math functions

	double length() const 
	{
		return sqrt(length_squared());
	}

	double length_squared() const 
	{
		return x * x + y * y + z * z;
	}

	// Utility Functions

	inline static vec3 random() {
		return vec3(random_double(), random_double(), random_double());
	}

	inline static vec3 random(double min, double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}
	
};

// Type aliases for vec3
using point3 = vec3;	// 3D Point
using color3 = vec3;	// RGB Color

// Utility Functions
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec3 operator*(double t, const vec3& v) {
	return vec3(t * v.x, t * v.y, t * v.z);
}

inline vec3 operator*(const vec3& v, double t) {
	return t * v;
}

inline vec3 operator/(vec3 v, double t) {
	return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v)
{
	return u.x * v.x
		+ u.y * v.y
		+ u.z * v.z;
}

inline vec3 cross(const vec3& u, const vec3& v)
{
	return vec3(u.y * v.z - u.z * v.y,
				u.z * v.x - u.x * v.z,
				u.x * v.y - u.y * v.x);
}

inline vec3 unit_vector(vec3 v)
{
	return v / v.length();
}

vec3 random_in_unit_sphere() {
	while (true) {
		auto p = vec3::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

vec3 random_unit_vector() {
	return unit_vector(random_in_unit_sphere());
}

vec3 random_in_hemisphere(const vec3& normal) {
	vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

#endif