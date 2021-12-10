#ifndef  QUATERNIONS_H
#define  QUATERNIONS_H


#include "matrix.h"

struct quaternion
{
	float t = 1;
	vec3 v;
	static quaternion fromAxisAngle(float angle, vec3 axis);
	static quaternion fromEuler(float x, float y, float z);
	static quaternion lerp(quaternion q1, quaternion q2, float t);
	static quaternion slerp(quaternion q0, quaternion q1, float t);
	quaternion operator + (quaternion q);
	friend quaternion operator * (float k, quaternion q);
	quaternion operator * (float f);
	quaternion operator * (quaternion q);
	vec3 operator * (vec3 v);
	bool operator == (quaternion q);
	quaternion operator / (float f);

	quaternion conjugate();
	float dot(quaternion q);
	float quadrance();
	quaternion inverse();
	float magnitude();
	quaternion normalize();
	mat3 toRotationMatrix();
	vec3 toEuler();
	void clean();

	string toString();
};

#endif
