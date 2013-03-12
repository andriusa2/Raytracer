#ifndef QUATERNION_H
#define QUATERNION_H
#include "./Vector3D.h"
#include <cmath>
class Quaternion{
public:
	Quaternion(float a, float b, float c, float d):x(a),y(b),z(c),w(d){};
	Quaternion(){};
	friend Quaternion operator*(Quaternion a, Quaternion b){
		Quaternion c;
		c.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
  		c.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
  		c.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
  		c.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
		return c;
	}
	Quaternion operator-(){
		return Quaternion(-x, -y, -z, w);
	}
	static Vector3D rotate(const Vector3D& axis, const Vector3D& point, float angle){
		float a = sinf(angle / 2);
		Quaternion q(axis.x() * a, axis.y() * a, axis.z() * a, cosf(angle / 2));
		Quaternion w(point.x(), point.y(), point.z(), 0);
		w=q * w * (-q);
		return Vector3D(w.x, w.y, w.z);
	}

private:
	float x, y, z, w;
};


#endif