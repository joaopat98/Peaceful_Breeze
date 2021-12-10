#include "vectors.h"

DivideByZeroException::DivideByZeroException()
{

}

float max_delta_comp = 0.0001f;

bool is_equal(float val, float target)
{
	return val < target + max_delta_comp && val > target - max_delta_comp;
}

float deg2rad(float degrees)
{
	return (float)(degrees * (M_PI / 180.0));
}

float rad2deg(float radians)
{
	return (float)(radians / (M_PI / 180.0));
}

#pragma region vec2

#pragma region constructors

vec2::vec2()
{
	x = y = 0;
}

vec2::vec2(float k)
{
	x = y = k;
}

vec2::vec2(const vec2& v)
{
	x = v.x;
	y = v.y;
}

vec2::vec2(float x, float y)
{
	this->x = x;
	this->y = y;
}

#pragma endregion

#pragma region operators

vec2 vec2::operator+(vec2 v)
{
	return vec2(x + v.x, y + v.y);
}

vec2 vec2::operator+(float f)
{
	return vec2(x + f, y + f);
}

vec2 vec2::operator-(vec2 v)
{
	return vec2(x - v.x, y - v.y);
}

vec2 vec2::operator-(float f)
{
	return vec2(x - f, y - f);
}

vec2 vec2::operator-()
{
	return vec2(-x, -y);
}

void vec2::operator+=(vec2 v)
{
	x += v.x;
	y += v.y;
}

void vec2::operator+=(float f)
{
	x += f;
	y += f;
}

void vec2::operator-=(vec2 v)
{
	x -= v.x;
	y -= v.y;
}

void vec2::operator-=(float f)
{
	x -= f;
	y -= f;
}

vec2 vec2::operator*(float f)
{
	return vec2(x * f, y * f);
}

vec2 operator+(float f, vec2 v)
{
	return v + f;
}

vec2 operator*(float f, vec2 v)
{
	return vec2(v.x * f, v.y * f);
}

void vec2::operator*=(float f)
{
	x *= f;
	y *= f;
}

bool vec2::operator==(vec2 v)
{
	return is_equal(x, v.x) && is_equal(y, v.y);
}

bool vec2::operator!=(vec2 v)
{
	return !(*this == v);
}

vec2 vec2::operator/(float f)
{
	return vec2(x / f, y / f);
}

void vec2::operator/=(float f)
{
	x /= f;
	y /= f;
}

#pragma endregion

#pragma region methods

float vec2::length()
{
	return sqrt(x * x + y * y);
}

vec2 vec2::normalize()
{
	return *this / length();
}

float vec2::quadrance()
{
	return (float) (pow(x, 2) + pow(y, 2));
}

float vec2::dot(vec2 v)
{
	return x * v.x + y * v.y;
}

void vec2::toArray(float* arr)
{
	arr[0] = x;
	arr[1] = y;
}

string vec2::toString()
{
	ostringstream ss;
	ss << "(" << x << ", " << y << ")";
	return ss.str();
}

#pragma endregion

#pragma region static methods

vec2 vec2::lerp(vec2 v1, vec2 v2, float t)
{
	return (v1 * (1 - t)) + (v2 * t);
}

float vec2::angle(vec2 from, vec2 to)
{
	return rad2deg(acos(from.dot(to) / (from.length() * to.length())));
}

#pragma endregion

#pragma region i / o

ostream& operator<<(ostream& out, vec2 v)
{
	return out << v.x << v.y;
}

istream& operator>>(istream& in, vec2& v)
{
	return in >> v.x >> v.y;
}

#pragma endregion

#pragma region converters

vec2::operator vec3()
{
	return vec3(x, y, 0);
}

vec2::operator vec4()
{
	return vec4(x, y, 0, 0);
}

#pragma endregion

#pragma endregion

#pragma region vec3

#pragma region constructors

vec3::vec3()
{
	x = y = z = 0;
}

vec3::vec3(float k)
{
	x = y = z = k;
}

vec3::vec3(const vec3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

vec3::vec3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

#pragma endregion

#pragma region operators

vec3 vec3::operator+(vec3 v)
{
	return vec3(x + v.x, y + v.y, z + v.z);
}

vec3 vec3::operator+(float f)
{
	return vec3(x + f, y + f, z + f);
}

vec3 vec3::operator-(vec3 v)
{
	return vec3(x - v.x, y - v.y, z - v.z);
}

vec3 vec3::operator-(float f)
{
	return vec3(x - f, y - f, z - f);
}

vec3 vec3::operator-()
{
	return vec3(-x, -y, -z);
}

void vec3::operator+=(vec3 v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

void vec3::operator+=(float f)
{
	x += f;
	y += f;
	z += f;
}

void vec3::operator-=(vec3 v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

void vec3::operator-=(float f)
{
	x -= f;
	y -= f;
	z -= f;
}

vec3 vec3::operator*(float f)
{
	return vec3(x * f, y * f, z * f);
}

vec3 operator+(float f, vec3 v)
{
	return v + f; \
}

vec3 operator*(float f, vec3 v)
{
	return vec3(v.x * f, v.y * f, v.z * f);
}

void vec3::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
}

vec3 vec3::operator/(float f)
{
	return vec3(x / f, y / f, z / f);
}

void vec3::operator/=(float f)
{
	x /= f;
	y /= f;
	z /= f;
}

bool vec3::operator==(vec3 v)
{
	return is_equal(x, v.x) && is_equal(y, v.y) && is_equal(z, v.z);
}

bool vec3::operator!=(vec3 v)
{
	return !(*this == v);
}

#pragma endregion

#pragma region methods

float vec3::length()
{
	return sqrt(x * x + y * y + z * z);
}

vec3 vec3::normalize()
{
	return *this / length();
}

float vec3::quadrance()
{
	return (float) (pow(x, 2) + pow(y, 2) + pow(z, 2));
}

float vec3::dot(vec3 v)
{
	return x * v.x + y * v.y + z * v.z;
}

vec3 vec3::cross(vec3 v)
{
	return vec3(
		y * v.z - z * v.y,
		z * v.x - x * v.z,
		x * v.y - y * v.x);
}

string vec3::toString()
{
	ostringstream ss;
	ss << "(" << x << ", " << y << ", " << z << ")";
	return ss.str();
}

void vec3::toArray(float* arr)
{
	arr[0] = x;
	arr[1] = y;
	arr[2] = z;
}

#pragma endregion

#pragma region static methods

vec3 vec3::lerp(vec3 v1, vec3 v2, float t)
{
	return (v1 * (1 - t)) + (v2 * t);
}

vec3 vec3::slerp(vec3 v1, vec3 v2, float t, float arc)
{
	arc = deg2rad(arc);
	t = /*(sin((1 - t) * arc) / sin(arc))  0 +*/
		(sin(t * arc) / sin(arc));
	return lerp(v1, v2, t);
}

float vec3::angle(vec3 from, vec3 to)
{
	return rad2deg(acos(from.dot(to) / (from.length() * to.length())));
}

#pragma endregion

#pragma region io

ostream& operator<<(ostream& out, vec3 v)
{
	return out << v.x << v.y << v.z;
}

istream& operator>>(istream& in, vec3& v)
{
	return in >> v.x >> v.y >> v.z;
}

#pragma endregion

#pragma region converters

vec3::operator vec2()
{
	return vec2(x, y);
}

vec3::operator vec4()
{
	return vec4(x, y, z, 0);
}

#pragma endregion

#pragma endregion

#pragma region vec4

#pragma region constructors

vec4::vec4()
{
	x = y = z = w = 0;
}

vec4::vec4(float k)
{
	x = y = z = w = k;
}

vec4::vec4(const vec4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

vec4::vec4(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

#pragma endregion

#pragma region operators

vec4 vec4::operator+(vec4 v)
{
	return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
}

vec4 vec4::operator+(float f)
{
	return vec4(x + f, y + f, z + f, w + f);
}

vec4 vec4::operator-(vec4 v)
{
	return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
}

vec4 vec4::operator-(float f)
{
	return vec4(x - f, y - f, z - f, w - f);
}

vec4 vec4::operator-()
{
	return vec4(-x, -y, -z, -w);
}

void vec4::operator+=(vec4 v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

void vec4::operator+=(float f)
{
	x += f;
	y += f;
	z += f;
	w += f;
}

void vec4::operator-=(vec4 v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

void vec4::operator-=(float f)
{
	x -= f;
	y -= f;
	z -= f;
	w -= f;
}

vec4 vec4::operator*(float f)
{
	return vec4(x * f, y * f, z * f, w * f);
}

vec4 operator+(float f, vec4 v)
{
	return v + f;
}

vec4 operator*(float f, vec4 v)
{
	return vec4(v.x * f, v.y * f, v.z * f, v.w * f);
}

void vec4::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;
}

vec4 vec4::operator/(float f)
{
	return vec4(x / f, y / f, z / f, w / f);
}

void vec4::operator/=(float f)
{
	x /= f;
	y /= f;
	z /= f;
	w /= f;
}

bool vec4::operator==(vec4 v)
{
	return is_equal(x, v.x) && is_equal(y, v.y) && is_equal(z, v.z) && is_equal(w, v.w);
}

bool vec4::operator!=(vec4 v)
{
	return !(*this == v);
}

#pragma endregion

#pragma region methods

float vec4::quadrance()
{
	return (float) (pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(w, 2));
}

float vec4::dot(vec4 v)
{
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

void vec4::toArray(float* arr)
{
	arr[0] = x;
	arr[1] = y;
	arr[2] = z;
	arr[3] = w;
}

string vec4::toString()
{
	ostringstream ss;
	ss << "(" << x << ", " << y << ", " << z << ", " << w << ")";
	return ss.str();
}

#pragma endregion

#pragma region static methods

vec4 vec4::lerp(vec4 v1, vec4 v2, float t)
{
	return (v1 * (1 - t)) + (v2 * t);
}

#pragma endregion

#pragma region i / o

ostream& operator<<(ostream& out, vec4 v)
{
	return out << v.x << v.y << v.z << v.w;
}

istream& operator>>(istream& in, vec4& v)
{
	return in >> v.x >> v.y >> v.z >> v.w;
}

float lerp(float f1, float f2, float t)
{
	return f1 * (1 - t) + f2 * t;
}

#pragma endregion

#pragma region converters

vec4::operator vec2()
{
	return vec2(x, y);
}

vec4::operator vec3()
{
	return vec3(x, y, z);
}

#pragma endregion

#pragma endregion
