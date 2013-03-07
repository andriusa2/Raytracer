#ifndef VECTOR3D
#define VECTOR3D

#include <cmath>
#include <ostream>

class Vector3D {
public:
    Vector3D():f() {};
    Vector3D(float x, float y, float z) {f[0]=x, f[1] = y, f[2] = z;}
    void normalize(){
		float t = 1.0f / length(); 
		f[0] *= t;
		f[1] *= t;
		f[2] *= t;
	}
	float length() { return sqrtf( sqlength() ); }
	float sqlength(){ return ( f[0]*f[0] + f[1]*f[1] + f[2]*f[2] ); }
	float dot(const Vector3D& a) { return ( a.f[0] * f[0] + a.f[1] * f[1] + a.f[2] * f[2] ); }
	Vector3D cross (const Vector3D& a) const {
        return Vector3D(
            f[1] * a.f[2] - f[2] * a.f[1],
            f[2] * a.f[0] - f[0] * a.f[2],
            f[0] * a.f[1] - f[1] * a.f[0]
            );
    }
	void operator+= (const Vector3D& a) { f[0] += a.f[0]; f[1] += a.f[1]; f[2] += a.f[2]; }
	void operator-= (const Vector3D& a) { f[0] -= a.f[0]; f[1] -= a.f[1]; f[2] -= a.f[2]; }
	void operator*= (const Vector3D& a) { f[0] *= a.f[0]; f[1] *= a.f[1]; f[2] *= a.f[2]; }
	void operator*= (float a) { f[0] *= a; f[1] *= a; f[2] *= a; }
	void operator/= (float a) { f[0] /= a; f[1] /= a; f[2] /= a; }
	Vector3D operator- () const {return Vector3D(-f[0], -f[1], -f[2]);}
	friend Vector3D operator+ (const Vector3D& a, const Vector3D& b) {return Vector3D( a[0] + b[0], a[1] + b[1], a[2] + b[2] );}
	friend Vector3D operator- (const Vector3D& a, const Vector3D& b) {return Vector3D( a[0] - b[0], a[1] - b[1], a[2] - b[2] );}
	friend Vector3D operator* (const Vector3D& a, const float& b    ) {return Vector3D( a[0] * b, a[1] * b, a[2] * b );}
	friend Vector3D operator* (const Vector3D& a, const Vector3D& b) {return Vector3D( a[0] * b[0], a[1] * b[1], a[2] * b[2] );}
	// friend Vector3D operator/ (const Vector3D& a, const Vector3D& b) {return Vector3D( a.x / b.x, a.y / b.y, a.z / b.z );}
	// bool operator> (const Vector3D& b){return ( x > b.x && y > b.y && z > b.z );}
    friend std::ostream& operator<< (std::ostream& out, const Vector3D& v) { out << "(" << v[0] << ";" << v[1] << ";" << v[2] << ")";  return out;}
    // some getters, etc
    float getX() const {return f[0];}
    float getY() const {return f[1];}
    float getZ() const {return f[2];}
    void setX(float x) {f[0] = x;}
    void setY(float y) {f[1] = y;}
    void setZ(float z) {f[2] = z;}
    float& operator[] (const unsigned int & id) {return f[id];}
    float get(const unsigned int & id) const {return f[id];}
private:
    float f[3];
};

#endif