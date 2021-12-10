#include  "quaternions.h"

quaternion quaternion::fromAxisAngle(float angle, vec3 axis)
{
	vec4 axisn = axis.normalize();

	quaternion q;
	float a = deg2rad(angle);
	q.t = cos(a / 2.0f);
	float s = sin(a / 2.0f);
	q.v.x = axisn.x * s;
	q.v.y = axisn.y * s;
	q.v.z = axisn.z * s;

	q.clean();
	return q.normalize();
}

quaternion quaternion::fromEuler(float x, float y, float z)
{
	x = deg2rad(x);
	y = deg2rad(y);
	z = deg2rad(z);
	float cy = (float)cos(z * 0.5f);
	float sy = (float)sin(z * 0.5f);
	float cp = (float)cos(y * 0.5f);
	float sp = (float)sin(y * 0.5f);
	float cr = (float)cos(x * 0.5f);
	float sr = (float)sin(x * 0.5f);

	quaternion q;
	q.t = cr * cp * cy + sr * sp * sy;
	q.v.x = sr * cp * cy - cr * sp * sy;
	q.v.y = cr * sp * cy + sr * cp * sy;
	q.v.z = cr * cp * sy - sr * sp * cy;

	return q;
}

quaternion quaternion::operator+(quaternion q)
{
	return { t + q.t, v + q.v };
}

quaternion operator*(float k, quaternion q)
{
	return { k * q.t, k * q.v };
}

quaternion quaternion::operator*(float f)
{
	return { f * t, f * v };
}

quaternion quaternion::operator*(quaternion q)
{
	return { t * q.t - v.dot(q.v), t * q.v + q.t * v + v.cross(q.v) };
}

vec3 quaternion::operator*(vec3 v)
{
	return toRotationMatrix() * v;
}

bool quaternion::operator==(quaternion q)
{
	quaternion q1 = normalize();
	quaternion q2 = q.normalize();
	return is_equal(q1.t, q2.t) && q1.v == q2.v;
}

quaternion quaternion::operator/(float f)
{
	return { t / f, v / f };
}

quaternion quaternion::conjugate()
{
	return  { t, -v };
}

float quaternion::dot(quaternion q)
{
	return t * q.t + v.dot(q.v);
}

float quaternion::quadrance()
{
	return t * t + v.dot(v);
}

quaternion quaternion::inverse()
{
	float quad = quadrance();
	if (quad == 0)
		throw NonInvertableException();
	return conjugate() / quad;
}

float quaternion::magnitude()
{
	return sqrt(quadrance());
}

quaternion quaternion::normalize()
{
	float m = magnitude();
	if (m == 0)
		throw new DivideByZeroException();
	return (*this) / magnitude();
}

mat3 quaternion::toRotationMatrix()
{
	quaternion norm = normalize();
	float xx = norm.v.x * norm.v.x;
	float xy = norm.v.x * norm.v.y;
	float xz = norm.v.x * norm.v.z;
	float xt = norm.v.x * norm.t;
	float yy = norm.v.y * norm.v.y;
	float yz = norm.v.y * norm.v.z;
	float yt = norm.v.y * norm.t;
	float zz = norm.v.z * norm.v.z;
	float zt = norm.v.z * norm.t;
	return mat3(
		1.0f - 2.0f * (yy + zz), 2.0f * (xy - zt), 2.0f * (xz + yt),
		2.0f * (xy + zt), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - xt),
		2.0f * (xz - yt), 2.0f * (yz + xt), 1.0f - 2.0f * (xx + yy)
	);
}

vec3 quaternion::toEuler()
{
	vec3 angles;

	float sinr_cosp = 2 * (t * v.x + v.y * v.z);
	float cosr_cosp = 1 - 2 * (v.x * v.x + v.y * v.y);
	angles.x = atan2(sinr_cosp, cosr_cosp);

	float sinp = 2 * (t * v.y - v.z * v.x);
	if (std::abs(sinp) >= 1)
		angles.y = (float)copysign(M_PI / 2, sinp);
	else
		angles.y = asin(sinp);

	float siny_cosp = 2 * (t * v.z + v.x * v.y);
	float cosy_cosp = 1 - 2 * (v.y * v.y + v.z * v.z);
	angles.z = atan2(siny_cosp, cosy_cosp);

	angles.x = rad2deg(angles.x);
	angles.y = rad2deg(angles.y);
	angles.z = rad2deg(angles.z);

	return angles;
}

void quaternion::clean()
{
	t = is_equal(t, 0) ? 0 : t;
	v.x = is_equal(v.x, 0) ? 0 : v.x;
	v.y = is_equal(v.y, 0) ? 0 : v.y;
	v.z = is_equal(v.z, 0) ? 0 : v.z;
}

string quaternion::toString()
{
	ostringstream ss;
	ss << "(" << t << ", " << v.x << ", " << v.y << ", " << v.z << ")\n";
	return ss.str();
}

quaternion quaternion::lerp(quaternion q1, quaternion q2, float t)
{
	return ((1 - t) * q1 + t * q2).normalize();
}

quaternion quaternion::slerp(quaternion q0, quaternion q1, float t)
{
	float angle = acos(q0.v.x * q1.v.x + q0.v.y * q1.v.y + q0.v.z * q1.v.z + q0.t * q1.t);
	float k0 = sin((1 - t) * angle) / sin(angle);
	float k1 = sin(t * angle) / sin(angle);
	quaternion qi = (q0 * k0) + (q1 * k1);
	qi = qi.normalize();
	return qi;
}
