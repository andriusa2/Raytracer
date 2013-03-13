#ifndef QUATERNION_H
#define QUATERNION_H
#include "./Vector3D.h"
#include <cmath>
class Quaternion{
public:
	Quaternion(float a, float b, float c, float d):x(a),y(b),z(c),w(d){};
	Quaternion(){};
	friend Quaternion operator*(const Quaternion &a, const Quaternion &b){
		Quaternion c;
		c.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
  		c.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
  		c.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
  		c.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
		return c;
	}
    void operator*=(const Quaternion & a){
        const float bx = x;
        const float by = y;
        const float bz = z;
        const float bw = w;
        x = bw*a.x + bx*a.w + by*a.z - bz*a.y;
  		y = bw*a.y - bx*a.z + by*a.w + bz*a.x;
  		z = bw*a.z + bx*a.y - by*a.x + bz*a.w;
  		w = bw*a.w - bx*a.x - by*a.y - bz*a.z;
    }
    void operator*=(const Vector3D & a){
        const float bx = x;
        const float by = y;
        const float bz = z;
        const float bw = w;
        x = bw*a.x() + by*a.z() - bz*a.y();
  		y = bw*a.y() - bx*a.z() + bz*a.x();
  		z = bw*a.z() + bx*a.y() - by*a.x();
  		w = -bx*a.x() - by*a.y() - bz*a.z();
    }
    void getVector(Vector3D & a) {
        a[0] = x;
        a[1] = y;
        a[2] = z;
    }
   
	Quaternion operator-(){
		return Quaternion(-x, -y, -z, w);
	}
    static Quaternion getRotation(const Vector3D & axis, float angle) {
        float a = sinf(angle/2);
        return Quaternion(a * axis.x(), a * axis.y(), a * axis.z(), cosf(angle / 2));
    }
    static Quaternion getRotationF(const Vector3D & axis, float angle) {
        return -getRotation(axis, angle);
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