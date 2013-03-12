#ifndef VECTOR3D
#define VECTOR3D

#include <cmath>
#include <ostream>
#include <istream>
class Vector3D {
public:
    Vector3D() { f[0] = 0.0f, f[1] = 0.0f, f[2] = 0.0f; }
    Vector3D(float x, float y, float z) {f[0]=x, f[1] = y, f[2] = z;}
    
    // some getters, etc
    float x() const {return f[0];}
    float y() const {return f[1];}
    float z() const {return f[2];}
    void x(float ax) {f[0] = ax;}
    void y(float ay) {f[1] = ay;}
    void z(float az) {f[2] = az;}
    float& operator[] (const unsigned int & id) {return f[id];}
    float get(const unsigned int & id) const {return f[id];}
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
	friend Vector3D operator+ (const Vector3D& a, const Vector3D& b) {return Vector3D( a.x() + b.x(), a.y() + b.y(), a.z() + b.z() );}
	friend Vector3D operator- (const Vector3D& a, const Vector3D& b) {return Vector3D( a.x() - b.x(), a.y() - b.y(), a.z() - b.z() );}
	friend Vector3D operator* (const Vector3D& a, const float& b    ) {return Vector3D( a.x() * b, a.y() * b, a.z() * b );}
    friend Vector3D operator* (const float& b, const Vector3D& a    ) {return Vector3D( a.x() * b, a.y() * b, a.z() * b );}
	friend Vector3D operator* (const Vector3D& a, const Vector3D& b) {return Vector3D( a.x() * b.x(), a.y() * b.y(), a.z() * b.z() );}
	// friend Vector3D operator/ (const Vector3D& a, const Vector3D& b) {return Vector3D( a.x / b.x, a.y / b.y, a.z / b.z );}
	// bool operator> (const Vector3D& b){return ( x > b.x && y > b.y && z > b.z );}
    friend std::ostream& operator<< (std::ostream& out, const Vector3D& v) { out << "(" << v.get(0) << ";" << v.get(1) << ";" << v.get(2) << ")";  return out;}
    friend std::istream& operator>> (std::istream& in, Vector3D& v) { in >> v[0] >> v[1] >> v[2];  return in;}
    unsigned int toRGBCol() { return (toCol(f[0]) << 16) + (toCol(f[1]) << 8) + toCol(f[2]);}
    void degToRad() { *this *= 3.14159265359f/180.f; }
private:
    int toCol(float a) { return (a < 0) ? 0 : ((a > 1) ? 255 : (int)(a * 255.0 + 0.1)); }
    float f[3];
};

#endif