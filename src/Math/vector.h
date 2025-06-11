#pragma once
#include <stdexcept>              // Для std::runtime_error, если заменяешь fatal_error

#define PI      3.14159265358979323846264338327950f
#define PI2     6.28318530717958647692528676655901f
#define EPSILON 0.00000001000000000000000000000000f

class vector3
{
public:
	float x, y, z;

	vector3() {}
	vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	bool operator==(const vector3& a) const
	{
		return x == a.x && y == a.y && z == a.z;
	}
	bool operator!=(const vector3& a) const
	{
		return !(*this == a);
	}

	vector3 operator+(const vector3& a) const
	{
		return vector3(x + a.x, y + a.y, z + a.z);
	}

	vector3 operator-(const vector3& a) const
	{
		return vector3(x - a.x, y - a.y, z - a.z);
	}

	vector3 operator-() const
	{
		return vector3(-x, -y, -z);
	}

	float dot_product(const vector3& a) const
	{
		return x * a.x + y * a.y + z * a.z;
	}

	vector3 cross_product(const vector3& a) const
	{
		return vector3(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
	}

	vector3 operator*(float a) const
	{
		return vector3(x * a, y * a, z * a);
	}

	vector3 operator/(float a) const
	{
		return *this * (1.0f / a);
	}

	vector3& operator+=(const vector3& a)
	{
		*this = *this + a;
		return *this;
	}

	vector3& operator-=(const vector3& a)
	{
		*this = *this - a;
		return *this;
	}

	vector3& operator*=(float a)
	{
		*this = *this * a;
		return *this;
	}

	inline float operator*(const vector3& a) const {
		return x * a.x + y * a.y + z * a.z; 
	}

	vector3& operator/=(float a)
	{
		*this = *this / a;
		return *this;
	}

	float operator[](int i) const
	{
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: return 0.0f;
		}
	}

	float& operator[](int i) {
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: throw std::out_of_range("vector3 index out of range");
		}
	}
};

class vector4
{
public:
	float x, y, z, w;

	vector4() : x(0), y(0), z(0), w(0) {}
	vector4(float _x, float _y, float _z, float _w)
		: x(_x), y(_y), z(_z), w(_w) {
	}

	bool operator==(const vector4& a) const
	{
		return x == a.x && y == a.y && z == a.z && w == a.w;
	}

	bool operator!=(const vector4& a) const
	{
		return !(*this == a);
	}

	vector4 operator+(const vector4& a) const
	{
		return vector4(x + a.x, y + a.y, z + a.z, w + a.w);
	}

	vector4 operator-(const vector4& a) const {
		return vector4(x - a.x, y - a.y, z - a.z, w - a.w);
	}

	vector4 operator-() const {
		return vector4(-x, -y, -z, -w);
	}

	float dot(const vector4& a) const {
		return x * a.x + y * a.y + z * a.z + w * a.w;
	}

	vector4 operator*(float a) const {
		return vector4(x * a, y * a, z * a, w * a);
	}

	vector4 operator/(float a) const {
		return *this * (1.0f / a);
	}

	vector4& operator+=(const vector4& a) {
		*this = *this + a;
		return *this;
	}

	vector4& operator-=(const vector4& a) {
		*this = *this - a;
		return *this;
	}

	vector4& operator*=(float a) {
		*this = *this * a;
		return *this;
	}

	vector4& operator/=(float a) {
		*this = *this / a;
		return *this;
	}

	float operator[](int i) const
	{
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		default: return 0.0f;
		}
	}

	float& operator[](int i) {
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		default: throw std::out_of_range("vector4 index out of range");
		}
	}

	float length() const
	{
		return sqrt(dot(*this));
	}

	vector4 normalized() const
	{
		float len = length();
		return len > 0.0f ? *this / len : vector4(0, 0, 0, 0);
	}
};
