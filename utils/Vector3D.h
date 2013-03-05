#ifndef VECTOR3D
#define VECTOR3D

#include <cmath>
#include <ostream>

class Vector3D {
public:
    Vector3D():x(0), y(0), z(0) {};
    Vector3D(float a_x, float a_y, float a_z):x(a_x), y(a_y), z(a_z) {};
    void normalize(){
		float t = 1.0f / length(); 
		x *= t;
		y *= t;
		z *= t;
	}
	float length() { return sqrtf( x * x + y * y + z * z); }
	float sqlength(){ return ( x * x + y * y + z * z ); }
	float dot(const Vector3D& a) { return ( a.x * x + a.y * y + a.z * z ); }
	Vector3D cross (const Vector3D& a) const {
        return Vector3D(
            y * a.z - z * a.y,
            z * a.x - x * a.z,
            x * a.y - y * a.x
            );
    }
	void operator+= (const Vector3D& a) { x += a.x; y += a.y; z += a.z; }
	void operator-= (const Vector3D& a) { x -= a.x; y -= a.y; z -= a.z; }
	void operator*= (const Vector3D& a) { x *= a.x; y *= a.y; z *= a.z; }
	void operator*= (float a) { x *= a; y *= a; z *= a; }
	void operator/= (float a) { x /= a; y /= a; z /= a; }
	Vector3D operator- () const {return Vector3D(-x, -y, -z);}
	friend Vector3D operator+ (const Vector3D& a, const Vector3D& b) {return Vector3D( a.x + b.x, a.y + b.y, a.z + b.z );}
	friend Vector3D operator- (const Vector3D& a, const Vector3D& b) {return Vector3D( a.x - b.x, a.y - b.y, a.z - b.z );}
	// friend Vector3D operator- (const Vector3D& a, const float& b) {return Vector3D(a.x-b, a.y-b, a.z-b);}
	friend Vector3D operator+ (const Vector3D& a, const float& b    ) {return Vector3D( a.x + b, a.y + b, a.z + b );}
	friend Vector3D operator* (const Vector3D& a, const float& b    ) {return Vector3D( a.x * b, a.y * b, a.z * b );}
	friend Vector3D operator* (const Vector3D& a, const Vector3D& b) {return Vector3D( a.x * b.x, a.y * b.y, a.z * b.z );}
	// friend Vector3D operator/ (const Vector3D& a, const Vector3D& b) {return Vector3D( a.x / b.x, a.y / b.y, a.z / b.z );}
	// bool operator> (const Vector3D& b){return ( x > b.x && y > b.y && z > b.z );}
    friend std::ostream& operator<< (std::ostream& out, const Vector3D v) { out << "(" << v.x << ";" << v.y << ";" << v.z << ")";  return out;}
    float x, y, z;
};

#endif